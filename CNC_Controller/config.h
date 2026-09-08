#pragma once

// ===================== STEPPER PINS =====================
#define X_STEP_PIN   26
#define X_DIR_PIN    27
#define Y_STEP_PIN   14
#define Y_DIR_PIN    12
#define STEPPERS_EN_PIN 13   // shared ENABLE, active LOW on most drivers

// ===================== LIMIT SWITCHES =====================
#define X_LIMIT_PIN  34      // input only, use external pull-up/down
#define Y_LIMIT_PIN  35      // input only, use external pull-up/down
#define LIMIT_ACTIVE_LOW true

// ===================== TFT (configured via TFT_eSPI User_Setup) =====
// Wiring is set in the TFT_eSPI library's User_Setup.h, not here.
// Typical SPI TFT pins used in this project (document only):
//   TFT_MOSI 23, TFT_SCLK 18, TFT_CS 5, TFT_DC 2, TFT_RST 4, TFT_MISO 19

// ===================== KEYPAD (4x4 matrix) =====================
#define KEYPAD_ROW_PINS {32, 33, 25, 16}
#define KEYPAD_COL_PINS {17, 21, 22, 15}

// ===================== MOTION PARAMETERS =====================
#define DEFAULT_STEPS_PER_MM_X   80.0f
#define DEFAULT_STEPS_PER_MM_Y   80.0f
#define DEFAULT_MAX_SPEED_MM_S   40.0f     // mm/s
#define DEFAULT_ACCEL_MM_S2      200.0f    // mm/s^2
#define DEFAULT_HOMING_SPEED_MM_S 8.0f

#define SOFT_LIMIT_MIN_X_MM  0.0f
#define SOFT_LIMIT_MAX_X_MM  300.0f
#define SOFT_LIMIT_MIN_Y_MM  0.0f
#define SOFT_LIMIT_MAX_Y_MM  300.0f

// ===================== NVS NAMESPACE =====================
#define NVS_NAMESPACE "cnc_cfg"
