// Displays the current time and date on a tiny I2C OLED display,
// synced from an NTP server over WiFi.
//
// Board:   ESP32-C3 SuperMini
// Display: 0.42" SSD1306 OLED, 72x40 pixels, I2C
// Libraries: "Adafruit SSD1306" and its dependencies "Adafruit GFX Library"
//            and "Adafruit BusIO" — install all three via Library Manager.
//
// Fill in your WiFi credentials and, if your board wires the OLED to
// different pins, edit PIN_OLED_SDA/PIN_OLED_SCL below, then upload.

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <WiFi.h>
#include <time.h>

// ---- WiFi credentials ----
const char *WIFI_SSID = "YOUR_WIFI_SSID";
const char *WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// ---- NTP / time zone settings ----
const char *NTP_SERVER = "pool.ntp.org";
// Offsets are in seconds. Example below is UTC+1 with 1 hour of DST (CET/CEST).
const long GMT_OFFSET_SEC = 1 * 3600;
const int DAYLIGHT_OFFSET_SEC = 1 * 3600;

// ---- OLED wiring (edit to match your board) ----
// ESP32-C3's default I2C pins, used by most integrated
// "C3 SuperMini + 0.42 OLED" boards.
const int PIN_OLED_SDA = 8;
const int PIN_OLED_SCL = 9;
const uint8_t OLED_I2C_ADDRESS = 0x3C;

const int SCREEN_WIDTH = 72;
const int SCREEN_HEIGHT = 40;

Adafruit_SSD1306 gfx(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

char lastTimeStr[16] = "";
char lastDateStr[16] = "";

void showMessage(const char *text) {
  gfx.clearDisplay();
  gfx.setTextSize(1);
  gfx.setTextColor(SSD1306_WHITE);
  gfx.setCursor(0, 0);
  gfx.print(text);
  gfx.display();
}

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  showMessage("Connecting WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void syncTime() {
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

  struct tm timeinfo;
  showMessage("Syncing time...");

  while (!getLocalTime(&timeinfo)) {
    delay(500);
  }
}

void setup() {
  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
  gfx.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS);
  gfx.clearDisplay();
  gfx.display();

  connectWiFi();
  syncTime();
  gfx.clearDisplay();
  gfx.display();
}

void drawCentered(const char *text, int y, uint16_t color) {
  int16_t x1, y1;
  uint16_t w, h;
  gfx.setTextSize(1);
  gfx.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  gfx.setCursor((SCREEN_WIDTH - w) / 2, y);
  gfx.setTextColor(color);
  gfx.print(text);
}

void loop() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeStr[16];
    char dateStr[16];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
    strftime(dateStr, sizeof(dateStr), "%d.%m.%Y", &timeinfo);

    if (strcmp(timeStr, lastTimeStr) != 0 || strcmp(dateStr, lastDateStr) != 0) {
      gfx.clearDisplay();
      drawCentered(timeStr, 12, SSD1306_WHITE);
      drawCentered(dateStr, 24, SSD1306_WHITE);
      gfx.display();

      strcpy(lastTimeStr, timeStr);
      strcpy(lastDateStr, dateStr);
    }
  }

  delay(200);
}
