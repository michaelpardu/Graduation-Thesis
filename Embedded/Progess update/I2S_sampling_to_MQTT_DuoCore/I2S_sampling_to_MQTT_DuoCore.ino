#include <driver/i2s.h>
#include <driver/adc.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define HIGH_SAMPLE_RATE 16000
#define TARGET_SAMPLE_RATE 500
#define SAMPLE_INTERVAL (HIGH_SAMPLE_RATE / TARGET_SAMPLE_RATE)
#define BATCH_SIZE 1000

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
const char* mqtt_server = "your_MQTT_BROKER";
const char* topic = "sensor/data";

WiFiClient espClient;
PubSubClient client(espClient);

uint16_t samples[BATCH_SIZE];

void setupWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

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

void taskReadSensor(void* parameter) {
  while (1) {
    int sampleIndex = 0;
    for (int i = 0; i < BATCH_SIZE * SAMPLE_INTERVAL; i++) {
      int value = readAnalogValueWithI2S();
      if (i % SAMPLE_INTERVAL == 0) {
        samples[sampleIndex++] = value;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void taskSendMQTT(void* parameter) {
  while (1) {
    if (!client.connected()) {
      reconnectMQTT();
    }
    client.loop();

    String payload = "";
    for (int i = 0; i < BATCH_SIZE; i++) {
      payload += String(samples[i]);
      if (i < BATCH_SIZE - 1) {
        payload += ",";
      }
    }

    client.publish(topic, payload.c_str());
    Serial.println("Data sent to MQTT server.");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void setup() {
  Serial.begin(115200);
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_0);
  setupWiFi();
  client.setServer(mqtt_server, 1883);
  setupI2S();

  xTaskCreatePinnedToCore(taskReadSensor, "Sensor Task", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(taskSendMQTT, "MQTT Task", 4096, NULL, 1, NULL, 1);
}

void loop() {
  // Loop is empty as tasks run independently
}
