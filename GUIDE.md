# in-c2 Firmware Guide

A guide for working on the ESP32 water quality firmware on your own. It covers how the code is organized, why, and how to add to it without breaking things.

---

## 1. The big picture

The firmware does five jobs at the same time:

| Job | Where it lives | What it does |
|---|---|---|
| Sensing | `tasks/sensing_task.cpp` + `sensors/` | Reads every sensor on a fixed timer |
| Formatting | `tasks/format_task.cpp` | Turns raw numbers into timestamped records |
| Cloud | `tasks/cloud_task.cpp` + `comms/` | Sends records over WiFi |
| Display | `tasks/display_task.cpp` + `display/` | Draws the latest values on the ILI9341 TFT |
| Startup | `main.cpp` | Starts the bus, the hardware, the queues, and the tasks |

The jobs run in parallel under **FreeRTOS**, the real-time scheduler built into the ESP32. Each job is a *task*: a function with an infinite loop that does its work and then waits.

```
SensorTask ──(rawQueue)──► FormatTask ──(cloudQueue)──► CloudTask
                               │
                               └──(latestQueue)──► DisplayTask
```

The arrows are the only way data moves between jobs. No task reads another task's variables directly.

---

## 2. Core concepts

### Tasks

A task is a function that never returns:

```cpp
void sensingTask(void* arg) {
  for (;;) {
    // do work
    vTaskDelay(pdMS_TO_TICKS(2000));  // wait 2 seconds
  }
}
```

Each task has:
- **A priority.** Higher number = runs first when several are ready. Sensing is 3, format is 2, cloud and display are 1.
- **A stack.** Its own memory for local variables. Too small and it crashes; too large wastes RAM. Start with 4096 bytes and raise it if a task misbehaves.
- **A core.** The ESP32 has two cores. WiFi runs on core 0 by default, so cloud goes there. Everything else runs on core 1.

### Queues

A queue is a fixed-size buffer that passes data between tasks by **copying** it in and out:

```cpp
xQueueSend(rawQueue, &reading, 0);            // put in, don't wait if full
xQueueReceive(rawQueue, &reading, portMAX_DELAY);  // take out, wait forever if empty
```

Copying means the receiving task owns its own data. The sender can overwrite its variable without affecting what was already sent.

The `latestQueue` is length 1 and uses `xQueueOverwrite` / `xQueuePeek`. It always holds the newest snapshot, which is what the display needs.

### Delays

Always use FreeRTOS delays inside tasks:

| Use | Don't use |
|---|---|
| `vTaskDelay(pdMS_TO_TICKS(500))` | `delay(500)` |
| `vTaskDelayUntil(&lastWake, period)` for steady timing | `vTaskDelay` alone when you need exact intervals |

`delay()` is for `setup()` only. Inside a task, it blocks the wrong things.

---

## 3. C and C++ files: `.h` and `.cpp`

Each `.cpp` is compiled alone into an object file. The linker then connects them. A file can only call functions it knows about, so:

- **`.h` (header)** declares the interface: names, argument types, return types.
- **`.cpp` (source)** defines the behavior.

```cpp
// sensors/ph.h
#pragma once
void phInit();
float phRead();
```

```cpp
// sensors/ph.cpp
#include "ph.h"
void phInit() { /* ... */ }
float phRead() { return 7.0f; }
```

Rules to follow:

1. **Every `.h` starts with `#pragma once`.** It stops the same header being included twice.
2. **Every `.cpp` includes its own header first.** The compiler then checks your definitions against the declarations.
3. **Only put declarations in headers.** Function bodies go in `.cpp`. The exceptions are tiny `inline` functions and constants.
4. **Use `static` for things private to one file.** A helper used only in `ph.cpp` should be `static` there and not appear in `ph.h`.
5. **Include only what you use.** `sensor_manager.cpp` includes the driver headers. Tasks include `sensor_manager.h`, never the drivers directly.

### Global variables: `extern`

A variable declared in a header with `extern` is promised to exist elsewhere. It must be **defined exactly once** in a `.cpp`:

```cpp
// queues.h
extern QueueHandle_t rawQueue;   // declaration: "this exists somewhere"
```

```cpp
// main.cpp
QueueHandle_t rawQueue;          // definition: this is where it lives
```

If you forget the definition, you get `undefined reference to 'rawQueue'` at link time. If you define it twice, you get `multiple definition`. If you put the definition inside a function, the variable only exists inside that function, so the linker can't find it.

---

## 4. Project layout

```
src/
  main.cpp                  Startup only: bus, hardware, queues, tasks
  config.h                  Pins, periods, stack sizes, queue lengths
  types.h                   Shared data structs (SensorReading, FormattedRecord)
  queues.h                  Queue handles, declared extern (recommended)

  sensors/                  One pair of files per sensor
    sensor_manager.h/.cpp   The single entry point: sensorsInit(), sensorsRead()
    ph.h/.cpp
    dissolved_oxygen.h/.cpp
    temperature.h/.cpp
    barometer.h/.cpp
    gps.h/.cpp

  tasks/                    Timing and queue logic only
    sensing_task.cpp
    format_task.cpp
    cloud_task.cpp
    display_task.cpp

  comms/                    WiFi and cloud details
    wifi_cloud.h/.cpp

  display/                  ILI9341 TFT details
    tft.h/.cpp
```

### The dependency rule

Dependencies only point one way:

```
types.h
  ▲
sensors/*.h               (driver interfaces)
  ▲
sensor_manager.h          (one entry point)
  ▲
tasks/, comms/, display/  (use managers, never drivers)
  ▲
main.cpp                  (wires everything together)
```

Lower layers never include higher ones. If `ph.cpp` needs something from a task, that thing belongs in a lower layer.

**Why this matters:** Swapping a sensor, screen, or network changes one folder. The tasks stay the same.

---

## 5. How to add something

### Adding a sensor

1. Create `sensors/newsensor.h` with `#pragma once` and prototypes for `newsensorInit()` and `newsensorRead()`.
2. Create `sensors/newsensor.cpp` that includes its own header first. Start with a stub that returns a fake value.
3. Add a field for its data to `SensorReading` in `types.h`.
4. Call its init from `sensorsInit()` and its read from `sensorsRead()` in `sensor_manager.cpp`.
5. Add the field to `formatAsJson()` if it should appear in the output.

Tasks don't change.

### Replacing a stub with real code

1. Keep the function names and return types exactly the same.
2. Move constants (pins, calibration) into `ph.cpp` as `static const` or `#define`, not into `ph.h`.
3. Build. If it compiles, the rest of the project already works with it.

### Adding a task

1. Write the function in `tasks/`, with an infinite loop and a delay.
2. Declare it in `main.cpp` above `setup()`.
3. Create any queue it needs in `setup()`.
4. Start it with `xTaskCreatePinnedToCore()` and pick a priority and core.

---

## 6. Debugging

### Build errors

| Error | Usual cause |
|---|---|
| `undefined reference to 'X'` (link) | Function or variable declared but never defined, or defined inside another function, or marked `static` |
| `multiple definition of 'X'` (link) | Defined in a header without `inline`, or defined in two `.cpp` files |
| `'X' was not declared in this scope` (compile) | Missing `#include`, or typo |
| `'X' does not name a type` (compile) | Header used before the type it needs is included |

Read the **first** error. Later errors are often caused by it.

### Runtime problems

Add a print after each queue call to see where data stops:

```cpp
Serial.println("sensing: sent reading");
```

Check the output of `xTaskCreatePinnedToCore()` and `xQueueCreate()`. `pdPASS` means the task started. `NULL` from `xQueueCreate()` means it failed, usually from running out of memory.

For stack trouble, check `uxTaskGetStackHighWaterMark(NULL)` inside a task. It returns the smallest free stack space the task has ever had. A number near zero means raise the stack size.

### Hardware checks

- **TFT blank:** Check 3.3 V power, common ground, CS/DC/RST, and the VSPI pins in `config.h`.
- **TFT orientation:** `setRotation(1)` selects landscape mode (320 x 240). Change it to `0` for portrait mode.
- **I2C sharing:** The BME280 uses SDA GPIO21 and SCL GPIO22. The TFT is SPI and does not use the I2C bus.
- **ADS1115 inputs:** Connect the pH sensor output to A0 and the DO sensor output to A1. Connect ADS1115 VDD to 3.3 V, GND to ground, and leave its address at `0x48` unless `ADS1115_ADDRESS` is changed in `config.h`.
- **Sensor reads 0 or -127:** Check wiring first. The DS18B20 returns -127 when it isn't detected.

---

## 7. Working habits

- **Build after every small change.** Finding one error is easy. Finding five is not.
- **Stub first, then real.** Make the whole pipeline run with fake values before adding hardware.
- **Keep `main.cpp` short.** If it grows, logic belongs in a module.
- **Don't use globals across files** except the queue handles, which are declared in one header.
- **Comment the why, not the what.** `// ADC is nonlinear below 0.3V, so we clamp` helps more than `// read ADC`.

---

## 8. Glossary

| Term | Meaning |
|---|---|
| FreeRTOS | Real-time scheduler on the ESP32 that runs tasks |
| Task | A function running in its own loop under FreeRTOS |
| Queue | Fixed-size buffer that passes copies of data between tasks |
| Mutex | Lock so only one task uses a shared resource at a time |
| Stack | Per-task memory for local variables |
| Priority | Which task runs first when several are ready |
| Core | One of the ESP32's two processors |
| Header (`.h`) | File declaring what exists |
| Source (`.cpp`) | File defining how it works |
| `extern` | Promise that a variable is defined in another file |
| `static` | Limits a name to the file it's in |
| `#pragma once` | Prevents a header being included twice |
| I2C | Two-wire bus used by the BME280 and other I2C sensors |
| SPI | Serial bus used by the ILI9341 TFT |
| ADC | Converts an analog voltage into a number |

## Wi-Fi, Firebase, and Offline Backup

Set the Wi-Fi and Firebase values in the local `.env` file before uploading.
The Firebase client uses anonymous authentication, so enable **Anonymous**
under Firebase Authentication and configure the Realtime Database URL in the
Firebase console. `CONFIGURATION_NOT_FOUND` during sign-up usually means
Anonymous Authentication is disabled or the API key belongs to a different
Firebase project than the database URL.

Records are pushed to the `/readings` path. When Wi-Fi is unavailable, the
latest JSON record is saved to `/backup.log` in the ESP32's LittleFS flash
filesystem. Firebase errors while Wi-Fi is still connected are reported but
are not written to the offline backup. The cloud task retries the saved record
after connectivity is restored.

Sensor readings are sampled every 2 seconds. The cloud task uploads the latest
available reading every 10 seconds; readings collected between uploads replace
the older pending reading instead of building an upload queue.
