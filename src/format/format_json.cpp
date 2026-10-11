#include "format_json.h"
#include <stdio.h>

bool formatAsJson(const SensorReading& r, FormattedRecord& out) {
  int n = snprintf(out.json, sizeof(out.json),
    "{\"t\":%lu,\"ph\":%.2f,\"do\":%.2f,\"temp\":%.2f,"
    "\"patm\":%.2f,\"atemp\":%.2f,\"hum\":%.2f,"
    "\"lat\":%.6f,\"lon\":%.6f,\"fix\":%d}",
    (unsigned long)r.timestampMs, r.ph, r.dissolvedO2, r.waterTempC,
    r.pressureHpa, r.airTempC, r.airHumidity,
    r.latitude, r.longitude, r.gpsFix ? 1 : 0);

  // snprintf returns how many characters it wanted to write.
  // If that's more than the buffer, the JSON got cut off.
  return n > 0 && n < (int)sizeof(out.json);
}