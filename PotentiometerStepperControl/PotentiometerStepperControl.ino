// 2 potentiometers controlling 2 x 28BYJ-48 stepper motors (via ULN2003 driver boards) on ESP32 Dev Module.
// Requires the "AccelStepper" library (Library Manager > search "AccelStepper" by Mike McCauley).
//
// Each potentiometer acts as a bidirectional speed control: center = stopped,
// turning one way spins the stepper CW, the other way spins it CCW, faster
// the further from center.
//
// Wiring:
//   Pot 1 wiper -> POT1_PIN, Pot 2 wiper -> POT2_PIN (ADC1 pins), pot ends -> 3V3 / GND
//   ULN2003 board 1 IN1..IN4 -> STEPPER1_IN1_PIN..STEPPER1_IN4_PIN
//   ULN2003 board 2 IN1..IN4 -> STEPPER2_IN1_PIN..STEPPER2_IN4_PIN
//   ULN2003 boards powered from an external 5V supply, board GND tied to ESP32 GND.

#include <AccelStepper.h>

// Use ADC1 pins only (ADC2 conflicts with WiFi).
const int POT1_PIN = 34;
const int POT2_PIN = 35;

const int STEPPER1_IN1_PIN = 32;
const int STEPPER1_IN2_PIN = 33;
const int STEPPER1_IN3_PIN = 25;
const int STEPPER1_IN4_PIN = 26;

const int STEPPER2_IN1_PIN = 27;
const int STEPPER2_IN2_PIN = 14;
const int STEPPER2_IN3_PIN = 13;
const int STEPPER2_IN4_PIN = 4;

const int ADC_MAX = 4095;      // ESP32 ADC resolution (12-bit)
const int ADC_CENTER = ADC_MAX / 2;
const int DEADZONE = 200;      // ignore small drift around center

const float MAX_SPEED = 600.0;        // steps/sec at full deflection (28BYJ-48 safe limit in half-step mode)
const float MAX_ACCELERATION = 300.0; // steps/sec^2

// 28BYJ-48 coil order through ULN2003 is IN1-IN3-IN2-IN4.
AccelStepper stepper1(AccelStepper::HALF4WIRE, STEPPER1_IN1_PIN, STEPPER1_IN3_PIN, STEPPER1_IN2_PIN, STEPPER1_IN4_PIN);
AccelStepper stepper2(AccelStepper::HALF4WIRE, STEPPER2_IN1_PIN, STEPPER2_IN3_PIN, STEPPER2_IN2_PIN, STEPPER2_IN4_PIN);

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
