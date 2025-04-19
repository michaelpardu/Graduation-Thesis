#include <WiFi.h>
#include <HTTPClient.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/i2s.h>
#include <driver/adc.h>
#include <Preferences.h>

const char* ssid = "KTXA-AB";
const char* password = "";
const char* serverUrl = "https://database.tqduy.id.vn/";

#define analogPin ADC1_CHANNEL_6  // GPIO34
#define HIGH_SAMPLE_RATE 16000
#define TARGET_SAMPLE_RATE 500
#define SAMPLE_INTERVAL (HIGH_SAMPLE_RATE / TARGET_SAMPLE_RATE)
#define bufferSize 500
#define MAX_FAILED_PACKETS 5
#define ERROR_LED_PIN 2

int buffer1[bufferSize];
int buffer2[bufferSize];
int* activeBuffer = buffer1;
int* sendingBuffer = buffer2;
volatile bool dataReady = false;
SemaphoreHandle_t bufferSemaphore;

Preferences preferences;
int failedPacketCount = 0;

void saveDataToFlash(const String& payload) {
  preferences.begin("data", false);
  int packetCount = preferences.getInt("packetCount", 0);
  String key = "packet" + String(packetCount);
  preferences.putString(key.c_str(), payload); // Convert key to const char*
  preferences.putInt("packetCount", packetCount + 1);
  preferences.end();
  Serial.println("Saved data to flash: " + payload);
}

String getDataFromFlash(int index) {
  preferences.begin("data", true);
  String key = "packet" + String(index);
  String packet = preferences.getString(key.c_str(), ""); // Convert key to const char*
  preferences.end();
  Serial.println("Retrieved data from flash (index " + String(index) + "): " + packet);
  return packet;
}

void clearFlashData() {
  preferences.begin("data", false);
  preferences.clear();
  preferences.end();
  Serial.println("Cleared all data from flash.");
}

void handleFailedPacket(const String& payload) {
  Serial.println("Handling failed packet: " + payload);
  saveDataToFlash(payload);
  failedPacketCount++;
  Serial.println("Failed packet count: " + String(failedPacketCount));
  if (failedPacketCount > MAX_FAILED_PACKETS) {
    Serial.println("Max failed packets exceeded. Halting program.");
    digitalWrite(ERROR_LED_PIN, HIGH);
    while (true) {
      delay(1000); // Halt program until restart
    }
  }
}

void resendFailedPackets() {
  preferences.begin("data", true);
  int packetCount = preferences.getInt("packetCount", 0);
  preferences.end();
  Serial.println("Resending failed packets. Total packets: " + String(packetCount));

  for (int i = 0; i < packetCount; i++) {
    String payload = getDataFromFlash(i);
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-API-Key", "IzUKf0FyBbktDiRxgry6fpRg8NTpxmb8XU777DDhwqDVnuUbolhSYxSUsijwBkN2");
    http.addHeader("X-UID", "EDS");

    int httpResponseCode = http.POST(payload);
    Serial.println("Resend HTTP Response code: " + String(httpResponseCode));
    if (httpResponseCode == 200) {
      preferences.begin("data", false);
      String key = "packet" + String(i);
      preferences.putString(key.c_str(), ""); // Convert key to const char*
      preferences.end();
      Serial.println("Packet " + String(i) + " marked as sent.");
    } else {
      handleFailedPacket(payload);
      break; // Stop resending if one fails
    }
    http.end();
  }
}

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
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_PIN_NO_CHANGE,
    .ws_io_num = I2S_PIN_NO_CHANGE,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  i2s_set_pin(I2S_NUM_0, &pin_config);
  i2s_set_adc_mode(ADC_UNIT_1, analogPin);
  i2s_adc_enable(I2S_NUM_0);
}

int readAnalogValueWithI2S() {
  uint16_t sample = 0;
  size_t bytesRead;
  esp_err_t result = i2s_read(I2S_NUM_0, &sample, sizeof(sample), &bytesRead, portMAX_DELAY);
  if (result != ESP_OK) {
    Serial.print("I2S Read Error: ");
    Serial.println(result);
  }
  return sample & 0xFFF;  // Chỉ lấy 12-bit dữ liệu
}

void adcTask(void* parameter) {
  int index = 0;
  while (true) {
    int value = readAnalogValueWithI2S();
    if (index % SAMPLE_INTERVAL == 0) {
      activeBuffer[index / SAMPLE_INTERVAL] = value;
    }
    index++;
    if (index >= bufferSize * SAMPLE_INTERVAL) {
      index = 0;
      if (xSemaphoreTake(bufferSemaphore, portMAX_DELAY) == pdTRUE) {
        int* temp = activeBuffer;
        activeBuffer = sendingBuffer;
        sendingBuffer = temp;
        dataReady = true;
        Serial.println("ADC buffer ready for sending.");
        xSemaphoreGive(bufferSemaphore);
      }
    }
  }
}

void sendDataTask(void* parameter) {
  while (true) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("WiFi connected. Checking for failed packets...");
      resendFailedPackets(); // Attempt to resend failed packets
      if (dataReady) {
        if (xSemaphoreTake(bufferSemaphore, portMAX_DELAY) == pdTRUE) {
          dataReady = false;

          HTTPClient http;
          http.begin(serverUrl);
          http.addHeader("Content-Type", "application/json");
          http.addHeader("X-API-Key", "IzUKf0FyBbktDiRxgry6fpRg8NTpxmb8XU777DDhwqDVnuUbolhSYxSUsijwBkN2");
          http.addHeader("X-UID", "EDS");
          String payload = "{\"data\":[";

          for (int i = 0; i < bufferSize; i++) {
            payload += String(sendingBuffer[i]);
            if (i < bufferSize - 1) payload += ",";
          }
          payload += "]}";

          Serial.println("Sending payload: " + payload);
          int httpResponseCode = http.POST(payload);
          Serial.println("HTTP Response code: " + String(httpResponseCode));
          if (httpResponseCode != 200) {
            handleFailedPacket(payload);
          }

          http.end();
          xSemaphoreGive(bufferSemaphore);
        }
      }
    } else {
      Serial.println("WiFi not connected.");
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ERROR_LED_PIN, OUTPUT);
  digitalWrite(ERROR_LED_PIN, LOW);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Clear flash data on restart
  Serial.println("Clearing flash data on restart...");
  clearFlashData();

  // Cấu hình ADC
  Serial.println("Configuring ADC...");
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(analogPin, ADC_ATTEN_DB_0);
  setupI2S();

  bufferSemaphore = xSemaphoreCreateMutex();

  Serial.println("Starting tasks...");
  xTaskCreatePinnedToCore(adcTask, "ADC Task", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(sendDataTask, "Send Data Task", 4096, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}
