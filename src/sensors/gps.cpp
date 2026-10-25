#include "gps.h"
#include <Arduino.h>
#include <HardwareSerial.h>
#include <TinyGPSPlus.h>
#include "config.h"

namespace {
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);
}

void gpsInit() {
    gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    Serial.printf(
        "GPS UART started: RX GPIO%d, TX GPIO%d, baud %d\n",
        GPS_RX_PIN, GPS_TX_PIN, GPS_BAUD
    );
}

void gpsRead(double& latitude, double& longitude, bool& fix) {
    while (gpsSerial.available() > 0) {
        gps.encode(static_cast<char>(gpsSerial.read()));
    }

    fix = gps.location.isValid() && gps.location.age() < 10000;
    if (fix) {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
    } else {
        latitude = 0.0;
        longitude = 0.0;
    }
}