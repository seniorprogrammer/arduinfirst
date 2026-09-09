#pragma once
#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include "config.h"
#include "touch_gt911.h"

static Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
    TFT_DE, TFT_VSYNC, TFT_HSYNC, TFT_PCLK,
    TFT_R0, TFT_R1, TFT_R2, TFT_R3, TFT_R4,
    TFT_G0, TFT_G1, TFT_G2, TFT_G3, TFT_G4, TFT_G5,
    TFT_B0, TFT_B1, TFT_B2, TFT_B3, TFT_B4,
    RGB_HSYNC_POLARITY, RGB_HSYNC_FRONT_PORCH, RGB_HSYNC_PULSE_WIDTH, RGB_HSYNC_BACK_PORCH,
    RGB_VSYNC_POLARITY, RGB_VSYNC_FRONT_PORCH, RGB_VSYNC_PULSE_WIDTH, RGB_VSYNC_BACK_PORCH,
    RGB_PCLK_ACTIVE_NEG, RGB_PREFER_SPEED);

static Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
    TFT_WIDTH, TFT_HEIGHT, rgbpanel);

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *lvgl_buf1;
static lv_color_t *lvgl_buf2;

static void lvgl_flush_cb(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = area->x2 - area->x1 + 1;
  uint32_t h = area->y2 - area->y1 + 1;
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)color_p, w, h);
  lv_disp_flush_ready(disp);
}

static void lvgl_touch_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) {
  int16_t x, y;
  if (touch_get_point(&x, &y)) {
    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = x;
    data->point.y = y;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

inline void display_init() {
  gfx->begin();
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);

  lv_init();
  touch_init();

  // A sor-buffereket PSRAM-ba tesszük (8048S043-on elérhető)
  size_t bufPixels = TFT_WIDTH * 40;
  lvgl_buf1 = (lv_color_t *)heap_caps_malloc(bufPixels * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
  lvgl_buf2 = (lv_color_t *)heap_caps_malloc(bufPixels * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
  lv_disp_draw_buf_init(&draw_buf, lvgl_buf1, lvgl_buf2, bufPixels);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = TFT_WIDTH;
  disp_drv.ver_res = TFT_HEIGHT;
  disp_drv.flush_cb = lvgl_flush_cb;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = lvgl_touch_read_cb;
  lv_indev_drv_register(&indev_drv);
}
