#include "types.h"
#include <Arduino.h>
#include "config.h"
#include "format/format_json.h"

extern QueueHandle_t rawQueue;
extern QueueHandle_t latestQueue;
extern QueueHandle_t cloudQueue;

void formatTask(void* arg) {
    SensorReading reading;
    FormattedRecord record;

    for (;;) {
        // wait until SensorTask sends a reading
        if (xQueueReceive(rawQueue, &reading, portMAX_DELAY) == pdTRUE) {
            // keep the latest reading for DisplayTask
            xQueueOverwrite(latestQueue, &reading);
        }

        if (formatAsJson(reading, record)) {
            if (xQueueOverwrite(cloudQueue, &record) != pdTRUE) {
                Serial.println("format: unable to replace latest cloud record.");
            }
        } else {
            Serial.println("format: record too long, dropping.");
        }
    }
}