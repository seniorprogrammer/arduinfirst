# arduinfirst

Arduino IDE sketches for ESP32 boards.

## Getting started

1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Add ESP32 board support via **File > Preferences > Additional Boards Manager URLs**:
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. Install the "esp32" package from **Tools > Board > Boards Manager**.
4. Open a sketch folder below in the Arduino IDE, select your ESP32 board under **Tools > Board**, and upload.

## Sketches

- [`NTP_GC9A01_Clock/`](NTP_GC9A01_Clock/NTP_GC9A01_Clock.ino) — connects to WiFi, syncs the time from an NTP server, and displays the current time and date on a round GC9A01 SPI TFT display. Requires the "GFX Library for Arduino" (`Arduino_GFX_Library`) from the Library Manager. Edit the WiFi credentials, time zone offsets, and display pin definitions at the top of the sketch before uploading.
