#include "settings.h"
#include <Preferences.h>

static Preferences prefs;

void settingsReset(CncSettings &s) {
  s.stepsPerMmX = DEFAULT_STEPS_PER_MM_X;
  s.stepsPerMmY = DEFAULT_STEPS_PER_MM_Y;
  s.maxSpeedMmS = DEFAULT_MAX_SPEED_MM_S;
  s.accelMmS2 = DEFAULT_ACCEL_MM_S2;
  s.homingSpeedMmS = DEFAULT_HOMING_SPEED_MM_S;
}

void settingsLoad(CncSettings &s) {
  settingsReset(s);
  prefs.begin(NVS_NAMESPACE, true);
  s.stepsPerMmX   = prefs.getFloat("stepsX", s.stepsPerMmX);
  s.stepsPerMmY   = prefs.getFloat("stepsY", s.stepsPerMmY);
  s.maxSpeedMmS   = prefs.getFloat("maxSpd", s.maxSpeedMmS);
  s.accelMmS2     = prefs.getFloat("accel", s.accelMmS2);
  s.homingSpeedMmS = prefs.getFloat("homeSpd", s.homingSpeedMmS);
  prefs.end();
}

void settingsSave(const CncSettings &s) {
  prefs.begin(NVS_NAMESPACE, false);
  prefs.putFloat("stepsX", s.stepsPerMmX);
  prefs.putFloat("stepsY", s.stepsPerMmY);
  prefs.putFloat("maxSpd", s.maxSpeedMmS);
  prefs.putFloat("accel", s.accelMmS2);
  prefs.putFloat("homeSpd", s.homingSpeedMmS);
  prefs.end();
}
