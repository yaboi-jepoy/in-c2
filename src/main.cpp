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
void cloudTask(void* arg);
void displayTask(void* arg);

// helper to diagnose task run issues
static void startTask(TaskFunction_t fn, const char* name,
                      uint32_t stack, UBaseType_t prio, BaseType_t core) {
  BaseType_t result = xTaskCreatePinnedToCore(fn, name, stack, NULL, prio, NULL, core);
  if (result == pdPASS) {
    Serial.printf("started task: %s\n", name);
  } else {
    Serial.printf("FAILED to start task: %s\n", name);
  }
}

void setup() {
    Serial.begin(115200);
    Wire.begin(I2C_SDA, I2C_SCL);

    // init sensors
    sensorsInit();

    // init display
    if (!oledInit()) {
        Serial.println("Screen not found. Check wiring and address.");
    }

    rawQueue    = xQueueCreate(RAW_QUEUE_LEN,  sizeof(SensorReading));
    cloudQueue  = xQueueCreate(CLOUD_QUEUE_LEN, sizeof(FormattedRecord));
    latestQueue = xQueueCreate(1, sizeof(SensorReading));

    if (rawQueue == nullptr || latestQueue == nullptr || cloudQueue == nullptr) {
        Serial.println("Queue creation failed.");
        return;
    }

    startTask(sensingTask, "sense",   4096, 3, 1);
    startTask(formatTask,  "format",  4096, 2, 1);
    startTask(cloudTask,   "cloud",   8192, 1, 0);
    startTask(displayTask, "display", 4096, 1, 1);

    // print free memory in the heap
    Serial.printf("free heap after setup: %u bytes\n", ESP.getFreeHeap());
}

void loop() {
    vTaskDelay(portMAX_DELAY);
}