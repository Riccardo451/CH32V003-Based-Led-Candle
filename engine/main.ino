#include "candle.h"

void hw_set_led(uint8_t ch, uint8_t value) {
    analogWrite(ch + 3, value); // pins 3,4,5
}

void setup() {
    candle_init();
}

void loop() {
    candle_update();
    delay(5);
}
