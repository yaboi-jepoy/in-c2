#include "ph.h"
#include <Arduino.h>
#include "ads1115.h"
#include "config.h"

// ---- Temporary provisional calibration ----
// These values allow approximate readings while proper pH buffer solutions
// are unavailable. Replace them after a two-point calibration.
// static const float V_PH7_MV = 1570.0f;
// static const float V_PH4_MV = 2112.0f;
static const float V_PH7_MV = 1049.0f;
static const float V_PH4_MV = 1988.0f;

void phInit() {
  float mV = 0.0f;
  if (ads1115ReadMilliVolts(PH_ADC_CHANNEL, mV)) {
    Serial.printf("setup: pH %.0f mV\n", mV);
  }
}

float phRead() {
  static bool errorReported = false;
  float mV = 0.0f;
  if (!ads1115ReadMilliVolts(PH_ADC_CHANNEL, mV)) {
    if (!errorReported) {
      Serial.println("error: pH ADS1115 read failed");
      errorReported = true;
    }
    return 0.0f;
  }
  errorReported = false;

  if (V_PH7_MV == 0.0f || V_PH4_MV == 0.0f || V_PH7_MV == V_PH4_MV) {
    return 0.0f;   // not calibrated yet
  }

  // Straight line through the two calibration points:
  // pH 7.0 at V_PH7_MV, pH 4.0 at V_PH4_MV
  return 7.0f + (mV - V_PH7_MV) * 3.0f / (V_PH7_MV - V_PH4_MV);
}