#include "keypad_input.h"
#include "config.h"
#include <Keypad.h>

static const byte ROWS = 4;
static const byte COLS = 4;

static char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

static byte rowPins[ROWS] = KEYPAD_ROW_PINS;
static byte colPins[COLS] = KEYPAD_COL_PINS;

static Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void keypadInit() {
  keypad.setDebounceTime(15);
}

char keypadPoll() {
  return keypad.getKey(); // returns NO_KEY (0) if nothing pressed
}
