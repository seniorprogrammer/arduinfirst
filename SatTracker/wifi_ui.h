#pragma once
#include <WiFi.h>
#include <Preferences.h>
#include <time.h>
#include <lvgl.h>
#include "config.h"

static Preferences prefs;

static lv_obj_t *scr_wifi;
static lv_obj_t *scr_main;
static lv_obj_t *wifi_list;
static lv_obj_t *pass_ta;
static lv_obj_t *status_label;
static lv_obj_t *clock_label;
static lv_obj_t *ip_label;
static String selected_ssid;

void show_main_screen();
void show_wifi_setup_screen();

static void connect_btn_cb(lv_event_t *e) {
  const char *pass = lv_textarea_get_text(pass_ta);
  lv_label_set_text(status_label, "Csatlakozas...");
  lv_timer_handler(); // status label frissuljon a kepernyon azonnal

  WiFi.begin(selected_ssid.c_str(), pass);

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_CONNECT_TIMEOUT_MS) {
    delay(50);
  }

  if (WiFi.status() == WL_CONNECTED) {
    prefs.putString("ssid", selected_ssid);
    prefs.putString("pass", pass);
    show_main_screen();
  } else {
    lv_label_set_text(status_label, "Sikertelen csatlakozas, probald ujra.");
  }
}

static void ssid_btn_cb(lv_event_t *e) {
  lv_obj_t *btn = lv_event_get_target(e);
  selected_ssid = lv_list_get_btn_text(wifi_list, btn);

  lv_obj_clean(scr_wifi);

  lv_obj_t *title = lv_label_create(scr_wifi);
  lv_label_set_text_fmt(title, "Halozat: %s", selected_ssid.c_str());
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

  pass_ta = lv_textarea_create(scr_wifi);
  lv_textarea_set_placeholder_text(pass_ta, "Jelszo");
  lv_textarea_set_password_mode(pass_ta, true);
  lv_textarea_set_one_line(pass_ta, true);
  lv_obj_set_width(pass_ta, 400);
  lv_obj_align(pass_ta, LV_ALIGN_TOP_MID, 0, 50);

  lv_obj_t *connect_btn = lv_btn_create(scr_wifi);
  lv_obj_align(connect_btn, LV_ALIGN_TOP_MID, 0, 100);
  lv_obj_t *cb_lbl = lv_label_create(connect_btn);
  lv_label_set_text(cb_lbl, "Csatlakozas");
  lv_obj_add_event_cb(connect_btn, connect_btn_cb, LV_EVENT_CLICKED, NULL);

  status_label = lv_label_create(scr_wifi);
  lv_obj_align(status_label, LV_ALIGN_TOP_MID, 0, 150);
  lv_label_set_text(status_label, "");

  lv_obj_t *kb = lv_keyboard_create(scr_wifi);
  lv_keyboard_set_textarea(kb, pass_ta);
  lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
}

static void scan_btn_cb(lv_event_t *e) {
  lv_obj_clean(wifi_list);
  lv_obj_t *l = lv_label_create(wifi_list);
  lv_label_set_text(l, "Kereses...");
  lv_timer_handler();

  int n = WiFi.scanNetworks();
  lv_obj_clean(wifi_list);
  for (int i = 0; i < n; i++) {
    lv_obj_t *btn = lv_list_add_btn(wifi_list, LV_SYMBOL_WIFI, WiFi.SSID(i).c_str());
    lv_obj_add_event_cb(btn, ssid_btn_cb, LV_EVENT_CLICKED, NULL);
  }
}

void show_wifi_setup_screen() {
  scr_wifi = lv_obj_create(NULL);

  lv_obj_t *title = lv_label_create(scr_wifi);
  lv_label_set_text(title, "WiFi beallitas");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

  lv_obj_t *scan_btn = lv_btn_create(scr_wifi);
  lv_obj_align(scan_btn, LV_ALIGN_TOP_MID, 0, 40);
  lv_obj_t *lbl = lv_label_create(scan_btn);
  lv_label_set_text(lbl, "Halozatok keresese");
  lv_obj_add_event_cb(scan_btn, scan_btn_cb, LV_EVENT_CLICKED, NULL);

  wifi_list = lv_list_create(scr_wifi);
  lv_obj_set_size(wifi_list, 400, 300);
  lv_obj_align(wifi_list, LV_ALIGN_TOP_MID, 0, 90);

  lv_scr_load(scr_wifi);
}

static void clock_timer_cb(lv_timer_t *t) {
  time_t now;
  time(&now);
  struct tm tm_utc;
  gmtime_r(&now, &tm_utc);
  char buf[40];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S UTC", &tm_utc);
  lv_label_set_text(clock_label, buf);
}

void show_main_screen() {
  scr_main = lv_obj_create(NULL);

  lv_obj_t *title = lv_label_create(scr_main);
  lv_label_set_text(title, "Muhold kovveto - Budapest");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

  ip_label = lv_label_create(scr_main);
  lv_label_set_text_fmt(ip_label, "WiFi: %s  IP: %s",
                         WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
  lv_obj_align(ip_label, LV_ALIGN_TOP_MID, 0, 50);

  clock_label = lv_label_create(scr_main);
  lv_obj_align(clock_label, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_text_font(clock_label, &lv_font_montserrat_14, 0);
  lv_label_set_text(clock_label, "Ido szinkronizalasa...");

  lv_obj_t *next_label = lv_label_create(scr_main);
  lv_label_set_text(next_label, "Kovetkezo fazis: TLE letoltes es palyaszamitas");
  lv_obj_align(next_label, LV_ALIGN_BOTTOM_MID, 0, -20);

  lv_timer_create(clock_timer_cb, 1000, NULL);

  configTime(0, 0, NTP_SERVER1, NTP_SERVER2); // UTC-ben szinkronizal, mert a palyaszamitas UTC-t igenyel

  lv_scr_load(scr_main);
}

inline bool wifi_try_autoconnect() {
  prefs.begin("wifi", false);
  String ssid = prefs.getString("ssid", "");
  String pass = prefs.getString("pass", "");
  if (ssid.length() == 0) return false;

  WiFi.begin(ssid.c_str(), pass.c_str());
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_CONNECT_TIMEOUT_MS) {
    delay(100);
  }
  return WiFi.status() == WL_CONNECTED;
}
