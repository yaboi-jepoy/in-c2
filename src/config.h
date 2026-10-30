#pragma once

// magic numbers
#define RAW_QUEUE_LEN       3
#define CLOUD_QUEUE_LEN     1
#define CLOUD_UPLOAD_PERIOD_MS 10000
#define SENSOR_SAMPLE_INTERVAL_MS 10000

// Sensor-array deployment. A 28BYJ-48 normally takes 4096 half-steps per
// output-shaft revolution. Measure the actual spool diameter and adjust these
// values if the array does not travel exactly the requested distance.
#define SENSOR_DEPTH_MM             250.0f
#define SPOOL_DIAMETER_MM            25.5f
#define STEPPER_HALF_STEPS_PER_REV   4096L
#define STEPPER_DEPLOY_DIRECTION     1
#define STEPPER_STEP_DELAY_MS        1
#define SENSOR_AVERAGE_COUNT         5
#define SENSOR_SETTLE_MS             1000

// These pins are shared by the I2C sensors.
#define I2C_SDA 21
#define I2C_SCL 22

// ILI9341 SPI display pins. MISO is not connected because this firmware only
// writes to the display.
#define TFT_SCK 18
#define TFT_MOSI 23
#define TFT_CS  5
#define TFT_DC  27
#define TFT_RST 26

// ADS1115 is on the shared I2C bus. pH connects to A0 and DO connects to A1.
#define ADS1115_ADDRESS 0x48
#define PH_ADC_CHANNEL  0
#define DO_ADC_CHANNEL  1
#define DS18B20_PIN 4

// GPS UART pins. GPS module TX connects to ESP32 RX; GPS module RX connects
// to ESP32 TX.
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17
#define GPS_BAUD   9600

// 28BYJ-48 stepper motor driver (ULN2003) control pins.
#define STEPPER_IN1 13
#define STEPPER_IN2 14
#define STEPPER_IN3 25
#define STEPPER_IN4 33

// Network and Firebase values are injected from the local .env file by
// scripts/load_env.py during the PlatformIO build.
#ifndef WIFI_SSID
#define WIFI_SSID       "YOUR_WIFI_SSID"
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD   "YOUR_WIFI_PASSWORD"
#endif
#ifndef FIREBASE_API_KEY
#define FIREBASE_API_KEY "YOUR_FIREBASE_API_KEY"
#endif
#ifndef FIREBASE_PROJECT_ID
#define FIREBASE_PROJECT_ID "YOUR_FIREBASE_PROJECT_ID"
#endif
#ifndef DEVICE_ID
#define DEVICE_ID       "prototype-01"
#endif
#define BACKUP_LOG_PATH "/backup.log"
#define BACKUP_TEMP_PATH "/backup.tmp"
#define BACKUP_FLUSH_INTERVAL_MS 10000
#define FIREBASE_AUTH_RETRY_MS 30000