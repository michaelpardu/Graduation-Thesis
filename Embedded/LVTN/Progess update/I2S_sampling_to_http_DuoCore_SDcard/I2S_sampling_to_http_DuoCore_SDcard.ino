  #include <WiFi.h>
  #include <HTTPClient.h>
  #include "FS.h"
  #include "SD_MMC.h"
  #include "driver/i2s.h"
  #include "driver/adc.h"
  #include "freertos/FreeRTOS.h"
  #include "freertos/task.h"

  const char* ssid = "KTXA-AB";
  const char* password = "";
  const char* serverUrl = "http://10.2.38.231:3800";

  #define analogPin ADC1_CHANNEL_6  // GPIO34
  #define HIGH_SAMPLE_RATE 16000
  #define TARGET_SAMPLE_RATE 500
  #define SAMPLE_INTERVAL (HIGH_SAMPLE_RATE / TARGET_SAMPLE_RATE)
  #define bufferSize 500

  int buffer1[bufferSize];
  int buffer2[bufferSize];
  int* activeBuffer = buffer1;
  int* sendingBuffer = buffer2;
  volatile bool dataReady = false;
  unsigned long blockTimestamp = 0;

  SemaphoreHandle_t bufferSemaphore;

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

  void adcTask(void* parameter) {
    int index = 0;
    while (true) {
      int value = readAnalogValueWithI2S();

      if (index % SAMPLE_INTERVAL == 0) {
        int sampleIndex = index / SAMPLE_INTERVAL;
        activeBuffer[sampleIndex] = value;

        // Ghi timestamp cho block khi ghi mẫu đầu tiên
        if (sampleIndex == 0) {
          blockTimestamp = millis();
        }
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

  void saveToSD(int* data, unsigned long timestamp) {
    File file = SD_MMC.open("/data.csv", FILE_APPEND);
    if (!file) {
      Serial.println("Failed to open file for writing");
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

  void sendDataToServer(int* data, unsigned long timestamp) {
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
    Serial.print("HTTP Response: ");
    Serial.println(code);
    http.end();
  }

  void senderTask(void* parameter) {
    while (true) {
      if (WiFi.status() == WL_CONNECTED && dataReady) {
        if (xSemaphoreTake(bufferSemaphore, portMAX_DELAY) == pdTRUE) {
          dataReady = false;
          int* data = sendingBuffer;
          unsigned long timestamp = blockTimestamp;

          saveToSD(data, timestamp);
          sendDataToServer(data, timestamp);

          xSemaphoreGive(bufferSemaphore);
        }
      }
      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }

  void setup() {
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi connected");

    if (!SD_MMC.begin()) {
      Serial.println("Card Mount Failed");
      return;
    }

    setupI2S();

    bufferSemaphore = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(adcTask, "ADC Task", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(senderTask, "Sender Task", 8192, NULL, 1, NULL, 1);
  }

  void loop() {
    vTaskDelete(NULL);
  }
