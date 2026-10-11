#pragma once

// Find the DS18B20 on its data pin. Returns false if no sensor is found.
bool temperatureInit();

// Read the water temperature in °C. Returns false if the read failed.
bool temperatureRead(float& waterTempC);