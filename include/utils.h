// File: include/utils.h

#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "hardware.h"
#include "hex_geometry.h"
#include "color_math.h"

// =========================================================
// LED HELPERS
// (generic int/float/colour math lives in hexcore/color_math.h)
// =========================================================

// Set LED colour safely (with bounds check)
void setLED(int idx, uint32_t colour);

// Fill entire strip
void fillAll(uint32_t colour);

// Fill a hex by index (0-based)
void fillHex(int hexIndex, uint32_t colour);

// Clear a hex by index
void clearHex(int hexIndex);

// Set one LED inside a hex by its logical position (0..HEX_LED_COUNT-1)
void setHexPixel(int hexIndex, int logicalIndex, uint32_t colour);

// Rotate a hex's pixels by `amount` logical positions
void rotateHex(int hexIndex, int amount);

// Multiply a hex's pixels by factor (0.0–1.0)
void fadeHex(int hexIndex, float factor);
