#include <WiFi.h>
#include <HTTPClient.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

const char* ssid = "KTXA-AB";
const char* password = "";
const char* serverUrl = "http://10.2.39.172:3000";

const int analogPin = 34;
const int sampleRate = 500;
const int bufferSize = 500;
int buffer1[bufferSize];
int buffer2[bufferSize];
int* activeBuffer = buffer1;
int* sendingBuffer = buffer2;
volatile bool dataReady = false;
SemaphoreHandle_t bufferSemaphore;

void IRAM_ATTR sampleADC(void* arg) {
    static int index = 0;
    activeBuffer[index] = analogRead(analogPin) % 10000; // Đảm bảo giá trị chỉ có 4 chữ số
    index++;
    if (index >= bufferSize) {
        index = 0;

        // Hoán đổi buffer
        if (xSemaphoreTakeFromISR(bufferSemaphore, NULL) == pdTRUE) {
            int* temp = activeBuffer;
            activeBuffer = sendingBuffer;
            sendingBuffer = temp;
            dataReady = true;
            xSemaphoreGiveFromISR(bufferSemaphore, NULL);
        }
    }
}

void adcTask(void* parameter) {
    const esp_timer_create_args_t timerArgs = {
        .callback = &sampleADC,
        .name = "adc_timer"
    };
    esp_timer_handle_t timerHandle;
    esp_timer_create(&timerArgs, &timerHandle);
    esp_timer_start_periodic(timerHandle, 2000);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void sendDataTask(void* parameter) {
    while (1) {
        if (WiFi.status() == WL_CONNECTED && dataReady) {
            if (xSemaphoreTake(bufferSemaphore, portMAX_DELAY) == pdTRUE) {
                dataReady = false;

                HTTPClient http;
                http.begin(serverUrl);
                http.addHeader("Content-Type", "application/json");

                String payload = "{";
                for (int i = 0; i < bufferSize; i++) {
                    String formattedValue = String(sendingBuffer[i]);
                    while (formattedValue.length() < 4) {
                        formattedValue = "0" + formattedValue;
                    }
                    payload += formattedValue;
                    if (i < bufferSize - 1) payload += ",";
                }
                payload += "}";

                int httpResponseCode = http.POST(payload);
                Serial.print("HTTP Response code: ");
                Serial.println(httpResponseCode);

                http.end();
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

    bufferSemaphore = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(adcTask, "ADC Task", 1000, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(sendDataTask, "Send Data Task", 5000, NULL, 1, NULL, 1);
}

void loop() {
    vTaskDelete(NULL);
}
