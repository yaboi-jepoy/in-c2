#pragma once
#include "types.h"

// screen initilization
// returns false if failed
bool oledInit();

// draw latest readings
void oledShowReading(const SensorReading& r);