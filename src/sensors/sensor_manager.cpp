#include "sensor_manager.h"
#include <Arduino.h>

void sensorsInit() {

}

// temporarily assigns values to the "reading" variable
void sensorsRead(SensorReading& reading) {
    reading.timestampMs = millis();
    reading.ph = 7.2f;
    reading.dissolvedO2 = 6.5f;
    reading.waterTempC = 28.0f;
    reading.pressureHpa = 1012.0f;
    reading.airTempC = 29.0f;
    reading.airHumidity = 75.0f;
    reading.latitude = 14.5995;
    reading.longitude = 120.9842;
    reading.gpsFix = true;
}