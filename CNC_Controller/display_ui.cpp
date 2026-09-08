#include "display_ui.h"
#include <TFT_eSPI.h>

static TFT_eSPI tft = TFT_eSPI();

// Cache last-drawn status values so displayUpdateStatus() only repaints
// the digits that changed, avoiding full-screen flicker during motion.
static float lastX = NAN, lastY = NAN;
static bool lastRunning = false;
static bool statusInitialized = false;

void displayInit() {
  tft.init();
  tft.setRotation(1); // landscape, adjust for your panel/mount
  tft.fillScreen(TFT_BLACK);
}

void displayShowMain() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("CNC Controller");

  tft.drawLine(0, 35, tft.width(), 35, TFT_DARKGREY);

  tft.setTextSize(3);
  tft.setCursor(10, 60);
  tft.print("X:");
  tft.setCursor(10, 110);
  tft.print("Y:");

  tft.setTextSize(2);
  tft.setCursor(10, tft.height() - 60);
  tft.print("A=Jog  D=Move-to  C=Home");
  tft.setCursor(10, tft.height() - 35);
  tft.print("B=Settings");

  statusInitialized = false;
  lastX = lastY = NAN;
  lastRunning = false;
}

void displayUpdateStatus(float xMm, float yMm, bool running) {
  const int valX = 70;
  tft.setTextSize(3);

  if (!statusInitialized || xMm != lastX) {
    tft.fillRect(valX, 60, tft.width() - valX, 30, TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setCursor(valX, 60);
    tft.printf("%.2f", xMm);
    lastX = xMm;
  }

  if (!statusInitialized || yMm != lastY) {
    tft.fillRect(valX, 110, tft.width() - valX, 30, TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setCursor(valX, 110);
    tft.printf("%.2f", yMm);
    lastY = yMm;
  }

  if (!statusInitialized || running != lastRunning) {
    tft.fillRect(10, 160, tft.width() - 10, 25, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(running ? TFT_YELLOW : TFT_DARKGREY, TFT_BLACK);
    tft.setCursor(10, 160);
    tft.print(running ? "MOVING" : "IDLE");
    lastRunning = running;
  }

  statusInitialized = true;
}

void displayShowEntry(const char *label, const String &buffer) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print(label);

  tft.drawRect(10, 50, tft.width() - 20, 40, TFT_DARKGREY);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(20, 60);
  tft.print(buffer);

  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(10, tft.height() - 35);
  tft.print("D=Confirm  C=Cancel  B=Del");
}

void displayShowHoming() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(20, tft.height() / 2 - 20);
  tft.print("HOMING...");
}

void displayShowSettings(const CncSettings &s) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("Settings");

  tft.setCursor(10, 45);
  tft.printf("Steps/mm X: %.2f", s.stepsPerMmX);
  tft.setCursor(10, 70);
  tft.printf("Steps/mm Y: %.2f", s.stepsPerMmY);
  tft.setCursor(10, 95);
  tft.printf("Max speed: %.1f mm/s", s.maxSpeedMmS);
  tft.setCursor(10, 120);
  tft.printf("Accel: %.1f mm/s^2", s.accelMmS2);
  tft.setCursor(10, 145);
  tft.printf("Home speed: %.1f mm/s", s.homingSpeedMmS);

  tft.setCursor(10, tft.height() - 35);
  tft.print("C=Back");
}

void displayShowError(const String &message) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("ERROR");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(10, 45);
  tft.println(message);
  tft.setCursor(10, tft.height() - 35);
  tft.print("C=Dismiss");
}
