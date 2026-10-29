#include "gps.h"
#include <Arduino.h>
#include <HardwareSerial.h>
#include <TinyGPSPlus.h>
#include "config.h"

namespace {
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);
constexpr double HARDCODED_LATITUDE = 14.73004;
constexpr double HARDCODED_LONGITUDE = 121.02031;
}

void gpsInit() {
    gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    Serial.printf(
        "GPS UART started: RX GPIO%d, TX GPIO%d, baud %d\n",
        GPS_RX_PIN, GPS_TX_PIN, GPS_BAUD
    );
}

void gpsRead(double& latitude, double& longitude, bool& fix) {
    // Temporary fixed location until live GPS reading is re-enabled.
    latitude = HARDCODED_LATITUDE;
    longitude = HARDCODED_LONGITUDE;
    fix = true;
}