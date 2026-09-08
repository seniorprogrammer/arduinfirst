# arduinfirst

Arduino IDE sketches for ESP32 boards.

## Getting started

1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Add ESP32 board support via **File > Preferences > Additional Boards Manager URLs**:
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. Install the "esp32" package from **Tools > Board > Boards Manager**.
4. Open a sketch folder below in the Arduino IDE, select your ESP32 board under **Tools > Board**, and upload.

## Sketches

- [`NTP_OLED_Clock/`](NTP_OLED_Clock/NTP_OLED_Clock.ino) — for an ESP32-C3 SuperMini with a 0.42" SSD1306 I2C OLED (72x40). Connects to WiFi, syncs the time from an NTP server, and displays the current time and date. Requires the "Adafruit SSD1306", "Adafruit GFX Library", and "Adafruit BusIO" libraries from the Library Manager. Edit the WiFi credentials, time zone offsets, and I2C pin definitions at the top of the sketch before uploading.
