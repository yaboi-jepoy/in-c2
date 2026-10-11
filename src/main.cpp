#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "types.h"
#include "sensors/sensor_manager.h"
#include "display/oled.h"

QueueHandle_t rawQueue;
QueueHandle_t cloudQueue;
QueueHandle_t latestQueue;

void sensingTask(void* arg);
void formatTask(void* arg);
// void cloudTask(void* arg);
void displayTask(void* arg);

void setup() {
    Serial.begin(9600);
    Wire.begin(I2C_SDA, I2C_SCL);

    // init sensors
    sensorsInit();

    // init display
    if (!oledInit()) {
        Serial.println("Screen not found. Check wiring and address.");
    }

    rawQueue    = xQueueCreate(3, sizeof(SensorReading));
    // cloudQueue  = xQueueCreate(CLOUD_QUEUE_LEN, sizeof(FormattedRecord));
    latestQueue = xQueueCreate(1, sizeof(SensorReading));

    if (rawQueue == nullptr || latestQueue == nullptr) {
        Serial.println("Queue creation failed.");
        return;
    }

    xTaskCreatePinnedToCore(sensingTask, "sense",   4096, NULL, 3, NULL, 1);
    xTaskCreatePinnedToCore(formatTask,  "format",  4096, NULL, 2, NULL, 1);
    // xTaskCreatePinnedToCore(cloudTask,   "cloud",   8192, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(displayTask, "display", 4096, NULL, 1, NULL, 1);
}

void loop() {
    vTaskDelay(portMAX_DELAY);
}