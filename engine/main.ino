// Arduino sketch

#include <Arduino.h>
#include "candle.h"

// choose 3 PWM-capable pins
const uint8_t pins[3] = {3, 5, 6};

// required by candle.h
void hw_set_led(uint8_t ch, uint8_t value) {
analogWrite(pins[ch], value);
}

void setup() {
for (int i = 0; i < 3; i++) {
pinMode(pins[i], OUTPUT);
}

```
candle_init();
```

}

void loop() {
candle_update();
delay(5); // ~200 Hz update rate
}
