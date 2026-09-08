// CNC_Controller.ino
//
// 2-axis (X/Y) CNC positioning controller.
// Target: ESP32 DevKit, 2x NEMA17 via STEP/DIR drivers, 4" SPI TFT
// (TFT_eSPI), 4x4 matrix keypad.
//
// Required libraries (Arduino IDE Library Manager):
//   - AccelStepper
//   - TFT_eSPI        (configure User_Setup.h for your panel/pins first)
//   - Keypad
//
// Pin assignments: see config.h. TFT pin wiring is set in TFT_eSPI's
// own User_Setup.h, not in this project's config.h.

#include "config.h"
#include "settings.h"
#include "motion.h"
#include "display_ui.h"
#include "keypad_input.h"

enum AppState {
  ST_MAIN,
  ST_JOG,
  ST_ENTER_X,
  ST_ENTER_Y,
  ST_HOMING,
  ST_SETTINGS,
  ST_ERROR
};

static AppState state = ST_MAIN;
static CncSettings settings;

static String entryBuffer;
static float pendingX = 0;

static const float JOG_STEP_MM = 1.0f;

static void enterState(AppState newState) {
  state = newState;
  switch (state) {
    case ST_MAIN:
      displayShowMain();
      break;
    case ST_JOG:
      displayShowMain(); // reuse main screen, jog just changes key handling
      break;
    case ST_ENTER_X:
      entryBuffer = "";
      displayShowEntry("Target X (mm):", entryBuffer);
      break;
    case ST_ENTER_Y:
      entryBuffer = "";
      displayShowEntry("Target Y (mm):", entryBuffer);
      break;
    case ST_HOMING:
      displayShowHoming();
      break;
    case ST_SETTINGS:
      displayShowSettings(settings);
      break;
    case ST_ERROR:
      break; // caller sets message via displayShowError() before this
  }
}

// Applies a keypress to a numeric entry buffer (digits, '*' as minus,
// '#' as decimal point). Returns true if the buffer changed.
static bool applyEntryKey(char key) {
  if (key >= '0' && key <= '9') {
    entryBuffer += key;
    return true;
  }
  if (key == '#' && entryBuffer.indexOf('.') < 0) {
    if (entryBuffer.length() == 0) entryBuffer += '0';
    entryBuffer += '.';
    return true;
  }
  if (key == '*' && entryBuffer.length() == 0) {
    entryBuffer += '-';
    return true;
  }
  return false;
}

static void handleMainKey(char key) {
  switch (key) {
    case 'A': enterState(ST_JOG); break;
    case 'D': enterState(ST_ENTER_X); break;
    case 'C': enterState(ST_HOMING); break;
    case 'B': enterState(ST_SETTINGS); break;
    default: break;
  }
}

static void handleJogKey(char key) {
  switch (key) {
    case '8': motionJogMm(0, JOG_STEP_MM); break;
    case '2': motionJogMm(0, -JOG_STEP_MM); break;
    case '4': motionJogMm(-JOG_STEP_MM, 0); break;
    case '6': motionJogMm(JOG_STEP_MM, 0); break;
    case '5': motionStop(); break;
    case 'C': enterState(ST_MAIN); break;
    default: break;
  }
}

static void handleEntryKey(AppState entryState, char key) {
  if (key == 'D') {
    if (entryBuffer.length() == 0) return;
    float value = entryBuffer.toFloat();
    if (entryState == ST_ENTER_X) {
      pendingX = value;
      enterState(ST_ENTER_Y);
    } else { // ST_ENTER_Y
      motionMoveToMm(pendingX, value);
      enterState(ST_MAIN);
    }
    return;
  }
  if (key == 'C') {
    enterState(ST_MAIN);
    return;
  }
  if (key == 'B') {
    if (entryBuffer.length() > 0) {
      entryBuffer.remove(entryBuffer.length() - 1);
      displayShowEntry(entryState == ST_ENTER_X ? "Target X (mm):" : "Target Y (mm):", entryBuffer);
    }
    return;
  }
  if (applyEntryKey(key)) {
    displayShowEntry(entryState == ST_ENTER_X ? "Target X (mm):" : "Target Y (mm):", entryBuffer);
  }
}

static void handleSettingsKey(char key) {
  if (key == 'C') {
    enterState(ST_MAIN);
  }
  // Extend here to allow editing individual settings values with the keypad.
}

static void handleErrorKey(char key) {
  if (key == 'C') {
    enterState(ST_MAIN);
  }
}

static void doHoming() {
  bool ok = motionHome();
  if (ok) {
    enterState(ST_MAIN);
  } else {
    displayShowError("Homing timed out - check limit switches/wiring");
    state = ST_ERROR;
  }
}

void setup() {
  Serial.begin(115200);

  settingsLoad(settings);

  displayInit();
  keypadInit();
  motionInit(settings);

  enterState(ST_MAIN);
}

void loop() {
  char key = keypadPoll();

  if (key) {
    switch (state) {
      case ST_MAIN:      handleMainKey(key); break;
      case ST_JOG:        handleJogKey(key); break;
      case ST_ENTER_X:    handleEntryKey(ST_ENTER_X, key); break;
      case ST_ENTER_Y:    handleEntryKey(ST_ENTER_Y, key); break;
      case ST_SETTINGS:   handleSettingsKey(key); break;
      case ST_ERROR:      handleErrorKey(key); break;
      case ST_HOMING:     /* ignore keys while homing */ break;
    }
  }

  // Homing runs synchronously once requested (blocks the UI loop by
  // design - the homing screen is already drawn and keys are ignored).
  if (state == ST_HOMING) {
    doHoming();
  }

  if (state == ST_MAIN || state == ST_JOG) {
    displayUpdateStatus(motionGetXMm(), motionGetYMm(), motionIsRunning());
  }
}
