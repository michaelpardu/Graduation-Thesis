#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include <driver/i2s.h>
#include <math.h>

const int analogPin = 34; // Chân ADC
const int sampleRate = 500; // 500 mẫu mỗi giây
const int bufferSize = 500;
int buffer[bufferSize];
volatile bool dataReady = false;
SemaphoreHandle_t bufferSemaphore;

void setupI2S() {
    i2s_config_t i2s_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
        .sample_rate = 16000, // Cao hơn để lấy mẫu chính xác
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = true
    };
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_PIN_NO_CHANGE,
        .ws_io_num = I2S_PIN_NO_CHANGE,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_6); // GPIO34
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
    return sample;
}

// Hàm đọc ADC
void IRAM_ATTR sampleADC(void* arg) {
    static int index = 0;
    buffer[index] = readAnalogValueWithI2S(); // Thay analogRead bằng I2S
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

void computeDCT(const int* input, float* output, int size) {
    for (int k = 0; k < size; k++) {
        float sum = 0.0;
        for (int n = 0; n < size; n++) {
            sum += input[n] * cos((M_PI / size) * k * (n + 0.5));
        }
        output[k] = sum;
    }
}

// Task để xử lý dữ liệu
void processDataTask(void* parameter) {
    static float dctBuffer[bufferSize]; // Buffer to store DCT results
    while (1) {
        if (dataReady) {
            if (xSemaphoreTake(bufferSemaphore, portMAX_DELAY) == pdTRUE) {
                dataReady = false;

                // Compute DCT
                computeDCT(buffer, dctBuffer, bufferSize);

                String payload = "[";
                for (int i = 0; i < bufferSize; i++) {
                    payload += String(dctBuffer[i], 2); // Format with 2 decimal places
                    if (i < bufferSize - 1) payload += ",";
                }
                payload += "]";

                Serial.println("Payload:");
                Serial.println(payload);

                xSemaphoreGive(bufferSemaphore);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Thời gian giữa các lần xử lý
    }
}

void setup() {
    Serial.begin(115200);
    setupI2S(); // Cấu hình I2S

    // Khởi tạo Semaphore để đồng bộ các task
    bufferSemaphore = xSemaphoreCreateMutex();

    // Tạo các task FreeRTOS
    xTaskCreatePinnedToCore(adcTask, "ADC Task", 1000, NULL, 1, NULL, 0);  // Task đọc ADC trên core 0
    xTaskCreatePinnedToCore(processDataTask, "Process Data Task", 5000, NULL, 1, NULL, 1);  // Task xử lý dữ liệu trên core 1
}

void loop() {
    vTaskDelete(NULL); // Không sử dụng loop() khi dùng FreeRTOS
}
