#include "stepper.h"
#include <Arduino.h>
#include "config.h"

namespace {
const uint8_t HALF_STEP_SEQUENCE[8][4] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1}
};

int sequenceIndex = 0;

void writePhase() {
  digitalWrite(STEPPER_IN1, HALF_STEP_SEQUENCE[sequenceIndex][0]);
  digitalWrite(STEPPER_IN2, HALF_STEP_SEQUENCE[sequenceIndex][1]);
  digitalWrite(STEPPER_IN3, HALF_STEP_SEQUENCE[sequenceIndex][2]);
  digitalWrite(STEPPER_IN4, HALF_STEP_SEQUENCE[sequenceIndex][3]);
}
}

void stepperInit() {
  pinMode(STEPPER_IN1, OUTPUT);
  pinMode(STEPPER_IN2, OUTPUT);
  pinMode(STEPPER_IN3, OUTPUT);
  pinMode(STEPPER_IN4, OUTPUT);
  writePhase();
}

void stepperMove(long steps, int direction) {
  if (steps < 0) {
    steps = -steps;
    direction = -direction;
  }
  direction = direction < 0 ? -1 : 1;

  for (long step = 0; step < steps; ++step) {
    sequenceIndex += direction;
    if (sequenceIndex < 0) sequenceIndex = 7;
    if (sequenceIndex > 7) sequenceIndex = 0;
    writePhase();
    vTaskDelay(pdMS_TO_TICKS(STEPPER_STEP_DELAY_MS));
  }
}
