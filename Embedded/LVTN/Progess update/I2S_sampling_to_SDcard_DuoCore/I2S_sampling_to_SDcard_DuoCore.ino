#include <driver/i2s.h>
#include <driver/adc.h>
#include <SPI.h>
#include <SD.h>
#include "freertos/queue.h"

#define HIGH_SAMPLE_RATE 16000
#define TARGET_SAMPLE_RATE 500
#define SAMPLE_INTERVAL (HIGH_SAMPLE_RATE / TARGET_SAMPLE_RATE)
#define DURATION_SECONDS 1
#define TOTAL_SAMPLES (TARGET_SAMPLE_RATE * DURATION_SECONDS)
#define SD_CS_PIN 5 // Chân chip select cho SD Card

uint16_t samples[TOTAL_SAMPLES];
QueueHandle_t dataQueue;
File dataFile;

void setupI2S() {
  i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
    .sample_rate = HIGH_SAMPLE_RATE,
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
  i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_6);
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

void i2sTask(void *parameter) {
  while (true) {
    uint16_t buffer[TOTAL_SAMPLES];
    int sampleIndex = 0;
    for (int i = 0; i < TOTAL_SAMPLES * SAMPLE_INTERVAL; i++) {
      int value = readAnalogValueWithI2S();
      if (i % SAMPLE_INTERVAL == 0) {
        buffer[sampleIndex++] = value;
      }
    }
    xQueueSend(dataQueue, buffer, portMAX_DELAY);
  }
}

void sdTask(void *parameter) {
  while (true) {
    uint16_t buffer[TOTAL_SAMPLES];
    if (xQueueReceive(dataQueue, buffer, portMAX_DELAY) == pdTRUE) {
      dataFile = SD.open("data.csv", FILE_APPEND);
      if (dataFile) {
        for (int i = 0; i < TOTAL_SAMPLES; i++) {
          dataFile.print(buffer[i]);
          if (i < TOTAL_SAMPLES - 1) {
            dataFile.print(", ");
          }
        }
        dataFile.println();
        dataFile.close();
        Serial.println("Data written to SD card.");
      } else {
        Serial.println("Error opening file on SD card.");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_0);
  setupI2S();

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD Card initialization failed!");
    return;
  }
  Serial.println("SD Card initialized.");

  dataQueue = xQueueCreate(5, sizeof(samples));

  xTaskCreatePinnedToCore(i2sTask, "I2S Task", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(sdTask, "SD Task", 4096, NULL, 1, NULL, 1);
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}