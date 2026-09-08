#pragma once
#include <Arduino.h>

// Thin wrapper around the Keypad library for a 4x4 matrix:
//   1 2 3 A     A = jog mode toggle
//   4 5 6 B     B = backspace
//   7 8 9 C     C = cancel / back
//   * 0 # D     * = minus sign, # = decimal point, D = enter/confirm

void keypadInit();

// Returns the key pressed since the last call, or 0 if none.
char keypadPoll();
