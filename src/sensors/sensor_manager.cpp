#include "sensor_manager.h"
#include <Arduino.h>
#include "ph.h"
#include "barometer.h"
#include "dissolved_oxygen.h"
#include "temperature.h"
#include "gps.h"
#include "ads1115.h"

void sensorsInit() {
    ads1115Init();
    phInit();
    dissolvedOxygenInit();
    gpsInit();
    if (!barometerInit()) {
        Serial.println("Barometer not found. Check wiring and address.");
    }
    if (!temperatureInit()) {
        Serial.println("DS18B20 not found. Check wiring and pull-up resistor.");
    }
}

// temporarily assigns values to the "reading" variable
void sensorsRead(SensorReading& reading) {
    static bool waterTemperatureErrorReported = false;
    static bool barometerErrorReported = false;
    reading.timestampMs = millis();
    reading.ph = phRead();
    
    if (!temperatureRead(reading.waterTempC)) {
        if (!waterTemperatureErrorReported) {
            Serial.println("error: DS18B20 read failed");
            waterTemperatureErrorReported = true;
        }
    } else {
        waterTemperatureErrorReported = false;
    }
    
    reading.dissolvedO2 = dissolvedOxygenRead(reading.waterTempC);
    
    //
    if (!barometerRead(reading.airTempC, reading.pressureHpa, reading.airHumidity)) {
        if (!barometerErrorReported) {
            Serial.println("error: BME280 read failed");
            barometerErrorReported = true;
        }
    } else {
        barometerErrorReported = false;
    }

    // reading.pressureHpa = 1012.0f;
    // reading.airTempC = 29.0f;
    // reading.airHumidity = 75.0f;

    gpsRead(reading.latitude, reading.longitude, reading.gpsFix);
}