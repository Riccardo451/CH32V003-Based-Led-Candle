#ifndef CANDLE_H
#define CANDLE_H

#include <stdint.h>

// ================= USER CONFIG =================

#ifndef CANDLE_CHANNELS
#define CANDLE_CHANNELS 3
#endif

// YOU must implement this in your main sketch
void hw_set_led(uint8_t ch, uint8_t value);

// ================= INTERNAL STATE =================

static uint16_t candle_lfsr = 0xBEEF;
static uint8_t candle_t = 0;
static uint8_t candle_ember = 40;

// smoothed flame states (NEW)
static uint8_t flame1_s = 0;
static uint8_t flame2_s = 0;

#define CANDLE_NOISE_SIZE 32
static uint8_t candle_noise[CANDLE_NOISE_SIZE];

// ================= GAMMA =================
static const uint8_t candle_gamma8[256] = {
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
static inline uint8_t candle_rand8(void) {
    candle_lfsr ^= candle_lfsr << 7;
    candle_lfsr ^= candle_lfsr >> 9;
    candle_lfsr ^= candle_lfsr << 8;
    return (uint8_t)candle_lfsr;
}

// ================= NOISE =================
static inline uint8_t candle_lerp(uint8_t a, uint8_t b, uint8_t t) {
    return a + ((b - a) * t >> 8);
}

static inline uint8_t candle_noise1d(uint8_t x) {
    uint8_t i = x & (CANDLE_NOISE_SIZE - 1);
    uint8_t j = (i + 1) & (CANDLE_NOISE_SIZE - 1);
    uint8_t t = x << 3;
    return candle_lerp(candle_noise[i], candle_noise[j], t);
}

// ================= HELPERS =================
static inline uint8_t candle_smooth(uint8_t v, uint8_t target, uint8_t k) {
    return v + ((int16_t)target - v) / k;
}

// ================= INIT =================
static inline void candle_init(void) {
    for (uint8_t i = 0; i < CANDLE_NOISE_SIZE; i++) {
        candle_noise[i] = candle_rand8();
    }
}

// ================= UPDATE =================
static inline void candle_update(void) {

    candle_t++;

    // desynchronized noise sampling
    uint8_t base = candle_noise1d((candle_t + 0)  >> 3);
    uint8_t mid  = candle_noise1d((candle_t + 37) >> 2);
    uint8_t var  = candle_noise1d((candle_t + 83) >> 3);

    uint8_t high = candle_rand8() & 0x0F;

    int16_t flame = (base * 3 + mid * 2 + high) / 6;

    if (flame < 20) flame = 20;
    if (flame > 255) flame = 255;

    uint8_t flame1 = flame;
    uint8_t flame2 = flame - (var >> 2);

    // occasional random flare
    if ((candle_rand8() & 0x1F) == 0) {
        flame1 += 20;
        if (flame1 > 255) flame1 = 255;
    }

    // smooth flames (reduces jitter)
    flame1_s = candle_smooth(flame1_s, flame1, 6);
    flame2_s = candle_smooth(flame2_s, flame2, 8);

    // ember (slow glow)
    candle_ember = candle_smooth(candle_ember, 30 + (base >> 1), 20);

    // output
    hw_set_led(0, candle_gamma8[flame1_s]);
    hw_set_led(1, candle_gamma8[flame2_s]);
    hw_set_led(2, candle_gamma8[candle_ember]);
}

#endif
