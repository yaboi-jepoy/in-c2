#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <Arduino.h>
#include "tft.h"
#include "config.h"

#ifndef TFT_MADCTL
#define TFT_MADCTL 0x40     // try 0x40, 0x00, 0x80, 0xC0 until the image is upright and not mirrored
#endif

#ifndef TFT_CORNER_TEST
#define TFT_CORNER_TEST 1   // set to 0 once the full screen is confirmed working
#endif

static Adafruit_ILI9341 display(TFT_CS, TFT_DC, TFT_RST);

// Layout (landscape, 320 x 240)
static const int ROW_H      = 28;   // height of one data row
static const int HEADER_H   = 30;
static const int LABEL_X    = 8;
static const int VALUE_X    = 130;
static const int GPS_X      = 56;   // GPS needs more width than the other rows
static const uint16_t COL_BG    = ILI9341_BLACK;
static const uint16_t COL_LABEL = ILI9341_CYAN;
static const uint16_t COL_VALUE = ILI9341_WHITE;

static const char* LABELS[] = {
    "pH", "Water", "DO", "Air", "Pressure", "Humidity"
};
static const int NUM_ROWS = sizeof(LABELS) / sizeof(LABELS[0]);

static int rowY(int i) { return HEADER_H + 4 + i * ROW_H; }

// Draw a value padded with spaces; the background colour erases the old text,
// so there is no fillRect() and no flicker.
static void drawValue(int y, const char* text, uint16_t color = COL_VALUE, int x = VALUE_X) {
    char buf[28];
    int chars = (display.width() - x) / 12;          // 12 px per char at text size 2
    snprintf(buf, sizeof(buf), "%-*s", chars, text);
    display.setTextSize(2);
    display.setTextColor(color, COL_BG);
    display.setCursor(x, y);
    display.print(buf);
}

static void drawStaticLayout() {
    display.fillScreen(COL_BG);

    // header bar
    display.fillRect(0, 0, display.width(), HEADER_H, ILI9341_NAVY);
    display.setTextSize(2);
    display.setTextColor(ILI9341_WHITE, ILI9341_NAVY);
    display.setCursor(LABEL_X, 7);
    display.print("Water Quality Node");

    // row labels
    display.setTextColor(COL_LABEL, COL_BG);
    for (int i = 0; i < NUM_ROWS; i++) {
        display.setCursor(LABEL_X, rowY(i));
        display.print(LABELS[i]);
    }

    // GPS label
    display.setCursor(LABEL_X, rowY(NUM_ROWS));
    display.print("GPS");
}

bool tftInit() {
    Serial.println("TFT: configuring VSPI");
    // -1 for SS: chip select is driven manually by the Adafruit library.
    // Passing TFT_CS here makes the ESP32 core also try to control it.
    SPI.begin(TFT_SCK, -1, TFT_MOSI, -1);

    // The library resets the panel itself through TFT_RST, but a slow
    // power-up on jumper wires sometimes needs this extra pulse.
    pinMode(TFT_RST, OUTPUT);
    digitalWrite(TFT_RST, HIGH);
    delay(5);
    digitalWrite(TFT_RST, LOW);
    delay(20);
    digitalWrite(TFT_RST, HIGH);
    delay(150);

    Serial.println("TFT: starting ILI9341");
    display.begin(10000000);      // conservative clock for jumper wires; try 20-40 MHz later
    display.setRotation(1);       // 1 = landscape 320 x 240 (0 and 2 are portrait 240 x 320)
    display.setTextWrap(false);

    // Writes past ~240 px were being rejected in both portrait and landscape, which means
    // this panel is natively 320 x 240 (ILI9342-style) and treats the library's MV
    // (row/column exchange) bit as "portrait". So keep the library's 320 x 240 size from
    // setRotation(1) but send a MADCTL WITHOUT the MV bit, and without the BGR bit
    // (the photo showed red/blue swapped, so this panel is RGB order).
    //   0x00 / 0x40 / 0x80 / 0xC0 = the four mirror/flip combinations, +0x08 for BGR.
    // If the picture is mirrored or upside down, change TFT_MADCTL to another of the four.
    uint8_t madctl = TFT_MADCTL;
    display.sendCommand(ILI9341_MADCTL, &madctl, 1);

    Serial.printf("TFT: size %d x %d\n", display.width(), display.height());

#if TFT_CORNER_TEST
    // Corner test: shows exactly which parts of the 320 x 240 area really get written.
    // Expected: red top-left, green top-right, blue bottom-left, white bottom-right,
    // plus a 1 px white border around the whole screen. Held for 6 s.
    display.fillScreen(ILI9341_BLACK);
    display.drawRect(0, 0, display.width(), display.height(), ILI9341_WHITE);
    display.fillRect(2, 2, 30, 30, ILI9341_RED);
    display.fillRect(display.width() - 32, 2, 30, 30, ILI9341_GREEN);
    display.fillRect(2, display.height() - 32, 30, 30, ILI9341_BLUE);
    display.fillRect(display.width() - 32, display.height() - 32, 30, 30, ILI9341_WHITE);
    display.setTextSize(2);
    display.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
    display.setCursor(40, 10);
    display.print("TOP-LEFT is here");
    display.setCursor(40, display.height() - 26);
    display.print("BOTTOM-LEFT");
    delay(6000);
#endif

    drawStaticLayout();
    return true;
}

void tftShowReading(const SensorReading& r) {
    char buf[28];

    snprintf(buf, sizeof(buf), "%.2f", r.ph);
    drawValue(rowY(0), buf);

    snprintf(buf, sizeof(buf), "%.2f C", r.waterTempC);
    drawValue(rowY(1), buf);

    snprintf(buf, sizeof(buf), "%.2f mg/L", r.dissolvedO2);
    drawValue(rowY(2), buf);

    snprintf(buf, sizeof(buf), "%.2f C", r.airTempC);
    drawValue(rowY(3), buf);

    snprintf(buf, sizeof(buf), "%.1f hPa", r.pressureHpa);
    drawValue(rowY(4), buf);

    snprintf(buf, sizeof(buf), "%.1f %%", r.airHumidity);
    drawValue(rowY(5), buf);

    // GPS
    if (r.gpsFix) {
        snprintf(buf, sizeof(buf), "%.5f, %.5f", r.latitude, r.longitude);
        drawValue(rowY(NUM_ROWS), buf, ILI9341_GREEN, GPS_X);
    } else {
        drawValue(rowY(NUM_ROWS), "no fix", ILI9341_YELLOW, GPS_X);
    }
}