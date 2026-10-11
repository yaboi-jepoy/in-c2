#include "sensors/sensor_manager.h"
#include "types.h"
#include <Arduino.h>
#include "config.h"

extern QueueHandle_t rawQueue;

void sensingTask(void* arg) {
    SensorReading reading;
    TickType_t lastWake = xTaskGetTickCount();

    for (;;) {
        sensorsRead(reading);

        if (xQueueSend(rawQueue, &reading, 0) != pdTRUE) {
            Serial.println("sensing: rawQueue full");
        }

        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(SAMPLE_PERIOD_MS));
    }
}