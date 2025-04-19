#include <WiFi.h>
#include <HTTPClient.h>
#include "FS.h"
#include "SD_MMC.h"
#include "driver/i2s.h"
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "time.h"

const char* ssid = "CTU";
const char* password = "";
const char* serverUrl = "https://serverport.tqduy.id.vn";
const char* ntpServer = "pool.ntp.org";

#define analogPin ADC1_CHANNEL_6  // GPIO34
#define HIGH_SAMPLE_RATE 1000
#define TARGET_SAMPLE_RATE 500
#define SAMPLE_INTERVAL (HIGH_SAMPLE_RATE / TARGET_SAMPLE_RATE)
#define bufferSize 500

#define LED_GPIO 33

int buffer1[bufferSize];
int buffer2[bufferSize];
int* activeBuffer = buffer1;
int* sendingBuffer = buffer2;
volatile bool dataReady = false;
unsigned long blockTimestamp = 0;
SemaphoreHandle_t bufferSemaphore;

time_t ntpBaseTime = 0; // Thời gian NTP lúc ban đầu (giây từ Epoch)
time_t lastNtpSync = 0;
const unsigned long NTP_SYNC_INTERVAL = 12UL * 60 * 60;

QueueHandle_t dataQueue;

void setupI2S() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
    .sample_rate = HIGH_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = true
  };
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_adc_mode(ADC_UNIT_1, analogPin);
  i2s_adc_enable(I2S_NUM_0);
}

int readAnalogValueWithI2S() {
  uint16_t sample = 0;
  size_t bytesRead;
  i2s_read(I2S_NUM_0, &sample, sizeof(sample), &bytesRead, portMAX_DELAY);
  return sample & 0xFFF;
}

String getCurrentFilename() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "/data.csv";
  char filename[32];
  strftime(filename, sizeof(filename), "/%Y-%m-%d.csv", &timeinfo);
  return String(filename);
}

bool sendDataToServer(int* data, unsigned long timestamp) {
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-Key", "IzUKf0FyBbktDiRxgry6fpRg8NTpxmb8XU777DDhwqDVnuUbolhSYxSUsijwBkN2");
  http.addHeader("X-UID", "EDS");

  String payload = "{\"t\":" + String(timestamp) + ",\"values\":[";
  for (int i = 0; i < bufferSize; i++) {
    payload += String(data[i]);
    if (i < bufferSize - 1) payload += ",";
  }
  payload += "]}";

  // Set timeout for HTTP POST to 300ms
  http.setTimeout(1000);

  int code = http.POST(payload);
  Serial.print("[SEND] Response: ");
  Serial.println(code);
  http.end();
  return (code >= 200 && code < 300);
}

void adcTask(void* parameter) {
  int index = 0;
  while (true) {
    int value = readAnalogValueWithI2S();
    if (index % SAMPLE_INTERVAL == 0) {
      int sampleIndex = index / SAMPLE_INTERVAL;
      activeBuffer[sampleIndex] = value;
      if (sampleIndex == 0) blockTimestamp = millis();
    }

    index++;
    if (index >= bufferSize * SAMPLE_INTERVAL) {
      index = 0;

      if (xSemaphoreTake(bufferSemaphore, portMAX_DELAY) == pdTRUE) {
        int* temp = activeBuffer;
        activeBuffer = sendingBuffer;
        sendingBuffer = temp;
        xSemaphoreGive(bufferSemaphore);

        // Tạo bản sao buffer để push vào queue
        int* copiedBuffer = (int*)malloc(sizeof(int) * bufferSize);
        memcpy(copiedBuffer, sendingBuffer, sizeof(int) * bufferSize);
        // Ghi vào file SD
        String filename = getCurrentFilename();
        File file = SD_MMC.open(filename, FILE_APPEND);
        if (file) {
          file.print(blockTimestamp); // Thời gian bắt đầu block
          file.print(",");
          for (int i = 0; i < bufferSize; i++) {
            file.print(sendingBuffer[i]);
            if (i < bufferSize - 1) file.print(",");
          }
          file.println();
          file.close();
          Serial.println("[SD] Successfully write data to SD card.");
        } else {
          Serial.println("[SD] Error when opening the file to record.");
        }

        if (xQueueSend(dataQueue, &copiedBuffer, 0) != pdPASS) {
          Serial.println("[ERROR] Queue full, dropping data.");
          free(copiedBuffer);
        } else {
          
          Serial.printf("[QUEUE] Pushed buffer at %p\n", copiedBuffer);
        }
      }
    }
  }
}

void syncTime() {
  configTime(7 * 3600, 0, ntpServer); // GMT+7
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    Serial.println("[NTP] Time synchronized");
    ntpBaseTime = time(nullptr); // Lấy thời gian NTP hiện tại
    lastNtpSync = ntpBaseTime;
  } else {
    Serial.println("[NTP] Failed to get time");
  }
}

unsigned long getTimestamp() {
  return ntpBaseTime + (millis())/1000; // Cộng thời gian đã trôi qua từ lần đồng bộ NTP
}

void senderTask(void* parameter) {
  while (true) {
    bool online = (WiFi.status() == WL_CONNECTED);
    digitalWrite(LED_GPIO, online ? LOW : HIGH); // LOW = LED tắt khi có mạng

    if (online) {
      if (time(nullptr) - lastNtpSync > NTP_SYNC_INTERVAL) {
        syncTime();
      }

      // In queue size
      Serial.print("[QUEUE] Queue size: ");
      Serial.println(uxQueueMessagesWaiting(dataQueue));

      // Gửi block dữ liệu mới nếu có
      if (dataReady && xSemaphoreTake(bufferSemaphore, portMAX_DELAY) == pdTRUE) {
        dataReady = false;
        int* data = sendingBuffer;

        unsigned long timestamp = getTimestamp();
        bool ok = sendDataToServer(data, timestamp);

        if (!ok) {
          Serial.println("[SEND] Failed to send current buffer. Pushing to queue.");
          // Đẩy dữ liệu vào queue để gửi lại sau
          if (xQueueSend(dataQueue, &data, 0) != pdPASS) {
            Serial.println("[QUEUE] Queue full. Dropping data.");
          }
        }

        xSemaphoreGive(bufferSemaphore);
      }
        // Gửi lại dữ liệu trong queue
        int* queuedData;
        if (xQueueReceive(dataQueue, &queuedData, 0) == pdPASS) {
          unsigned long timestamp = getTimestamp();
          bool ok = sendDataToServer(queuedData, timestamp);

          if (!ok) {
            Serial.println("[SEND] Failed to resend queued data. Pushing back to queue.");
            // Gửi lỗi lại thì đẩy lại vào queue
            xQueueSend(dataQueue, &queuedData, 0);
          } else {
            free(queuedData);  // Giải phóng bộ nhớ nếu gửi thành công
          }
        }
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(LED_GPIO, OUTPUT);
  digitalWrite(LED_GPIO, HIGH); // HIGH = LED sáng (mặc định là mất mạng)

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  if (!SD_MMC.begin()) {
    Serial.println("[SD] Card Mount Failed");
    return;
  }

  setupI2S();
  syncTime();

  dataQueue = xQueueCreate(15, sizeof(int*));

  bufferSemaphore = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(adcTask, "ADC Task", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(senderTask, "Sender Task", 8192, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
