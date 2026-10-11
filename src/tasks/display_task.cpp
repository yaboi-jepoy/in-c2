// tasks/display_task.cpp
#include <Arduino.h>
#include "config.h"
#include "types.h"
#include "display/oled.h"

extern QueueHandle_t latestQueue;

void displayTask(void* arg) {
  SensorReading r;
  for (;;) {
    if (xQueuePeek(latestQueue, &r, portMAX_DELAY) == pdTRUE) {
      oledShowReading(r);
    }
    vTaskDelay(pdMS_TO_TICKS(500));   // redraw twice a second, not on every reading
  }
}