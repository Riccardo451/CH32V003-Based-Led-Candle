#ifndef CANDLE_H
#define CANDLE_H

// ===== DEFAULT CONFIG (MUST BE FIRST) =====
#ifndef CPU_IS_POWERFUL
#define CPU_IS_POWERFUL 0
#endif

#ifndef CANDLE_USE_GAMMA
#define CANDLE_USE_GAMMA 1
#endif

#ifndef CANDLE_USE_COMPRESSION
#define CANDLE_USE_COMPRESSION 1
#endif

#ifndef CANDLE_FLOOR
#define CANDLE_FLOOR 10
#endif

#ifndef CANDLE_UPDATE_INT
#define CANDLE_UPDATE_INT 3
#endif

#ifndef CANDLE_CHANNELS
#define CANDLE_CHANNELS 3
#endif

#include <stdint.h>

// ===============================================

// YOU must implement this in your main sketch
void hw_set_led(uint8_t ch, uint8_t value);

// ================= INTERNAL STATE =================

static uint16_t candle_lfsr = 0xBEEF;
static uint8_t candle_t = 0;
static uint8_t candle_ember = 40;

// smoothed flame states
static uint8_t flame1_s = 0;
static uint8_t flame2_s = 0;

#define CANDLE_NOISE_SIZE 32
static uint8_t candle_noise[CANDLE_NOISE_SIZE];


// ================= GAMMA =================

#include <math.h>

#if CPU_IS_POWERFUL
    // Float calculation for STM32, ESP32 etc 
    static inline uint8_t candle_gamma(uint8_t x) {
        float normalized = x / 255.0f;
        float corrected = powf(normalized, 2.2f);  // gamma 2.2
        return (uint8_t)(corrected * 255.0f + 0.5f);
    }
#else
    // Integer approximation for slow CPU ex Atmega328
    static inline uint8_t candle_gamma(uint8_t x) {
        uint16_t y = x;
        y = (y * y) >> 8;       // approx gamma ~2.0
        y = (y * x) >> 8;       // pushes toward ~2.2
        return (uint8_t)y;
    }
#endif
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

// compression: lifts low values instead of hard clipping
static inline uint8_t candle_lift(uint8_t v) {
    return CANDLE_FLOOR + ((uint16_t)v * (255 - CANDLE_FLOOR) >> 8);
}

// ================= INIT =================

static inline void candle_init(void) {
    for (uint8_t i = 0; i < CANDLE_NOISE_SIZE; i++) {
        candle_noise[i] = candle_rand8();
    }
}

// ================= UPDATE =================

static uint8_t candle_sub = 0;

static inline void candle_update(void) {

    if (++candle_sub >= CANDLE_UPDATE_INT) {   // increase 3 → slower (e.g. 4, 5...)
    candle_sub = 0;
    candle_t++;
}

    // desynchronized noise sampling
    uint8_t base = candle_noise1d((candle_t + 0)  >> 3);
    uint8_t mid  = candle_noise1d((candle_t + 37) >> 2);
    uint8_t var  = candle_noise1d((candle_t + 83) >> 3);

    uint8_t high = candle_rand8() & 0x0F;

    int16_t flame = (base * 3 + mid * 2 + high) / 6;

    if (flame < 10) flame = 10;
    if (flame > 255) flame = 255;

    uint8_t flame1 = flame;
    uint8_t flame2 = flame - (var >> 2);

    // occasional random flare
    if ((candle_rand8() & 0x1F) == 0) {
        flame1 += 20;
        if (flame1 > 255) flame1 = 255;
    }

    // smooth flames
    flame1_s = candle_smooth(flame1_s, flame1, 8);
    flame2_s = candle_smooth(flame2_s, flame2, 10);

    // ember (slow glow)
    candle_ember = candle_smooth(candle_ember, 30 + (base >> 1), 20);


// --- compression stage ---
#if CANDLE_USE_COMPRESSION
    uint8_t out1 = candle_lift(flame1_s);
    uint8_t out2 = candle_lift(flame2_s);
    uint8_t out3 = candle_lift(candle_ember);
#else
    uint8_t out1 = flame1_s;
    uint8_t out2 = flame2_s;
    uint8_t out3 = candle_ember;
#endif

// --- gamma stage ---
#if CANDLE_USE_GAMMA
    out1 = candle_gamma(out1);
    out2 = candle_gamma(out2);
    out3 = candle_gamma(out3);
#endif

// --- final output ---
hw_set_led(0, out1);
hw_set_led(1, out2);
hw_set_led(2, out3);
}

#endif
