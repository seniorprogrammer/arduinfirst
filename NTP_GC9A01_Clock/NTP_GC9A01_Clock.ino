// Displays the current time and date on a round GC9A01 SPI TFT display,
// synced from an NTP server over WiFi.
//
// Board:   ESP32-C3 SuperMini
// Display: GC9A01 240x240 round SPI TFT
// Libraries: "Adafruit GC9A01A" and its dependencies "Adafruit GFX Library"
//            and "Adafruit BusIO" — install all three via Library Manager.
//
// Wire the display to the pins defined below (or edit them to match your
// wiring), fill in your WiFi credentials, and upload. The pins below avoid
// the C3 SuperMini's strapping pins (GPIO8, GPIO9) and its USB/serial pins
// (GPIO18, GPIO19, GPIO20, GPIO21).

#include <Adafruit_GC9A01A.h>
#include <SPI.h>
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

// ---- GC9A01 pin wiring (edit to match your board) ----
const int PIN_TFT_CS = 7;
const int PIN_TFT_DC = 2;
const int PIN_TFT_RST = 10;
const int PIN_TFT_SCK = 4;
const int PIN_TFT_MOSI = 6;
const int PIN_TFT_BL = 3;  // backlight, -1 if not connected

Adafruit_GC9A01A gfx(&SPI, PIN_TFT_DC, PIN_TFT_CS, PIN_TFT_RST);

const int SCREEN_SIZE = 240;
const int CENTER = SCREEN_SIZE / 2;

char lastTimeStr[16] = "";
char lastDateStr[32] = "";

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  gfx.setTextColor(GC9A01A_WHITE);
  gfx.setTextSize(2);
  gfx.setCursor(CENTER - 60, CENTER - 10);
  gfx.print("Connecting WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void syncTime() {
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

  struct tm timeinfo;
  gfx.fillScreen(GC9A01A_BLACK);
  gfx.setTextColor(GC9A01A_WHITE);
  gfx.setTextSize(2);
  gfx.setCursor(CENTER - 55, CENTER - 10);
  gfx.print("Syncing time...");

  while (!getLocalTime(&timeinfo)) {
    delay(500);
  }
}

void setup() {
  SPI.begin(PIN_TFT_SCK, -1 /* MISO unused */, PIN_TFT_MOSI, PIN_TFT_CS);

  gfx.begin();
  gfx.setRotation(0);
  gfx.fillScreen(GC9A01A_BLACK);

  if (PIN_TFT_BL >= 0) {
    pinMode(PIN_TFT_BL, OUTPUT);
    digitalWrite(PIN_TFT_BL, HIGH);
  }

  connectWiFi();
  syncTime();
  gfx.fillScreen(GC9A01A_BLACK);
}

void drawCentered(const char *text, int y, int textSize, uint16_t color) {
  int16_t x1, y1;
  uint16_t w, h;
  gfx.setTextSize(textSize);
  gfx.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  gfx.setCursor(CENTER - w / 2, y);
  gfx.setTextColor(color, GC9A01A_BLACK);
  gfx.print(text);
}

void loop() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeStr[16];
    char dateStr[32];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
    strftime(dateStr, sizeof(dateStr), "%A, %d %B %Y", &timeinfo);

    if (strcmp(timeStr, lastTimeStr) != 0) {
      drawCentered(timeStr, CENTER - 20, 4, GC9A01A_WHITE);
      strcpy(lastTimeStr, timeStr);
    }

    if (strcmp(dateStr, lastDateStr) != 0) {
      drawCentered(dateStr, CENTER + 30, 1, GC9A01A_CYAN);
      strcpy(lastDateStr, dateStr);
    }
  }

  delay(200);
}
