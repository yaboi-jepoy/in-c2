#include "ph.h"
#include <Arduino.h>

static const int PH_PIN = 34;   // ADC1, input only

// ---- Calibration: fill these in from YOUR two-point calibration ----
// mV reading in the pH 7.0 buffer, and in the pH 4.0 buffer.
// Leave at 0 until measured. 0 means "not calibrated yet".
static const float V_PH7_MV = 1570.0f;
static const float V_PH4_MV = 2112.0f;

void phInit() {
  analogSetPinAttenuation(PH_PIN, ADC_11db);   // read up to ~3.3V
}

float phRead() {
  uint32_t sum = 0;
  for (int i = 0; i < 16; i++) {
    sum += analogReadMilliVolts(PH_PIN);
  }
  float mV = sum / 16.0f;

  // for calibration: keep this until the constants are set
  Serial.printf("pH mV: %.0f\n", mV);

  if (V_PH7_MV == 0.0f || V_PH4_MV == 0.0f || V_PH7_MV == V_PH4_MV) {
    return 0.0f;   // not calibrated yet
  }

  // Straight line through the two calibration points:
  // pH 7.0 at V_PH7_MV, pH 4.0 at V_PH4_MV
  return 7.0f + (mV - V_PH7_MV) * 3.0f / (V_PH7_MV - V_PH4_MV);
}