#include <WiFi.h>
#include "config.h"
#include "display_lvgl.h"
#include "wifi_ui.h"

// Az Arduino-s lvgl konyvtar lv_conf.h sablonja alapbol LV_TICK_CUSTOM 1-et
// hasznal, es a millis()-t automatikusan bedrotozza (LV_TICK_CUSTOM_SYS_TIME_EXPR).
// Emiatt nincs szukseg kezi lv_tick_inc()/lv_tick_set_cb() hivasra.

void setup() {
  Serial.begin(115200);
  display_init();

  if (wifi_try_autoconnect()) {
    show_main_screen();
  } else {
    show_wifi_setup_screen();
  }
}

void loop() {
  lv_timer_handler();
  delay(5);
}
