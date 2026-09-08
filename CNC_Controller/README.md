# CNC_Controller

2-axis (X/Y) CNC positioning controller for ESP32, Arduino IDE.

## Hardware
- ESP32 DevKit (30/38-pin)
- 2x NEMA17 stepper + STEP/DIR driver (A4988 / DRV8825 / TMC2209 in legacy step/dir mode)
- 4" SPI TFT (ILI9341/ILI9488-class), driven by TFT_eSPI
- 4x4 matrix keypad
- 2x limit switches (X, Y) for homing

## Libraries (Arduino IDE Library Manager)
- `AccelStepper`
- `TFT_eSPI` — must be configured for your exact panel/pinout via its
  `User_Setup.h` (or `User_Setup_Select.h`) before building. This is done
  once inside the library folder, not in this sketch.
- `Keypad`

## Pin map
See `config.h` for stepper, limit switch, and keypad pins. TFT wiring is
set inside TFT_eSPI's own `User_Setup.h`.

Defaults assume active-LOW driver ENABLE and active-LOW limit switches
(NC switch to GND with `INPUT_PULLUP`, or wire per your switch type and
flip `LIMIT_ACTIVE_LOW` in `config.h`).

## Keypad layout
```
1 2 3 A     A = jog mode
4 5 6 B     B = settings / backspace (context-dependent)
7 8 9 C     C = home / cancel / back
* 0 # D     * = minus, # = decimal point, D = move-to / confirm
```

In jog mode: `8`/`2`/`4`/`6` step +Y/-Y/-X/+X by 1 mm, `5` stops, `C` exits.

## Files
- `CNC_Controller.ino` — state machine and main loop
- `config.h` — pins and default motion parameters
- `motion.h/.cpp` — AccelStepper wrapper, runs step generation on core 0
- `display_ui.h/.cpp` — TFT_eSPI screens
- `keypad_input.h/.cpp` — Keypad library wrapper
- `settings.h/.cpp` — persisted calibration (steps/mm, speed, accel) via NVS/Preferences

## Calibration
Steps/mm defaults (`DEFAULT_STEPS_PER_MM_X/Y` in `config.h`) assume a
1.8° motor (200 steps/rev), 16x microstepping, and a specific
leadscrew/pulley — **recalibrate for your actual mechanics** before
trusting positioning. Settings screen currently displays saved values;
extend `handleSettingsKey()` in the .ino to edit them from the keypad, or
edit `config.h` defaults and re-flash.

## Not yet implemented (natural next steps)
- Editing settings values from the keypad UI (currently view-only)
- G-code parsing / SD card job storage
- Z axis / spindle control
- Soft-limit warning screen before clamping a requested move
