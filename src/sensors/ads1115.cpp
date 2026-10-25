#include "ads1115.h"
#include <Adafruit_ADS1X15.h>
#include <Arduino.h>
#include "config.h"

namespace {
Adafruit_ADS1115 ads;
bool initialized = false;
}

bool ads1115Init() {
  if (!ads.begin(ADS1115_ADDRESS)) {
    Serial.println("ADS1115 not found. Check I2C wiring and address.");
    return false;
  }

  // GAIN_ONE provides a +/-4.096 V range at 0.125 mV per count.
  ads.setGain(GAIN_ONE);
  initialized = true;
  Serial.println("ADS1115 initialized");
  return true;
}

bool ads1115ReadMilliVolts(uint8_t channel, float& millivolts) {
  if (!initialized || channel > 3) {
    return false;
  }

  int32_t sum = 0;
  for (uint8_t sample = 0; sample < 16; ++sample) {
    sum += ads.readADC_SingleEnded(channel);
  }
  millivolts = (sum / 16.0f) * 0.125f;
  return true;
}
