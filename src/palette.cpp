// /src/palette.cpp

#include "palette.h"
#include "hardware.h"
#include "color_math.h"

// =========================================================
// BASE COLOUR CONSTANTS
// (packColour is bit-identical to strip.Color, but is pure —
//  no static-init ordering dependency on the strip object)
// =========================================================
const uint32_t COL_RED    = packColour(255,   0,   0);   // Red
const uint32_t COL_ORANGE = packColour(255, 120,   0);   // Orange
const uint32_t COL_YELLOW = packColour(255, 255,   0);   // Yellow
const uint32_t COL_GREEN  = packColour(  0, 255,   0);   // Green
const uint32_t COL_CYAN   = packColour(  0, 255, 255);   // Cyan
const uint32_t COL_BLUE   = packColour(  0,   0, 255);   // Blue
const uint32_t COL_PURPLE = packColour(180,   0, 255);   // Purple
const uint32_t COL_WHITE  = packColour(255, 255, 255);   // White

// =========================================================
// MASTER PALETTE (15 COLOURS FROM ORIGINAL SKETCH)
// =========================================================
const uint32_t PALETTE_MASTER[] = {
    packColour(255,   0,   0),   // Red
    packColour(255,  20, 147),   // Deep Pink
    packColour(  0, 255, 255),   // Cyan
    packColour( 57, 255,  20),   // Neon Green
    packColour(255, 255,   0),   // Yellow
    packColour(255,  80,   0),   // Orange
    packColour(  0, 120, 255),   // Sky Blue
    packColour(  0, 255, 140),   // Mint
    packColour(120,   0, 255),   // Purple
    packColour(  0,  60, 255),   // Royal Blue
    packColour(200, 220,  40),   // Lime Yellow
    packColour(255,   0, 110),   // Hot Pink
    packColour(180,   0, 255),   // Violet
    packColour(  0, 255, 200),   // Aqua
    packColour(255, 100,   0)    // Amber
};

const int PALETTE_MASTER_SIZE =
    sizeof(PALETTE_MASTER) / sizeof(PALETTE_MASTER[0]);

// =========================================================
// HUE → COLOUR (0–255)
// =========================================================
uint32_t colourFromHue(uint8_t hue) {
    return strip.ColorHSV((uint16_t)hue * 256, 255, 255);
}

// =========================================================
// DIM COLOUR
// =========================================================
uint32_t dimColour(uint32_t c, float factor) {
    return dimColour32(c, factor);
}

// =========================================================
// RANDOM PALETTE COLOUR
// =========================================================
uint32_t randomPaletteColour() {
    return PALETTE_MASTER[random(PALETTE_MASTER_SIZE)];
}
