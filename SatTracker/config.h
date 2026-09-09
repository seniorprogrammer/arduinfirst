#pragma once

// ============ Kijelző (RGB parallel, ST7262 vezérlő, 800x480) ============
// FONTOS: ellenőrizd a gyártói demo alapján a saját board revíziódra!
#define TFT_WIDTH   800
#define TFT_HEIGHT  480
#define GFX_BL      2   // háttérvilágítás PWM/GPIO

#define TFT_DE      40
#define TFT_VSYNC   41
#define TFT_HSYNC   39
#define TFT_PCLK    42

#define TFT_R0 45
#define TFT_R1 48
#define TFT_R2 47
#define TFT_R3 21
#define TFT_R4 14

#define TFT_G0 5
#define TFT_G1 6
#define TFT_G2 7
#define TFT_G3 15
#define TFT_G4 16
#define TFT_G5 4

#define TFT_B0 8
#define TFT_B1 3
#define TFT_B2 46
#define TFT_B3 9
#define TFT_B4 1

// RGB panel sync/porch időzítés (Sunton 800x480 panelek tipikus értékei)
#define RGB_HSYNC_POLARITY     0
#define RGB_HSYNC_FRONT_PORCH  8
#define RGB_HSYNC_PULSE_WIDTH  4
#define RGB_HSYNC_BACK_PORCH   8
#define RGB_VSYNC_POLARITY     0
#define RGB_VSYNC_FRONT_PORCH  8
#define RGB_VSYNC_PULSE_WIDTH  4
#define RGB_VSYNC_BACK_PORCH   8
#define RGB_PCLK_ACTIVE_NEG    1
#define RGB_PREFER_SPEED       16000000

// ============ Érintőpanel (GT911, I2C) ============
#define TOUCH_SDA  19
#define TOUCH_SCL  20
#define TOUCH_RST  38
#define GT911_ADDR 0x5D   // alternatív cím: 0x14 (ha nem reagál, ezt próbáld)

// ============ WiFi / NTP ============
#define WIFI_CONNECT_TIMEOUT_MS 10000
#define NTP_SERVER1 "pool.ntp.org"
#define NTP_SERVER2 "time.google.com"

// ============ Földi állomás (QTH) - Budapest ============
#define QTH_LAT   47.4979
#define QTH_LON   19.0402
#define QTH_ALT_M 120.0
