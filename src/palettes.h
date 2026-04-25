// /src/palettes.h
// =========================================================
// Color palettes for LED modes
// NOTES:
//   - Uses Adafruit_NeoPixel::Color() for safe static init
//   - All colors stored as packed uint32_t
// =========================================================

#pragma once
#include <Adafruit_NeoPixel.h>

// MASTER PALETTE (15 COLORS)
static const uint32_t PALETTE_MASTER[] = {
    Adafruit_NeoPixel::Color(255,   0,   0),   // Red
    Adafruit_NeoPixel::Color(255,  20, 147),   // Deep Pink
    Adafruit_NeoPixel::Color(  0, 255, 255),   // Cyan
    Adafruit_NeoPixel::Color( 57, 255,  20),   // Neon Green
    Adafruit_NeoPixel::Color(255, 255,   0),   // Yellow
    Adafruit_NeoPixel::Color(255,  80,   0),   // Orange
    Adafruit_NeoPixel::Color(  0, 120, 255),   // Sky Blue
    Adafruit_NeoPixel::Color(  0, 255, 140),   // Mint
    Adafruit_NeoPixel::Color(120,   0, 255),   // Purple
    Adafruit_NeoPixel::Color(  0,  60, 255),   // Royal Blue
    Adafruit_NeoPixel::Color(200, 220,  40),   // Lime Yellow
    Adafruit_NeoPixel::Color(255,   0, 110),   // Hot Pink
    Adafruit_NeoPixel::Color(180,   0, 255),   // Violet
    Adafruit_NeoPixel::Color(  0, 255, 200),   // Aqua
    Adafruit_NeoPixel::Color(255, 100,   0)    // Amber
};

static const int PALETTE_MASTER_SIZE = sizeof(PALETTE_MASTER) / sizeof(PALETTE_MASTER[0]);
