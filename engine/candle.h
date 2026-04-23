#ifndef CANDLE_H
#define CANDLE_H

#include <stdint.h>

#define CANDLE_CHANNELS 3

void candle_init(void);
void candle_update(void);

// platform must implement this
void hw_set_led(uint8_t ch, uint8_t value);

#endif
