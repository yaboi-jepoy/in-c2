#include "sensor_manager.h"
#include <Arduino.h>
#include "ph.h"
#include "barometer.h"

void sensorsInit() {
    phInit();
    if (!barometerInit()) {
        Serial.println("Barometer not found. Check wiring and address.");
    }
}

// temporarily assigns values to the "reading" variable
void sensorsRead(SensorReading& reading) {
    reading.timestampMs = millis();
    reading.ph = phRead();
    reading.dissolvedO2 = 6.5f;
    reading.waterTempC = 28.0f;

    //
    if (!barometerRead(reading.airTempC, reading.pressureHpa, reading.airHumidity)) {
        Serial.println("BME280 read failed");
    }

    // reading.pressureHpa = 1012.0f;
    // reading.airTempC = 29.0f;
    // reading.airHumidity = 75.0f;

    // GPS
    reading.latitude = 14.5995;
    reading.longitude = 120.9842;
    reading.gpsFix = true;
}