// Blinks the onboard LED on an ESP32 board.
// Most ESP32 dev boards expose the onboard LED on GPIO 2;
// change LED_PIN if your board wires it differently.

const int LED_PIN = 2;
const int BLINK_DELAY_MS = 1000;

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(BLINK_DELAY_MS);
  digitalWrite(LED_PIN, LOW);
  delay(BLINK_DELAY_MS);
}
