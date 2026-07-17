// /include/palette.h
#pragma once
#include <Arduino.h>

// =========================================================
// BASE PALETTE COLOURS
// =========================================================
extern const uint32_t COL_RED;
extern const uint32_t COL_ORANGE;
extern const uint32_t COL_YELLOW;
extern const uint32_t COL_GREEN;
extern const uint32_t COL_CYAN;
extern const uint32_t COL_BLUE;
extern const uint32_t COL_PURPLE;
extern const uint32_t COL_WHITE;

// =========================================================
// MASTER PALETTE (15 COLOURS FROM ORIGINAL SKETCH)
// =========================================================
extern const uint32_t PALETTE_MASTER[];
extern const int PALETTE_MASTER_SIZE;

// =========================================================
// PALETTE HELPERS
// =========================================================
uint32_t colourFromHue(uint8_t hue);
uint32_t dimColour(uint32_t c, float factor);
uint32_t randomPaletteColour();
