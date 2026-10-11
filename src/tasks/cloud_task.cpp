#include <Arduino.h>
#include "config.h"
#include "types.h"

extern QueueHandle_t cloudQueue;

void cloudTask(void* arg) {
  FormattedRecord record;

  for (;;) {
    if (xQueueReceive(cloudQueue, &record, portMAX_DELAY) == pdTRUE) {
      // Stand-in for the real HTTP POST. This is the packet.
      // Serial.println(record.json);
    }
  }
}