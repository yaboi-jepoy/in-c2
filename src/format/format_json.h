#pragma once
#include "types.h"

// Turns one reading into a JSON string in out.json.
// Returns false if the text didn't fit in the buffer.
bool formatAsJson(const SensorReading& r, FormattedRecord& out);