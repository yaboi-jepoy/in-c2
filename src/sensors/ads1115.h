#pragma once

#include <stdint.h>

bool ads1115Init();
bool ads1115ReadMilliVolts(uint8_t channel, float& millivolts);
