// types.h
#pragma once
#include <stdint.h>

struct SensorReading {
  uint32_t timestampMs;
  float ph;
  float dissolvedO2;     // mg/L
  float waterTempC;
  float pressureHpa;     // BME280
  double latitude;       // GPS
  double longitude;
  bool gpsFix;
};

struct FormattedRecord {
  char json[256];        // fixed buffer avoids heap fragmentation
};