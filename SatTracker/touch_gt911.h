#pragma once
#include <Wire.h>
#include "config.h"

// Minimál GT911 driver saját megvalósítással (nincs külső könyvtár-függőség).
// Csak egyujjas érintést kezel, ami a UI-hoz elég.

static bool gt911_write_reg(uint16_t reg, uint8_t val) {
  Wire.beginTransmission(GT911_ADDR);
  Wire.write(reg >> 8);
  Wire.write(reg & 0xFF);
  Wire.write(val);
  return Wire.endTransmission() == 0;
}

static bool gt911_read_regs(uint16_t reg, uint8_t *buf, uint8_t len) {
  Wire.beginTransmission(GT911_ADDR);
  Wire.write(reg >> 8);
  Wire.write(reg & 0xFF);
  if (Wire.endTransmission(false) != 0) return false;
  Wire.requestFrom((int)GT911_ADDR, (int)len);
  for (uint8_t i = 0; i < len && Wire.available(); i++) {
    buf[i] = Wire.read();
  }
  return true;
}

inline void touch_init() {
  pinMode(TOUCH_RST, OUTPUT);
  digitalWrite(TOUCH_RST, LOW);
  delay(10);
  digitalWrite(TOUCH_RST, HIGH);
  delay(50);
  Wire.begin(TOUCH_SDA, TOUCH_SCL);
  Wire.setClock(400000);
}

// true, ha van érvényes érintési pont; x/y kimeneti paraméterekbe írja a koordinátát
inline bool touch_get_point(int16_t *x, int16_t *y) {
  uint8_t status;
  if (!gt911_read_regs(0x814E, &status, 1)) return false;
  if ((status & 0x80) == 0) return false;

  bool touched = false;
  uint8_t touchCount = status & 0x0F;
  if (touchCount > 0) {
    uint8_t buf[7];
    if (gt911_read_regs(0x8150, buf, 7)) {
      *x = buf[1] | (buf[2] << 8);
      *y = buf[3] | (buf[4] << 8);
      touched = true;
    }
  }

  gt911_write_reg(0x814E, 0); // buffer-státusz törlése -> következő olvasás engedélyezése
  return touched;
}
