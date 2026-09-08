// 2 potentiometers controlling 2 stepper motors (via STEP/DIR drivers, e.g. A4988/DRV8825) on ESP32 Dev Module.
// Requires the "AccelStepper" library (Library Manager > search "AccelStepper" by Mike McCauley).
//
// Each potentiometer acts as a bidirectional speed control: center = stopped,
// turning one way spins the stepper CW, the other way spins it CCW, faster
// the further from center.
//
// Wiring:
//   Pot 1 wiper -> POT1_PIN, Pot 2 wiper -> POT2_PIN (ADC1 pins), pot ends -> 3V3 / GND
//   Driver 1 STEP -> STEPPER1_STEP_PIN, DIR -> STEPPER1_DIR_PIN
//   Driver 2 STEP -> STEPPER2_STEP_PIN, DIR -> STEPPER2_DIR_PIN
//   Drivers powered from an external motor supply, driver GND tied to ESP32 GND.

#include <AccelStepper.h>

// Use ADC1 pins only (ADC2 conflicts with WiFi).
const int POT1_PIN = 34;
const int POT2_PIN = 35;

const int STEPPER1_STEP_PIN = 25;
const int STEPPER1_DIR_PIN = 26;
const int STEPPER2_STEP_PIN = 27;
const int STEPPER2_DIR_PIN = 14;

const int ADC_MAX = 4095;      // ESP32 ADC resolution (12-bit)
const int ADC_CENTER = ADC_MAX / 2;
const int DEADZONE = 200;      // ignore small drift around center

const float MAX_SPEED = 1000.0;      // steps/sec at full deflection
const float MAX_ACCELERATION = 500.0; // steps/sec^2

AccelStepper stepper1(AccelStepper::DRIVER, STEPPER1_STEP_PIN, STEPPER1_DIR_PIN);
AccelStepper stepper2(AccelStepper::DRIVER, STEPPER2_STEP_PIN, STEPPER2_DIR_PIN);

float readAxisSpeed(int pin) {
  int raw = analogRead(pin);
  int offset = raw - ADC_CENTER;

  if (abs(offset) < DEADZONE) {
    return 0.0;
  }

  return (float)offset / ADC_CENTER * MAX_SPEED;
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  stepper1.setMaxSpeed(MAX_SPEED);
  stepper1.setAcceleration(MAX_ACCELERATION);
  stepper2.setMaxSpeed(MAX_SPEED);
  stepper2.setAcceleration(MAX_ACCELERATION);
}

void loop() {
  float speed1 = readAxisSpeed(POT1_PIN);
  float speed2 = readAxisSpeed(POT2_PIN);

  stepper1.setSpeed(speed1);
  stepper2.setSpeed(speed2);

  stepper1.runSpeed();
  stepper2.runSpeed();
}
