#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "oled.h"
#include "config.h"

static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool oledInit() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        return false;
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Starting...");
    display.display();
    return true;
}

void oledShowReading(const SensorReading& r) {
    display.clearDisplay();
    // ph_level
    display.setTextColor(1);
    display.setTextWrap(false);
    display.setCursor(33, 6);
    display.print("pH:");
    // ph_value
    display.setCursor(54, 6);
    display.print(r.ph);
    // temp_level
    display.setCursor(21, 15);
    display.print("temp:");
    // temp_value
    display.setCursor(54, 15);
    display.print(r.waterTempC);
    // temp_unit
    display.setCursor(86, 15);
    display.print("C");
    // do_level
    display.setCursor(33, 24);
    display.print("DO:");
    // do_value
    display.setCursor(54, 24);
    display.print(r.dissolvedO2);
    // do_unit
    display.setCursor(86, 24);
    display.print("mg/L");
        // bar_temp_level
    display.setCursor(21, 36);
    display.print("air:");
    // bar_temp_value
    display.setCursor(50, 36);
    display.print(r.airTempC);
    // bar_pA_level
    display.setCursor(27, 44);
    display.print("hPa:");
    // bar_hpa_value
    display.setCursor(50, 44);
    display.print(r.pressureHpa);
    // bar_hum_level
    display.setCursor(39, 52);
    display.print("H:");
    // bar_hum_value
    display.setCursor(51, 52);
    display.print(r.airHumidity);
    // gps_long
    display.setCursor(94, 38);
    display.print("long:");
    // gps_lat
    display.setCursor(94, 50);
    display.print("lat:");
    display.display();
}