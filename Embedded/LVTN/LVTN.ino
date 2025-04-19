#include <WiFi.h>
#include <HTTPClient.h>
#include "FS.h"
#include "SD_MMC.h"
#include "driver/i2s.h"
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "time.h"
#include "esp_task_wdt.h"
#include <DCTLibrary.h>

const char* ssid = "CTU";
const char* password = "";
const char* serverUrl = "https://database.tqduy.id.vn";
const char* ntpServer = "pool.ntp.org";

#define analogPin ADC1_CHANNEL_6  // GPIO34
#define HIGH_SAMPLE_RATE 1000
#define TARGET_SAMPLE_RATE 500
#define SAMPLE_INTERVAL (HIGH_SAMPLE_RATE / TARGET_SAMPLE_RATE)
#define bufferSize 500
#define KeepOutBufferSize 150

#define LED_GPIO 33

float buffer1[bufferSize];
float buffer2[bufferSize];
float* activeBuffer = buffer1;
float* sendingBuffer = buffer2;
float output[500];

volatile bool dataReady = false;
unsigned long blockTimestamp = 0;
SemaphoreHandle_t bufferSemaphore;

time_t ntpBaseTime = 0;
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

float readAnalogValueWithI2S() {
  uint16_t sample = 0;
  size_t bytesRead;
  i2s_read(I2S_NUM_0, &sample, sizeof(sample), &bytesRead, portMAX_DELAY);
  return float(sample & 0xFFF);
}

String getCurrentFilename() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "/data.csv";
  char filename[32];
  strftime(filename, sizeof(filename), "/%Y-%m-%d.csv", &timeinfo);
  return String(filename);
}

bool sendDataToServer(float* data, unsigned long timestamp) {
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-Key", "IzUKf0FyBbktDiRxgry6fpRg8NTpxmb8XU777DDhwqDVnuUbolhSYxSUsijwBkN2");
  http.addHeader("X-UID", "EDS");

  String payload = "{\"t\":" + String(timestamp) + ",\"values\":[";
  for (int i = 0; i < KeepOutBufferSize; i++) {
    payload += String(data[i], 2); // Lấy 2 chữ số thập phân
    if (i < KeepOutBufferSize - 1) payload += ",";
  }
  payload += "]}";

  http.setTimeout(1000);
  int code = http.POST(payload);
  Serial.print("[SEND] Response: ");
  Serial.println(code);
  http.end();
  return (code >= 200 && code < 300);
}

void adcTask(void* parameter) {
  esp_task_wdt_add(NULL);
  int index = 0;
  while (true) {
    esp_task_wdt_reset();
    float value = readAnalogValueWithI2S();
    if (index % SAMPLE_INTERVAL == 0) {
      int sampleIndex = index / SAMPLE_INTERVAL;
      activeBuffer[sampleIndex] = value;
      if (sampleIndex == 0) blockTimestamp = millis();
    }

    index++;
    if (index >= bufferSize * SAMPLE_INTERVAL) {
      index = 0;

      if (xSemaphoreTake(bufferSemaphore, portMAX_DELAY) == pdTRUE) {
        // Compute DCT on activeBuffer and store the result in output
        DCTLibrary::compute_dct(activeBuffer, output, bufferSize);

        // Swap activeBuffer and sendingBuffer pointers
        float* temp = activeBuffer;
        activeBuffer = sendingBuffer;
        sendingBuffer = temp;

        xSemaphoreGive(bufferSemaphore);

        // Copy the processed data from output to a new buffer
        float* copiedBuffer = (float*)malloc(sizeof(float) * bufferSize);
        memcpy(copiedBuffer, output, sizeof(float) * bufferSize);

        // Save the processed data to the SD card
        String filename = getCurrentFilename();
        File file = SD_MMC.open(filename, FILE_APPEND);
        if (file) {
          file.print(blockTimestamp);
          file.print(",");
          for (int i = 0; i < bufferSize; i++) {
            file.print(output[i], 2);
            if (i < bufferSize - 1) file.print(",");
          }
          file.println();
          file.close();
          Serial.println("[SD] Successfully write data to SD card.");
        } else {
          Serial.println("[SD] Error when opening the file to record.");
        }

        // Push the copied buffer to the queue
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
  configTime(7 * 3600, 0, ntpServer);
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    Serial.println("[NTP] Time synchronized");
    ntpBaseTime = time(nullptr);
    lastNtpSync = ntpBaseTime;
  } else {
    Serial.println("[NTP] Failed to get time");
  }
}

unsigned long getTimestamp() {
  return ntpBaseTime + (millis()) / 1000;
}

void senderTask(void* parameter) {
  esp_task_wdt_add(NULL);
  while (true) {
    esp_task_wdt_reset();
    bool online = (WiFi.status() == WL_CONNECTED);
    digitalWrite(LED_GPIO, online ? LOW : HIGH);

    if (online) {
      if (time(nullptr) - lastNtpSync > NTP_SYNC_INTERVAL) {
        syncTime();
      }

      Serial.print("[QUEUE] Queue size: ");
      Serial.println(uxQueueMessagesWaiting(dataQueue));

      if (dataReady && xSemaphoreTake(bufferSemaphore, portMAX_DELAY) == pdTRUE) {
        dataReady = false;
        float* data = sendingBuffer;

        unsigned long timestamp = getTimestamp();
        bool ok = sendDataToServer(data, timestamp);

        if (!ok) {
          Serial.println("[SEND] Failed to send current buffer. Pushing to queue.");
          if (xQueueSend(dataQueue, &data, 0) != pdPASS) {
            Serial.println("[QUEUE] Queue full. Dropping data.");
          }
        }

        xSemaphoreGive(bufferSemaphore);
      }

      float* queuedData;
      if (xQueueReceive(dataQueue, &queuedData, 0) == pdPASS) {
        unsigned long timestamp = getTimestamp();
        bool ok = sendDataToServer(queuedData, timestamp);

        if (!ok) {
          Serial.println("[SEND] Failed to resend queued data. Pushing back to queue.");
          xQueueSend(dataQueue, &queuedData, 0);
        } else {
          free(queuedData);
        }
      }
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_GPIO, OUTPUT);
  digitalWrite(LED_GPIO, HIGH);

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

  dataQueue = xQueueCreate(60, sizeof(float*));
  bufferSemaphore = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(adcTask, "ADC Task", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(senderTask, "Sender Task", 8192, NULL, 1, NULL, 1);
  // Cấu hình watchdog timer
  esp_task_wdt_config_t wdt_config = {
    .timeout_ms = 5000,      // timeout 5 giây
    .idle_core_mask = (1 << 0) | (1 << 1),  // áp dụng cho cả core 0 và core 1
    .trigger_panic = true    // sẽ gây panic nếu hết timeout
  };

}

void loop() {
  vTaskDelete(NULL);
}
