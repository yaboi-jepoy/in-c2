#include "dissolved_oxygen.h"
#include <Arduino.h>
#include "ads1115.h"
#include "config.h"

// ---- Temporary provisional calibration ----
// Approximate voltage reference used while air-saturated calibration water is
// unavailable. Replace it after an air-saturation calibration.
static const float V_SAT_MV = 1684.0f;

// Saturation DO (mg/L) in fresh water at 1 atm, every 5 °C from 0 to 40 °C
static const float T_TABLE[]  = {0, 5, 10, 15, 20, 25, 30, 35, 40};
static const float DO_TABLE[] = {14.62, 12.77, 11.29, 10.08, 9.09, 8.26, 7.56, 6.95, 6.41};

void dissolvedOxygenInit() {
  float mV = 0.0f;
  if (ads1115ReadMilliVolts(DO_ADC_CHANNEL, mV)) {
    Serial.printf("setup: DO %.0f mV\n", mV);
  }
}

// Pure helper: interpolate the saturation value for a temperature
static float doSaturationMgL(float tempC) {
  const int n = sizeof(T_TABLE) / sizeof(T_TABLE[0]);
  if (tempC <= T_TABLE[0])     return DO_TABLE[0];
  if (tempC >= T_TABLE[n - 1]) return DO_TABLE[n - 1];

  for (int i = 0; i < n - 1; i++) {
    if (tempC <= T_TABLE[i + 1]) {
      float f = (tempC - T_TABLE[i]) / (T_TABLE[i + 1] - T_TABLE[i]);
      return DO_TABLE[i] + f * (DO_TABLE[i + 1] - DO_TABLE[i]);
    }
  }
  return DO_TABLE[n - 1];
}

float dissolvedOxygenRead(float waterTempC) {
  static bool errorReported = false;
  float mV = 0.0f;
  if (!ads1115ReadMilliVolts(DO_ADC_CHANNEL, mV)) {
    if (!errorReported) {
      Serial.println("error: DO ADS1115 read failed");
      errorReported = true;
    }
    return 0.0f;
  }
  errorReported = false;

  if (V_SAT_MV <= 0.0f) return 0.0f;   // not calibrated yet
  return (mV / V_SAT_MV) * doSaturationMgL(waterTempC);
}