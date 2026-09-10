#pragma once
#include <Wire.h>
#include "config.h"

// Minimál GT911 driver saját megvalósítással (nincs külső könyvtár-függőség).
// Csak egyujjas érintést kezel, ami a UI-hoz elég.
// Az I2C cím board-revíziónként eltérhet (0x5D vagy 0x14), ezért induláskor
// mindkettőt megpróbáljuk és a soros portra kiírjuk, melyik válaszolt.

static uint8_t gt911_addr = 0;

static bool gt911_write_reg(uint16_t reg, uint8_t val) {
  Wire.beginTransmission(gt911_addr);
  Wire.write(reg >> 8);
  Wire.write(reg & 0xFF);
  Wire.write(val);
  return Wire.endTransmission() == 0;
}

static bool gt911_read_regs(uint16_t reg, uint8_t *buf, uint8_t len) {
  Wire.beginTransmission(gt911_addr);
  Wire.write(reg >> 8);
  Wire.write(reg & 0xFF);
  if (Wire.endTransmission(false) != 0) return false;
  Wire.requestFrom((int)gt911_addr, (int)len);
  for (uint8_t i = 0; i < len && Wire.available(); i++) {
    buf[i] = Wire.read();
  }
  return true;
}

static bool i2c_probe(uint8_t addr) {
  Wire.beginTransmission(addr);
  return Wire.endTransmission() == 0;
}

inline void touch_init() {
  if (TOUCH_RST >= 0) {
    pinMode(TOUCH_RST, OUTPUT);
    digitalWrite(TOUCH_RST, LOW);
    delay(10);
    digitalWrite(TOUCH_RST, HIGH);
    delay(50);
  }

  Wire.begin(TOUCH_SDA, TOUCH_SCL);
  Wire.setClock(400000);
  delay(10);

  Serial.println("Touch: I2C busz vizsgalata...");
  bool found = false;
  for (uint8_t addr = 1; addr < 127; addr++) {
    if (i2c_probe(addr)) {
      Serial.printf("Touch: I2C eszkoz talalva cimen 0x%02X\n", addr);
      if (addr == 0x5D || addr == 0x14) {
        gt911_addr = addr;
        found = true;
      }
    }
  }

  if (found) {
    Serial.printf("Touch: GT911 cim beallitva: 0x%02X\n", gt911_addr);
  } else {
    Serial.println("Touch: GT911 nem valaszolt egyik ismert cimen sem (0x5D / 0x14)! "
                    "Ellenorizd a TOUCH_SDA/TOUCH_SCL/TOUCH_RST pineket a config.h-ban.");
    gt911_addr = GT911_ADDR;
  }
}

// true, ha van érvényes érintési pont; x/y kimeneti paraméterekbe írja a koordinátát
inline bool touch_get_point(int16_t *x, int16_t *y) {
  if (gt911_addr == 0) return false;

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
      Serial.printf("Touch: x=%d y=%d\n", *x, *y);
    }
  }

  gt911_write_reg(0x814E, 0); // buffer-státusz törlése -> következő olvasás engedélyezése
  return touched;
}
