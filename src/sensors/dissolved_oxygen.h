#pragma once

// Prepare the DO sensor. Call once from sensorsInit().
void dissolvedOxygenInit();

// Convert the probe voltage to dissolved oxygen in mg/L.
// Temperature is needed because oxygen solubility depends on it,
// so sensorsRead() passes in the DS18B20 value.
float dissolvedOxygenRead(float waterTempC);