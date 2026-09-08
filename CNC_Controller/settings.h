#pragma once
#include "config.h"

struct CncSettings {
  float stepsPerMmX;
  float stepsPerMmY;
  float maxSpeedMmS;
  float accelMmS2;
  float homingSpeedMmS;
};

void settingsLoad(CncSettings &s);
void settingsSave(const CncSettings &s);
void settingsReset(CncSettings &s);
