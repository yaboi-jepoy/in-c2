#pragma once

// Start the BME280. Returns false if it isn't found on the bus.
bool barometerInit();

// Read air temperature, pressure, and humidity in one call.
// Returns false if the sensor isn't ready or the read failed.
bool barometerRead(float& airTempC, float& pressureHpa, float& humidity);