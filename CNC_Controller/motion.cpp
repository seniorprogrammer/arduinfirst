#include "motion.h"
#include "config.h"
#include <AccelStepper.h>

static AccelStepper stepperX(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);
static AccelStepper stepperY(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);

static CncSettings settings;
static TaskHandle_t motionTaskHandle = nullptr;
static portMUX_TYPE motionMux = portMUX_INITIALIZER_UNLOCKED;

static float clampf(float v, float lo, float hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

static void applySpeedAccel() {
  stepperX.setMaxSpeed(settings.maxSpeedMmS * settings.stepsPerMmX);
  stepperX.setAcceleration(settings.accelMmS2 * settings.stepsPerMmX);
  stepperY.setMaxSpeed(settings.maxSpeedMmS * settings.stepsPerMmY);
  stepperY.setAcceleration(settings.accelMmS2 * settings.stepsPerMmY);
}

static void motionTask(void *param) {
  for (;;) {
    portENTER_CRITICAL(&motionMux);
    stepperX.run();
    stepperY.run();
    portEXIT_CRITICAL(&motionMux);
    // No delay: AccelStepper::run() is non-blocking and must be polled
    // as often as possible for smooth high-speed stepping.
    taskYIELD();
  }
}

void motionInit(const CncSettings &s) {
  settings = s;

  pinMode(STEPPERS_EN_PIN, OUTPUT);
  motionEnableDrivers(false); // start disabled until homed/used

  pinMode(X_LIMIT_PIN, INPUT);
  pinMode(Y_LIMIT_PIN, INPUT);

  applySpeedAccel();

  xTaskCreatePinnedToCore(motionTask, "motionTask", 4096, nullptr, 3,
                           &motionTaskHandle, 0 /* core 0 */);
}

void motionApplySettings(const CncSettings &s) {
  settings = s;
  applySpeedAccel();
}

void motionEnableDrivers(bool enable) {
  // Most STEP/DIR drivers (A4988/DRV8825/TMC2209 legacy mode) use
  // active-LOW enable.
  digitalWrite(STEPPERS_EN_PIN, enable ? LOW : HIGH);
}

void motionMoveToMm(float xMm, float yMm) {
  xMm = clampf(xMm, SOFT_LIMIT_MIN_X_MM, SOFT_LIMIT_MAX_X_MM);
  yMm = clampf(yMm, SOFT_LIMIT_MIN_Y_MM, SOFT_LIMIT_MAX_Y_MM);
  motionEnableDrivers(true);
  portENTER_CRITICAL(&motionMux);
  stepperX.moveTo(xMm * settings.stepsPerMmX);
  stepperY.moveTo(yMm * settings.stepsPerMmY);
  portEXIT_CRITICAL(&motionMux);
}

void motionJogMm(float dxMm, float dyMm) {
  float targetX = motionGetXMm() + dxMm;
  float targetY = motionGetYMm() + dyMm;
  motionMoveToMm(targetX, targetY);
}

void motionStop() {
  portENTER_CRITICAL(&motionMux);
  stepperX.stop();
  stepperY.stop();
  portEXIT_CRITICAL(&motionMux);
}

void motionEmergencyStop() {
  portENTER_CRITICAL(&motionMux);
  stepperX.setSpeed(0);
  stepperY.setSpeed(0);
  stepperX.moveTo(stepperX.currentPosition());
  stepperY.moveTo(stepperY.currentPosition());
  portEXIT_CRITICAL(&motionMux);
  motionEnableDrivers(false);
}

bool motionIsRunning() {
  portENTER_CRITICAL(&motionMux);
  bool running = (stepperX.distanceToGo() != 0) || (stepperY.distanceToGo() != 0);
  portEXIT_CRITICAL(&motionMux);
  return running;
}

float motionGetXMm() {
  return stepperX.currentPosition() / settings.stepsPerMmX;
}

float motionGetYMm() {
  return stepperY.currentPosition() / settings.stepsPerMmY;
}

static bool limitTriggered(int pin) {
  int reading = digitalRead(pin);
  return LIMIT_ACTIVE_LOW ? (reading == LOW) : (reading == HIGH);
}

bool motionHome() {
  motionEnableDrivers(true);

  float homeSpeedX = settings.homingSpeedMmS * settings.stepsPerMmX;
  float homeSpeedY = settings.homingSpeedMmS * settings.stepsPerMmY;

  // Move toward negative direction (toward home switches) until triggered.
  portENTER_CRITICAL(&motionMux);
  stepperX.setMaxSpeed(homeSpeedX);
  stepperY.setMaxSpeed(homeSpeedY);
  stepperX.moveTo(-1000000L);
  stepperY.moveTo(-1000000L);
  portEXIT_CRITICAL(&motionMux);

  const uint32_t timeoutMs = 60000;
  uint32_t start = millis();
  bool xHomed = false, yHomed = false;

  while (!(xHomed && yHomed)) {
    if (millis() - start > timeoutMs) {
      motionEmergencyStop();
      applySpeedAccel();
      return false;
    }
    if (!xHomed && limitTriggered(X_LIMIT_PIN)) {
      portENTER_CRITICAL(&motionMux);
      stepperX.stop();
      stepperX.setCurrentPosition(0);
      portEXIT_CRITICAL(&motionMux);
      xHomed = true;
    }
    if (!yHomed && limitTriggered(Y_LIMIT_PIN)) {
      portENTER_CRITICAL(&motionMux);
      stepperY.stop();
      stepperY.setCurrentPosition(0);
      portEXIT_CRITICAL(&motionMux);
      yHomed = true;
    }
    delay(2);
  }

  applySpeedAccel();
  return true;
}
