#include "types.h"
#include <Arduino.h>

extern QueueHandle_t rawQueue;
extern QueueHandle_t latestQueue;

void formatTask(void* arg) {
    SensorReading reading;

    for (;;) {
        if (xQueueReceive(rawQueue, &reading, portMAX_DELAY) == pdTRUE) {
            xQueueOverwrite(latestQueue, &reading);
        }
    }
}