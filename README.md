# 🔥 CH32V003 Cinematic Candle (Perlin-Style Flame Simulation)

A highly realistic candle flicker simulation for the CH32V003 microcontroller using **coherent (Perlin-style) noise**, multi-layer turbulence, and gamma-corrected PWM.

This project goes beyond simple LED flickering and aims to reproduce the *organic motion of real flame* using lightweight mathematical models that fit on a tiny MCU.

---

## ✨ Features

* 🌊 **Perlin-style coherent noise** (smooth, natural motion)
* 🔥 **Multi-layer flame simulation**

  * Low-frequency drift (flame body)
  * Mid-frequency turbulence (convection)
  * High-frequency shimmer (air instability)
* 🎚️ **Gamma correction** for perceptually linear brightness
* 🔴 **Ember glow channel** (thermal inertia simulation)
* ⚡ **Efficient & lightweight** (runs on low-cost RISC-V MCU)
* 🎬 Designed for **cinematic realism**, not just blinking LEDs

---

## 🧠 How It Works

Real flames are not random—they exhibit **correlated motion over time** due to fluid dynamics and heat convection.

This project approximates that behavior using:

### 1. Coherent Noise (Perlin-style)

Instead of random values, brightness is derived from **smoothly interpolated noise**, producing continuous motion.

### 2. Multi-Frequency Layers

The flame signal is composed of:

* **Base drift** → slow movement of the flame body
* **Mid turbulence** → chaotic convection patterns
* **High shimmer** → subtle fast flickering

### 3. Thermal Inertia (Ember Channel)

A third LED simulates heat buildup and decay, creating a stable visual “anchor” like a real wick glow.

### 4. Gamma Correction

LED brightness is corrected to match human perception, eliminating harsh stepping and improving realism.

---

## 🔌 Hardware Setup

Target MCU: CH32V003

### Recommended LED Configuration

| Pin | Function   | LED Type     |
| --- | ---------- | ------------ |
| PD2 | Flame Core | Warm white   |
| PD3 | Flame Edge | Amber / warm |
| PD4 | Ember Base | Red / orange |

### Wiring

Each LED:

```
MCU Pin → Resistor (220–470Ω) → LED → GND
```

---

## ⚙️ Build & Flash

You can compile using:

* MounRiver Studio
* PlatformIO (with CH32V support)
* WCH toolchain

Steps (generic):

1. Clone the repo
2. Open project in your IDE
3. Build
4. Flash via WCH-Link or compatible programmer

---

## 🎛️ Tuning the Flame

You can easily customize the look of the flame:

### More aggressive flicker

```c
uint8_t high = rand8() & 0x1F;
```

### Slower, calmer flame

```c
uint8_t base = noise1d(t >> 3);
```

### Brighter flame

Increase minimum clamp:

```c
if (flame < 40) flame = 40;
```

### Stronger ember glow

```c
ember = smooth(ember, 50 + (base >> 1), 20);
```

---

## 👁️ Visual Tips (Huge Impact)

To get the best realism:

* Use **diffusion material** (paper, wax, frosted plastic)
* Slightly **separate LEDs physically**
* Mix colors:

  * Warm white + amber + red
* Avoid direct LED visibility → simulate a flame volume

---

## 📊 Why This Looks Real

| Technique            | Effect                     |
| -------------------- | -------------------------- |
| Coherent noise       | Removes digital randomness |
| Multi-layer blending | Mimics real flame physics  |
| Gamma correction     | Matches human vision       |
| Ember channel        | Adds subconscious realism  |

---

## 🚀 Possible Extensions

* ⏱️ Timer interrupt (“zero jitter”) engine
* 🔥 Multi-candle simulation (shared turbulence field)
* 🎵 Audio-reactive flame
* 🧠 2D flame grid (mini fluid simulation)
* 🎨 Color temperature shifting over time

---

## 🙌 Credits / Inspiration

Inspired by:

* Procedural noise techniques used in graphics
* Real-world flame behavior and combustion physics
* Embedded LED art and physical computing

---

## ⚠️ Disclaimer

This project simulates flame visually only.
It does **not** produce heat or fire.

---

## ⭐ If you like it

Consider starring the repo or extending it—there’s a lot of room to push this even further into physical simulation territory.



## Follow up ideas:
* 2D flame column simulation (tiny grid)
* heat diffusion model (Navier-Stokes-lite approximation)
* camera-like response curve (film emulation)
* multiple interacting candles (shared air field)

That’s where it stops being an LED effect and starts being a procedural fire simulator on a microcontroller.
