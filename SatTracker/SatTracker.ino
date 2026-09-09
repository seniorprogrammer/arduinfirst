#include <WiFi.h>
#include "config.h"
#include "display_lvgl.h"
#include "wifi_ui.h"

static uint32_t last_tick;

void setup() {
  Serial.begin(115200);
  display_init();

  if (wifi_try_autoconnect()) {
    show_main_screen();
  } else {
    show_wifi_setup_screen();
  }

  last_tick = millis();
}

void loop() {
  uint32_t now = millis();
  lv_tick_inc(now - last_tick);
  last_tick = now;

  lv_timer_handler();
  delay(5);
}
