#include "sensors/sensor_manager.h"
#include "types.h"
#include <Arduino.h>
#include "config.h"
#include "stepper.h"
#include <math.h>

extern QueueHandle_t rawQueue;

static long deploymentSteps() {
    const float circumferenceMm = PI * SPOOL_DIAMETER_MM;
    return lroundf((SENSOR_DEPTH_MM / circumferenceMm) *
                   STEPPER_HALF_STEPS_PER_REV);
}

static void averageReading(SensorReading& average) {
    SensorReading sample;
    float phTotal = 0.0f;
    float doTotal = 0.0f;
    float waterTempTotal = 0.0f;
    float pressureTotal = 0.0f;
    float airTempTotal = 0.0f;
    float humidityTotal = 0.0f;
    int phCount = 0;
    int doCount = 0;
    int waterTempCount = 0;
    int pressureCount = 0;
    int airTempCount = 0;
    int humidityCount = 0;
    bool gpsFix = false;
    double latitude = 0.0;
    double longitude = 0.0;

    for (int i = 0; i < SENSOR_AVERAGE_COUNT; ++i) {
        sensorsRead(sample);
        if (sample.ph > 0.0f) {
            phTotal += sample.ph;
            ++phCount;
        }
        if (sample.dissolvedO2 > 0.0f) {
            doTotal += sample.dissolvedO2;
            ++doCount;
        }
        if (sample.waterTempC > -100.0f) {
            waterTempTotal += sample.waterTempC;
            ++waterTempCount;
        }
        if (sample.pressureHpa > 0.0f) {
            pressureTotal += sample.pressureHpa;
            ++pressureCount;
        }
        if (sample.airTempC > -100.0f) {
            airTempTotal += sample.airTempC;
            ++airTempCount;
        }
        if (sample.airHumidity >= 0.0f) {
            humidityTotal += sample.airHumidity;
            ++humidityCount;
        }
        if (sample.gpsFix) {
            gpsFix = true;
            latitude = sample.latitude;
            longitude = sample.longitude;
        }
    }

    average.ph = phCount > 0 ? phTotal / phCount : 0.0f;
    average.dissolvedO2 = doCount > 0 ? doTotal / doCount : 0.0f;
    average.waterTempC = waterTempCount > 0 ? waterTempTotal / waterTempCount : 0.0f;
    average.pressureHpa = pressureCount > 0 ? pressureTotal / pressureCount : 0.0f;
    average.airTempC = airTempCount > 0 ? airTempTotal / airTempCount : 0.0f;
    average.airHumidity = humidityCount > 0 ? humidityTotal / humidityCount : 0.0f;
    average.latitude = latitude;
    average.longitude = longitude;
    average.gpsFix = gpsFix;
    average.timestampMs = millis();
}

void sensingTask(void* arg) {
    SensorReading reading;

    for (;;) {
        const long steps = deploymentSteps();
        Serial.printf("sensing: lowering array %ld steps\n", steps);
        stepperMove(steps, STEPPER_DEPLOY_DIRECTION);
        vTaskDelay(pdMS_TO_TICKS(SENSOR_SETTLE_MS));

        averageReading(reading);

        Serial.println("sensing: returning array to its original level");
        stepperMove(steps, -STEPPER_DEPLOY_DIRECTION);
        if (xQueueSend(rawQueue, &reading, 0) != pdTRUE) {
            Serial.println("sensing: rawQueue full");
        }

        vTaskDelay(pdMS_TO_TICKS(SENSOR_SAMPLE_INTERVAL_MS));
    }
}