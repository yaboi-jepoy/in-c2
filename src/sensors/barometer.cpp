#include "barometer.h"
#include <Wire.h>
#include <Adafruit_BME280.h>

// Private to this file: nothing else touches the BME280 object
static Adafruit_BME280 bme;
static bool ready = false;

bool barometerInit() {
  // The address depends on how SDO is wired: GND = 0x76, VCC = 0x77
  ready = bme.begin(0x76, &Wire);
  if (!ready) {
    ready = bme.begin(0x77, &Wire);
  }
  return ready;
}

bool barometerRead(float& airTempC, float& pressureHpa, float& humidity) {
  if (!ready) return false;

  airTempC    = bme.readTemperature();           // °C
  pressureHpa = bme.readPressure() / 100.0f;     // Pa -> hPa
  humidity    = bme.readHumidity();              // %
  return true;
}