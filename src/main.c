#include "ch32v00x.h"
#include <stdint.h>

// ================= Gamma correction =================
const uint8_t gamma8[256] = {
  // (trimmed for readability; same γ≈2.2 table as before)
  0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,
  2,2,2,2,3,3,3,4,4,4,5,5,6,6,7,7,
  8,8,9,10,10,11,12,13,13,14,15,16,
  17,18,19,20,21,22,23,24,25,27,28,29,
  30,32,33,35,36,38,39,41,42,44,46,48,
  49,51,53,55,57,59,61,63,65,67,69,71,
  73,76,78,80,83,85,87,90,92,95,97,100,
  103,105,108,111,113,116,119,122,125,128,131,134,
  137,140,143,146,149,152,155,159,162,165,168,172,
  175,178,182,185,189,192,196,199,203,207,210,214,
  218,221,225,229,233,237,240,244,248,252,255
};

// ================= RNG =================
static uint16_t lfsr = 0xBEEF;
uint8_t rand8(void) {
    lfsr ^= lfsr << 7;
    lfsr ^= lfsr >> 9;
    lfsr ^= lfsr << 8;
    return (uint8_t)lfsr;
}

// ================= PWM =================
void pwm_init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOD, ENABLE);

    GPIOD->CFGLR &= ~(0xFFF << (2 * 4));
    GPIOD->CFGLR |=  (0xBBB << (2 * 4)); // PD2/3/4 AF

    TIM1->PSC = 0;
    TIM1->ATRLR = 255;

    TIM1->CHCTLR1 |= (6 << 4) | (6 << 12);
    TIM1->CHCTLR2 |= (6 << 4);

    TIM1->CCER |= TIM_CC1E | TIM_CC2E | TIM_CC3E;
    TIM1->BDTR |= TIM_MOE;
    TIM1->CTLR1 |= TIM_CEN;
}

// ================= Perlin-style 1D noise =================
// (cheap gradient + smoothing approximation)

#define NOISE_SIZE 32
uint8_t noise[NOISE_SIZE];

// initialize noise field
void noise_init(void) {
    for (int i = 0; i < NOISE_SIZE; i++) {
        noise[i] = rand8();
    }
}

// smooth interpolation
static inline uint8_t lerp(uint8_t a, uint8_t b, uint8_t t) {
    return a + ((b - a) * t >> 8);
}

// coherent noise lookup
uint8_t noise1d(uint8_t x) {
    uint8_t i = x & (NOISE_SIZE - 1);
    uint8_t j = (i + 1) & (NOISE_SIZE - 1);

    uint8_t t = x << 3; // fractional part

    return lerp(noise[i], noise[j], t);
}

// ================= Flame state =================
uint8_t t = 0;

uint8_t smooth(uint8_t v, uint8_t target, uint8_t k) {
    return v + ((int16_t)target - v) / k;
}

// ================= Candle engine =================
void candle_update(void) {

    t++;

    // ---- Low-frequency body movement (flame column) ----
    uint8_t base = noise1d(t >> 2);     // slow drift

    // ---- Mid turbulence (heat convection) ----
    uint8_t mid  = noise1d(t >> 1);     // medium variation

    // ---- High shimmer (air instability) ----
    uint8_t high = rand8() & 0x0F;

    // ---- Combine layers (weighted like real flame physics) ----
    int16_t flame =
        (base * 3) +
        (mid * 2) +
        (high * 1);

    flame /= 6;

    // ---- Normalize + clamp ----
    if (flame < 20) flame = 20;
    if (flame > 255) flame = 255;

    // ---- Two slightly spatially offset “flame points” ----
    uint8_t flame1 = flame;
    uint8_t flame2 = flame - (noise1d(t >> 3) >> 3);

    // ---- Ember base glow (slow thermal inertia) ----
    static uint8_t ember = 40;
    ember = smooth(ember, 30 + (base >> 1), 20);

    // ---- Output PWM (gamma corrected) ----
    TIM1->CH1CVR = gamma8[flame1];
    TIM1->CH2CVR = gamma8[flame2];
    TIM1->CH3CVR = gamma8[ember];
}

// ================= timing =================
void delay_ms(void) {
    for (volatile uint32_t i = 0; i < 5000; i++) __NOP();
}

// ================= MAIN =================
int main(void) {
    pwm_init();
    noise_init();

    while (1) {
        candle_update();
        delay_ms();
    }
}
