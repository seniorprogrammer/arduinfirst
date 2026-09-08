// Displays the current time and date on a round GC9A01 SPI TFT display,
// synced from an NTP server over WiFi.
//
// Board:   any ESP32 dev board
// Display: GC9A01 240x240 round SPI TFT
// Library: "GFX Library for Arduino" by moononournation (Arduino_GFX_Library)
//          Install via Library Manager, then also install its "Arduino_BusIO"
//          dependency if prompted.
//
// Wire the display to the pins defined below (or edit them to match your
// wiring), fill in your WiFi credentials, and upload.

#include <Arduino_GFX_Library.h>
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
const int PIN_TFT_CS = 5;
const int PIN_TFT_DC = 2;
const int PIN_TFT_RST = 4;
const int PIN_TFT_SCK = 18;
const int PIN_TFT_MOSI = 23;
const int PIN_TFT_BL = 15;  // backlight, -1 if not connected

Arduino_DataBus *bus = new Arduino_ESP32SPI(PIN_TFT_DC, PIN_TFT_CS, PIN_TFT_SCK, PIN_TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_GC9A01(bus, PIN_TFT_RST, 0 /* rotation */, true /* IPS */);

const int SCREEN_SIZE = 240;
const int CENTER = SCREEN_SIZE / 2;

char lastTimeStr[16] = "";
char lastDateStr[32] = "";

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  gfx->setTextColor(WHITE);
  gfx->setTextSize(2);
  gfx->setCursor(CENTER - 60, CENTER - 10);
  gfx->print("Connecting WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void syncTime() {
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

  struct tm timeinfo;
  gfx->fillScreen(BLACK);
  gfx->setTextColor(WHITE);
  gfx->setTextSize(2);
  gfx->setCursor(CENTER - 55, CENTER - 10);
  gfx->print("Syncing time...");

  while (!getLocalTime(&timeinfo)) {
    delay(500);
  }
}

void setup() {
  gfx->begin();
  gfx->fillScreen(BLACK);

  if (PIN_TFT_BL >= 0) {
    pinMode(PIN_TFT_BL, OUTPUT);
    digitalWrite(PIN_TFT_BL, HIGH);
  }

  connectWiFi();
  syncTime();
  gfx->fillScreen(BLACK);
}

void drawCentered(const char *text, int y, int textSize, uint16_t color) {
  int16_t x1, y1;
  uint16_t w, h;
  gfx->setTextSize(textSize);
  gfx->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  gfx->setCursor(CENTER - w / 2, y);
  gfx->setTextColor(color, BLACK);
  gfx->print(text);
}

void loop() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeStr[16];
    char dateStr[32];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
    strftime(dateStr, sizeof(dateStr), "%A, %d %B %Y", &timeinfo);

    if (strcmp(timeStr, lastTimeStr) != 0) {
      drawCentered(timeStr, CENTER - 20, 4, WHITE);
      strcpy(lastTimeStr, timeStr);
    }

    if (strcmp(dateStr, lastDateStr) != 0) {
      drawCentered(dateStr, CENTER + 30, 1, CYAN);
      strcpy(lastDateStr, dateStr);
    }
  }

  delay(200);
}
