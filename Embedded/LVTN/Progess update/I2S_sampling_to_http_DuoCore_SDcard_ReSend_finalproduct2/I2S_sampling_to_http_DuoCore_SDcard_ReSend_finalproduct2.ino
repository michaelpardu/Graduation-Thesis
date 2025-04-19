#include <WiFi.h>
#include <HTTPClient.h>
#include "FS.h"
#include "SD_MMC.h"
#include "driver/i2s.h"
#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "time.h"

const char* ssid = "KTXA-AB";
const char* password = "";
const char* serverUrl = "http://10.2.38.231:3800";
const char* ntpServer = "pool.ntp.org";

#define analogPin ADC1_CHANNEL_6  // GPIO34
#define HIGH_SAMPLE_RATE 4000
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
unsigned long lastMillis = 0;
time_t lastNtpSync = 0;
const unsigned long NTP_SYNC_INTERVAL = 12UL * 60 * 60;

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

void saveToSD(int* data, unsigned long timestamp, String filename) {
  File file = SD_MMC.open(filename, FILE_APPEND);
  if (!file) {
    Serial.println("[ERROR] Failed to open file for writing.");
    return;
  }
  file.print(timestamp);
  for (int i = 0; i < bufferSize; i++) {
    file.print(",");
    file.print(data[i]);
  }
  file.println();
  file.close();
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
  
  int code = http.POST(payload);
  Serial.print("[SEND] Response: ");
  Serial.println(code);
  http.end();
  return (code >= 200 && code < 300);
}

void resendFailedData() {
  File root = SD_MMC.open("/");
  while (true) {
    File file = root.openNextFile();
    if (!file) break;

    String fileName = String(file.name());
    if (!file.isDirectory() && fileName.startsWith("/retry_")) {
      Serial.println("[RESEND] Processing file: " + fileName);

      bool resendSuccess = true;
      while (file.available()) {
        String line = file.readStringUntil('\n');
        int commaIdx = line.indexOf(',');
        if (commaIdx < 0) continue;

        unsigned long timestamp = line.substring(0, commaIdx).toInt();
        int values[bufferSize];
        int idx = 0;

        int start = commaIdx + 1;
        while (idx < bufferSize && start < line.length()) {
          int end = line.indexOf(',', start);
          if (end == -1) end = line.length();
          values[idx++] = line.substring(start, end).toInt();
          start = end + 1;
        }

        Serial.print("[RESEND] Attempting resend at t=");
        Serial.println(timestamp);

        if (sendDataToServer(values, timestamp)) {
          Serial.println("[RESEND] ✔ Sent block at t=" + String(timestamp));
        } else {
          Serial.println("[RESEND] ❌ Failed at t=" + String(timestamp));
          resendSuccess = false;
          break;
        }
      }

      file.close();
      if (resendSuccess) {
        SD_MMC.remove(fileName);
        Serial.println("[RESEND] ✅ File done and deleted: " + fileName);
      } else {
        Serial.println("[RESEND] 🔁 Retrying next time: " + fileName);
      }
    }
  }
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
        dataReady = true;
        xSemaphoreGive(bufferSemaphore);
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
  return ntpBaseTime + (millis() - lastMillis) / 1000; // Cộng thời gian đã trôi qua từ lần đồng bộ NTP
}

void senderTask(void* parameter) {
  while (true) {
    bool online = (WiFi.status() == WL_CONNECTED);
    digitalWrite(LED_GPIO, online ? LOW : HIGH); // LOW = LED tắt khi có mạng

    if (online) {
      // Hiển thị RSSI định kỳ
      Serial.print("[WiFi] RSSI: ");
      Serial.println(WiFi.RSSI());

      resendFailedData();
      if (time(nullptr) - lastNtpSync > NTP_SYNC_INTERVAL) {
        syncTime();
      }

      if (dataReady && xSemaphoreTake(bufferSemaphore, portMAX_DELAY) == pdTRUE) {
        dataReady = false;
        int* data = sendingBuffer;

        // Thay đổi cách tính timestamp
        unsigned long timestamp = getTimestamp();
        lastMillis = millis(); // Cập nhật thời gian hiện tại sau khi gửi

        saveToSD(data, timestamp, getCurrentFilename());
        bool ok = sendDataToServer(data, timestamp);
        if (!ok) {
          String failName = "/retry_" + String(millis()) + ".csv";
          saveToSD(data, timestamp, failName);
          Serial.println("[SEND] Failed, saved to retry.");
        }
        xSemaphoreGive(bufferSemaphore);
      }
    }

    // In ra lượng RAM tiêu thụ
    Serial.print("[MEMORY] Free heap: ");
    Serial.println(ESP.getFreeHeap());
    Serial.print("[MEMORY] Max free block: ");
    Serial.println(ESP.getMaxAllocHeap());
    Serial.print("[MEMORY] Total heap: ");
    Serial.println(ESP.getHeapSize());

    vTaskDelay(pdMS_TO_TICKS(1000));
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

  // Hiển thị RSSI sau khi kết nối WiFi thành công
  Serial.print("[WiFi] RSSI: ");
  Serial.println(WiFi.RSSI());

  if (!SD_MMC.begin()) {
    Serial.println("[SD] Card Mount Failed");
    return;
  }
 
  setupI2S();
  syncTime();

  bufferSemaphore = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(adcTask, "ADC Task", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(senderTask, "Sender Task", 8192, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
