#include "format_json.h"
#include <stdio.h>
#include <time.h>
#include "config.h"

bool formatAsJson(const SensorReading& r, FormattedRecord& out) {
  struct tm utc;
  time_t now = time(nullptr);
  bool timeAvailable = now > 1000000000 && gmtime_r(&now, &utc) != nullptr;

  char measuredAt[25] = "1970-01-01T00:00:00Z";
  if (timeAvailable) {
    strftime(measuredAt, sizeof(measuredAt), "%Y-%m-%dT%H:%M:%SZ", &utc);
  }

  char latitude[20] = "null";
  char longitude[20] = "null";
  if (r.gpsFix) {
    snprintf(latitude, sizeof(latitude), "%.6f", r.latitude);
    snprintf(longitude, sizeof(longitude), "%.6f", r.longitude);
  }
  int n = snprintf(out.json, sizeof(out.json),
    "{\"deviceId\":\"%s\",\"source\":\"esp32\","
    "\"waterTemperature\":%.2f,\"ph\":%.2f,\"dissolvedOxygen\":%.2f,"
    "\"airTemperature\":%.2f,\"humidity\":%.2f,\"pressure\":%.2f,"
    "\"latitude\":%s,\"longitude\":%s,\"measuredAt\":\"%s\"}",
    DEVICE_ID, r.waterTempC, r.ph, r.dissolvedO2, r.airTempC,
    r.airHumidity, r.pressureHpa,
    latitude, longitude,
    measuredAt);

  // snprintf returns how many characters it wanted to write.
  // If that's more than the buffer, the JSON got cut off.
  return n > 0 && n < (int)sizeof(out.json);
}