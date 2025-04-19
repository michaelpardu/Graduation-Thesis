#include <WiFi.h>
#include <HTTPClient.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

const char* ssid = "KTXA-AB";
const char* password = "";
const char* serverUrl = "http://10.2.39.172:3000"; // Đổi IP và cổng theo server của bạn

const int analogPin = 34; // Chân ADC
const int sampleRate = 500; // 500 mẫu mỗi giây
const int bufferSize = 500;
int buffer[bufferSize];
volatile bool dataReady = false;
SemaphoreHandle_t bufferSemaphore;

// Hàm đọc ADC
void IRAM_ATTR sampleADC(void* arg) {  // Chú ý kiểu tham số ở đây
    static int index = 0;
    buffer[index] = analogRead(analogPin);
    index++;
    if (index >= bufferSize) {
        index = 0;
        dataReady = true;  // Đánh dấu rằng dữ liệu đã sẵn sàng
    }
}

// Task để lấy mẫu ADC
void adcTask(void* parameter) {
    // Cấu hình timer để đọc ADC mỗi 2ms (500 mẫu/giây)
    const esp_timer_create_args_t timerArgs = {
        .callback = &sampleADC, // Hàm gọi khi timer hết hạn
        .name = "adc_timer"
    };
    esp_timer_handle_t timerHandle;
    esp_timer_create(&timerArgs, &timerHandle);
    esp_timer_start_periodic(timerHandle, 2000);  // Gọi callback mỗi 2ms

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(100)); // Để task chạy liên tục
    }
}

// Task để gửi dữ liệu lên server
void sendDataTask(void* parameter) {
    while (1) {
        if (WiFi.status() == WL_CONNECTED && dataReady) {
            if (xSemaphoreTake(bufferSemaphore, portMAX_DELAY) == pdTRUE) {
                dataReady = false;

                // Tạo HTTP request để gửi dữ liệu
                HTTPClient http;
                http.begin(serverUrl);
                http.addHeader("Content-Type", "application/json");

                String payload = "{[";
                for (int i = 0; i < bufferSize; i++) {
                    payload += String(buffer[i]);
                    if (i < bufferSize - 1) payload += ",";
                }
                payload += "]}";

                int httpResponseCode = http.POST(payload);
                Serial.print("HTTP Response code: ");
                Serial.println(httpResponseCode);

                http.end();
                xSemaphoreGive(bufferSemaphore);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Thời gian giữa các lần gửi
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    // Đợi kết nối WiFi
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");

    // Khởi tạo Semaphore để đồng bộ các task
    bufferSemaphore = xSemaphoreCreateMutex();

    // Tạo các task FreeRTOS
    xTaskCreatePinnedToCore(adcTask, "ADC Task", 1000, NULL, 1, NULL, 0);  // Task đọc ADC trên core 0
    xTaskCreatePinnedToCore(sendDataTask, "Send Data Task", 5000, NULL, 1, NULL, 1);  // Task gửi dữ liệu lên server trên core 1
}

void loop() {
    vTaskDelete(NULL); // Không sử dụng loop() khi dùng FreeRTOS
}
