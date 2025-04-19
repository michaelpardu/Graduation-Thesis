#include <driver/i2s.h>
#include <driver/adc.h>

#define HIGH_SAMPLE_RATE 16000
#define TARGET_SAMPLE_RATE 500
#define SAMPLE_INTERVAL (HIGH_SAMPLE_RATE / TARGET_SAMPLE_RATE)
#define DURATION_SECONDS 1
#define TOTAL_SAMPLES (TARGET_SAMPLE_RATE * DURATION_SECONDS)

uint16_t samples[TOTAL_SAMPLES];

void setupI2S() {
  // Cấu hình cho I2S
  i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
    .sample_rate = HIGH_SAMPLE_RATE, 
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = true // Sử dụng audio PLL
  };
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_PIN_NO_CHANGE,
    .ws_io_num = I2S_PIN_NO_CHANGE,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_PIN_NO_CHANGE // Không cần đặt chân cụ thể cho ADC
  };

  // Khởi tạo I2S
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_6); // Kênh ADC1_6 tương ứng với GPIO34
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

void setup() {
  Serial.begin(115200);
  // Cấu hình ADC
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_0);
  setupI2S();
}

void loop() {
  // Lưu trữ các giá trị vào mảng
  int sampleIndex = 0;
  for (int i = 0; i < TOTAL_SAMPLES * SAMPLE_INTERVAL; i++) {
    int value = readAnalogValueWithI2S();
    if (i % SAMPLE_INTERVAL == 0) {
      samples[sampleIndex++] = value;
    }
  }
  // In các giá trị dưới dạng CSV ngang
  for (int i = 0; i < TOTAL_SAMPLES; i++) {
    Serial.print(samples[i]);
    if (i < TOTAL_SAMPLES - 1) {
      Serial.print(", ");
    }
  }
  Serial.println();
  Serial.println();
  // Đợi một chút trước khi thực hiện lại
  delay(1000); // Delay 1 giây để tránh việc in ra quá nhanh
}
