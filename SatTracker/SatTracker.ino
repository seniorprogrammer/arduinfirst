#include <WiFi.h>
#include "config.h"
#include "display_lvgl.h"
#include "wifi_ui.h"

// Ha lv_conf.h-ban LV_TICK_CUSTOM engedelyezve van, az lv_tick_inc() nincs
// definialva - helyette lv_tick_set_cb()-vel kell regisztralni a forras fuggvenyt.
#if LV_TICK_CUSTOM
static uint32_t my_tick_get_cb(void) {
  return millis();
}
#else
static uint32_t last_tick;
#endif

void setup() {
  Serial.begin(115200);
  display_init();

#if LV_TICK_CUSTOM
  lv_tick_set_cb(my_tick_get_cb);
#else
  last_tick = millis();
#endif

  if (wifi_try_autoconnect()) {
    show_main_screen();
  } else {
    show_wifi_setup_screen();
  }
}

void loop() {
#if !LV_TICK_CUSTOM
  uint32_t now = millis();
  lv_tick_inc(now - last_tick);
  last_tick = now;
#endif

  lv_timer_handler();
  delay(5);
}
