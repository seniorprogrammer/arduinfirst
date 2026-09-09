#include <WiFi.h>
#include "config.h"
#include "display_lvgl.h"
#include "wifi_ui.h"

// Ehhez a sketch-hez az lv_conf.h-ban LV_TICK_CUSTOM legyen 0 -
// igy az lv_tick_inc()-et mi magunk hivjuk meg a loop()-ban.
static uint32_t last_tick;

void setup() {
  Serial.begin(115200);
  display_init();

  last_tick = millis();

  if (wifi_try_autoconnect()) {
    show_main_screen();
  } else {
    show_wifi_setup_screen();
  }
}

void loop() {
  uint32_t now = millis();
  lv_tick_inc(now - last_tick);
  last_tick = now;

  lv_timer_handler();
  delay(5);
}
