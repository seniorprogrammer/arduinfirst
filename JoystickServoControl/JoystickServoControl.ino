// Joystick (X/Y, 4-direction) controlling 2 servos on ESP32 Dev Module.
// Requires the "ESP32Servo" library (Library Manager > search "ESP32Servo" by Kevin Harrington).
//
// Wiring:
//   Joystick VCC -> 3V3, GND -> GND
//   Joystick VRx -> JOY_X_PIN (ADC1 pin)
//   Joystick VRy -> JOY_Y_PIN (ADC1 pin)
//   Servo 1 signal -> SERVO_X_PIN, Servo 2 signal -> SERVO_Y_PIN
//   Servos powered from an external 5V supply, with servo GND tied to ESP32 GND.

#include <ESP32Servo.h>

// Use ADC1 pins only (ADC2 conflicts with WiFi).
const int JOY_X_PIN = 34;
const int JOY_Y_PIN = 35;

const int SERVO_X_PIN = 18;
const int SERVO_Y_PIN = 19;

const int ADC_MAX = 4095;      // ESP32 ADC resolution (12-bit)
const int ADC_CENTER = ADC_MAX / 2;
const int DEADZONE = 200;      // ignore small drift around center

Servo servoX;
Servo servoY;

int readAxisAngle(int pin) {
  int raw = analogRead(pin);
  int offset = raw - ADC_CENTER;

  if (abs(offset) < DEADZONE) {
    offset = 0;
  }

  return map(offset, -ADC_CENTER, ADC_CENTER, 0, 180);
}

void setup() {
  Serial.begin(115200);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);

  servoX.setPeriodHertz(50);
  servoY.setPeriodHertz(50);
  servoX.attach(SERVO_X_PIN, 500, 2400);
  servoY.attach(SERVO_Y_PIN, 500, 2400);

  analogReadResolution(12);
}

void loop() {
  int angleX = readAxisAngle(JOY_X_PIN);
  int angleY = readAxisAngle(JOY_Y_PIN);

  servoX.write(angleX);
  servoY.write(angleY);

  Serial.printf("X: %d  Y: %d\n", angleX, angleY);

  delay(20);
}
