#include "sensor_manager.h"
#include <Arduino.h>
#include "ph.h"
#include "barometer.h"
#include "dissolved_oxygen.h"
#include "temperature.h"

void sensorsInit() {
    phInit();
    dissolvedOxygenInit();
    if (!barometerInit()) {
        Serial.println("Barometer not found. Check wiring and address.");
    }
    if (!temperatureInit()) {
        Serial.println("DS18B20 not found. Check wiring and pull-up resistor.");
    }
}

// temporarily assigns values to the "reading" variable
void sensorsRead(SensorReading& reading) {
    reading.timestampMs = millis();
    reading.ph = phRead();
    
    if (!temperatureRead(reading.waterTempC)) {
        Serial.println("DS18B20 read failed");
    }
    
    reading.dissolvedO2 = dissolvedOxygenRead(reading.waterTempC);
    
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