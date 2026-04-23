#include "candle.h"

// ================= Gamma =================
static const uint8_t gamma8[256] = {
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

static uint8_t rand8(void) {
    lfsr ^= lfsr << 7;
    lfsr ^= lfsr >> 9;
    lfsr ^= lfsr << 8;
    return (uint8_t)lfsr;
}

// ================= Noise =================
#define NOISE_SIZE 32
static uint8_t noise[NOISE_SIZE];

static uint8_t lerp(uint8_t a, uint8_t b, uint8_t t) {
    return a + ((b - a) * t >> 8);
}

static uint8_t noise1d(uint8_t x) {
    uint8_t i = x & (NOISE_SIZE - 1);
    uint8_t j = (i + 1) & (NOISE_SIZE - 1);
    uint8_t t = x << 3;
    return lerp(noise[i], noise[j], t);
}

// ================= State =================
static uint8_t t = 0;
static uint8_t ember = 40;

static uint8_t smooth(uint8_t v, uint8_t target, uint8_t k) {
    return v + ((int16_t)target - v) / k;
}

// ================= Init =================
void candle_init(void) {
    for (int i = 0; i < NOISE_SIZE; i++) {
        noise[i] = rand8();
    }
}

// ================= Update =================
void candle_update(void) {

    t++;

    uint8_t base = noise1d(t >> 2);
    uint8_t mid  = noise1d(t >> 1);
    uint8_t high = rand8() & 0x0F;

    int16_t flame = (base * 3 + mid * 2 + high) / 6;

    if (flame < 20) flame = 20;
    if (flame > 255) flame = 255;

    uint8_t flame1 = flame;
    uint8_t flame2 = flame - (noise1d(t >> 3) >> 3);

    ember = smooth(ember, 30 + (base >> 1), 20);

    hw_set_led(0, gamma8[flame1]);
    hw_set_led(1, gamma8[flame2]);
    hw_set_led(2, gamma8[ember]);
}
