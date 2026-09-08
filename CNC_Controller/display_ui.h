#pragma once
#include <Arduino.h>
#include "settings.h"

enum UiScreen {
  SCREEN_MAIN,
  SCREEN_ENTRY,
  SCREEN_HOMING,
  SCREEN_SETTINGS,
  SCREEN_ERROR
};

void displayInit();

// Full-redraw screens (call on screen change).
void displayShowMain();
void displayShowEntry(const char *label, const String &buffer);
void displayShowHoming();
void displayShowSettings(const CncSettings &s);
void displayShowError(const String &message);

// Lightweight partial updates for the main status screen, called every
// loop iteration without a full redraw to avoid flicker.
void displayUpdateStatus(float xMm, float yMm, bool running);
