#include "temperature.h"
#include "config.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// Private to this file: nothing else touches the 1-Wire bus
static OneWire oneWire(DS18B20_PIN);
static DallasTemperature ds(&oneWire);
static bool ready = false;

bool temperatureInit() {
  ds.begin();
  ready = (ds.getDeviceCount() > 0);
  return ready;
}

bool temperatureRead(float& waterTempC) {
  if (!ready) return false;

  // Asks the sensor for a new reading. Takes about 750 ms at the default
  // 12-bit resolution. That's fine for our 2-second sampling period.
  ds.requestTemperatures();

  float t = ds.getTempCByIndex(0);
  if (t == DEVICE_DISCONNECTED_C) {   // -127 means the sensor stopped responding
    return false;
  }

  waterTempC = t;
  return true;
}