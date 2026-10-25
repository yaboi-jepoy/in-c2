#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "types.h"
#include "comms/wifi_cloud.h"

extern QueueHandle_t cloudQueue;

void cloudTask(void* arg) {
  FormattedRecord record;
  TickType_t lastUpload = xTaskGetTickCount();
  cloudInit();

  for (;;) {
    vTaskDelayUntil(&lastUpload, pdMS_TO_TICKS(CLOUD_UPLOAD_PERIOD_MS));

    if (xQueueReceive(cloudQueue, &record, 0) == pdTRUE) {
      if (!cloudUpload(record)) {
        if (WiFi.status() != WL_CONNECTED) {
          if (backupRecord(record)) {
            Serial.println("cloud: offline; latest record saved to backup");
          } else {
            Serial.println("error: offline and backup failed");
          }
        } else {
          Serial.println("error: Firebase upload failed while Wi-Fi is connected");
        }
      }
    }

    cloudService();
  }
}