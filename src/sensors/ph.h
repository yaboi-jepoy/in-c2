#pragma once

// Prepare the pH sensor. Call once from setup via sensorsInit().
void phInit();

// Return the current pH of the water (0-14).
// Returns a placeholder until the real DFRobot code is added.
float phRead();