#include "dissolved_oxygen.h"
#include <Arduino.h>

static const int DO_PIN = 35;   // ADC1, input only

// ---- Calibration: fill these in from YOUR air calibration ----
// Voltage (mV) in air-saturated water at the calibration temperature.
// Leave at 0 until you've measured it. 0 means "not calibrated yet".
static const float V_SAT_MV = 1684.0f;

// Saturation DO (mg/L) in fresh water at 1 atm, every 5 °C from 0 to 40 °C
static const float T_TABLE[]  = {0, 5, 10, 15, 20, 25, 30, 35, 40};
static const float DO_TABLE[] = {14.62, 12.77, 11.29, 10.08, 9.09, 8.26, 7.56, 6.95, 6.41};

void dissolvedOxygenInit() {
  analogSetPinAttenuation(DO_PIN, ADC_11db);   // read up to ~3.3V
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
  uint32_t sum = 0;
  for (int i = 0; i < 16; i++) {
    sum += analogReadMilliVolts(DO_PIN);
  }
  float mV = sum / 16.0f;

  // for calibration: keep this until V_SAT_MV is set
  Serial.printf("DO mV: %.0f\n", mV);

  if (V_SAT_MV <= 0.0f) return 0.0f;   // not calibrated yet
  return (mV / V_SAT_MV) * doSaturationMgL(waterTempC);
}