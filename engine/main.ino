// Arduino sketch

#include <Arduino.h>
#include "candle.h"

/* choose 3 PWM-capable pins
  Pin	Function	LED Type
  PD2 -> 3	Flame Core	Warm white
  PD3 -> 5	Flame Edge	Amber / warm
  PD4 -> 6	Ember Base	Red / orange
*/

const uint8_t pins[3] = {3, 5, 6};

// required by candle.h
void hw_set_led(uint8_t ch, uint8_t value) {
analogWrite(pins[ch], value);
}

void setup() {

for (int i = 0; i < 3; i++) {
  pinMode(pins[i], OUTPUT);
  }

candle_init();
}

void loop() {
candle_update();
delay(25); // update rate
}
