// CH32V003 main.c

#include "ch32v00x.h"
#include "candle.h"

// ---- hardware layer ----
void hw_set_led(uint8_t ch, uint8_t value) {
if (ch == 0) TIM1->CH1CVR = value;
else if (ch == 1) TIM1->CH2CVR = value;
else if (ch == 2) TIM1->CH3CVR = value;
}

// ---- simple delay ----
void delay_ms(uint32_t ms) {
for (uint32_t i = 0; i < ms * 8000; i++) {
__NOP();
}
}

// ---- PWM init (basic TIM1 setup) ----
void pwm_init(void) {
RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOD, ENABLE);

```
// PD2, PD3, PD4 as AF push-pull
GPIOD->CFGLR &= ~(0xFFF << (2 * 4));
GPIOD->CFGLR |=  (0xBBB << (2 * 4));

TIM1->PSC = 0;
TIM1->ATRLR = 255;

TIM1->CHCTLR1 |= (6 << 4) | (6 << 12); // CH1, CH2
TIM1->CHCTLR2 |= (6 << 4);             // CH3

TIM1->CCER |= TIM_CC1E | TIM_CC2E | TIM_CC3E;
TIM1->BDTR |= TIM_MOE;
TIM1->CTLR1 |= TIM_CEN;
```

}

// ---- MAIN ----
int main(void) {

```
pwm_init();
candle_init();

while (1) {
    candle_update();
    delay_ms(5); // ~200 Hz
}
```

}
