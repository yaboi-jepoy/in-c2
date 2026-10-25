#pragma once

// magic numbers
#define SAMPLE_PERIOD_MS    2000
#define RAW_QUEUE_LEN       3
#define CLOUD_QUEUE_LEN     20

// These pins are shared by the I2C sensors.
#define I2C_SDA 21
#define I2C_SCL 22

// ILI9341 SPI display pins.
// VSPI hardware pins: SCK=18, MISO=19, MOSI=23.
#define TFT_SCK 18
#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_CS  27
#define TFT_DC  26
#define TFT_RST 25

// sensor pins
#define PH_PIN    34
#define DO_PIN    35
#define DS18B20_PIN 4