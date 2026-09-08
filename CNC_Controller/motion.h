#pragma once
#include <Arduino.h>
#include "settings.h"

// Motion control wraps two AccelStepper instances and runs their
// step-generation loop in a dedicated FreeRTOS task on core 0, so the
// UI/keypad loop on core 1 never causes step timing hiccups.

void motionInit(const CncSettings &s);
void motionApplySettings(const CncSettings &s);

// Absolute move in mm. Non-blocking: returns immediately, motion runs
// in the background task. Use motionIsRunning() to poll completion.
void motionMoveToMm(float xMm, float yMm);

// Relative jog in mm (positive/negative).
void motionJogMm(float dxMm, float dyMm);

void motionStop();               // decelerate to stop
void motionEmergencyStop();      // immediate stop, disables drivers

bool motionIsRunning();
float motionGetXMm();
float motionGetYMm();

// Blocking homing routine (call from UI task while showing a "Homing" screen).
// Returns true on success, false if a limit switch never triggered (timeout).
bool motionHome();

void motionEnableDrivers(bool enable);
