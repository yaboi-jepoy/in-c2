#pragma once

// magic numbers
#define SAMPLE_PERIOD_MS    2000
#define RAW_QUEUE_LEN       3
#define CLOUD_QUEUE_LEN     20

// these pins will be shared by SSD1306, BME280, AND ADS1115 LATER
#define I2C_SDA 21
#define I2C_SCL 22

// SSD1306 display variables
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64
#define OLED_ADDR       0x3C    // NOT 0x3D
#define OLED_RESET      -1      // no reset pin