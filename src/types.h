// types.h
#pragma once
#include <stdint.h>

struct SensorReading {
  // from core sensors
  float ph;
  float dissolvedO2;     // mg/L
  float waterTempC;
  
  // from supplementary sensors
  float pressureHpa;     // BME280
  float airTempC;
  float airHumidity;
  double latitude;       // GPS
  double longitude;
  uint32_t timestampMs;
  bool gpsFix;
};

struct FormattedRecord {
  char json[256];        // fixed buffer avoids heap fragmentation
};