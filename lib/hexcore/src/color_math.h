// hexcore/color_math.h
// =========================================================
// Pure colour + integer math. No Arduino dependency —
// everything here is natively unit-testable.
// Colours are packed 0x00RRGGBB, matching Adafruit_NeoPixel::Color.
// =========================================================

#pragma once
#include <stdint.h>

// Pack r/g/b into 0x00RRGGBB (bit-identical to Adafruit_NeoPixel::Color)
uint32_t packColour(uint8_t r, uint8_t g, uint8_t b);

// Scale a packed colour by factor (0.0–1.0), truncating each channel
uint32_t dimColour32(uint32_t c, float factor);

// Linear blend a → b, t clamped to 0.0–1.0
uint32_t lerpColour32(uint32_t a, uint32_t b, float t);

// Generic helpers
int clampInt(int v, int lo, int hi);
float clampFloat(float v, float lo, float hi);

// Wrap index into [0, max): -1 → max-1, max → 0
int wrapIndex(int i, int max);
