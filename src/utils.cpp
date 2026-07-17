// File: src/utils.cpp

#include "utils.h"

// =========================================================
// LED HELPERS
// =========================================================
void setLED(int idx, uint32_t colour) {
    if (idx < 0 || idx >= NUM_LEDS) return;
    strip.setPixelColor(idx, colour);
}

void fillAll(uint32_t colour) {
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, colour);
    }
}

void fillHex(int hexIndex, uint32_t colour) {
    if (hexIndex < 0 || hexIndex >= NUM_HEXES) return;

    const uint16_t* hex = HEXES[hexIndex];
    for (int i = 0; i < HEX_LED_COUNT; i++) {
        setLED(hex[i], colour);
    }
}

void clearHex(int hexIndex) {
    fillHex(hexIndex, 0);
}

void setHexPixel(int hexIndex, int logicalIndex, uint32_t colour) {
    if (hexIndex < 0 || hexIndex >= NUM_HEXES) return;
    if (logicalIndex < 0 || logicalIndex >= HEX_LED_COUNT) return;
    strip.setPixelColor(HEXES[hexIndex][logicalIndex], colour);
}

void rotateHex(int hexIndex, int amount) {
    if (hexIndex < 0 || hexIndex >= NUM_HEXES) return;

    const uint16_t* hex = HEXES[hexIndex];
    uint32_t temp[HEX_LED_COUNT];

    for (int i = 0; i < HEX_LED_COUNT; i++) {
        int src = (i - amount + HEX_LED_COUNT) % HEX_LED_COUNT;
        temp[i] = strip.getPixelColor(hex[src]);
    }
    for (int i = 0; i < HEX_LED_COUNT; i++) {
        strip.setPixelColor(hex[i], temp[i]);
    }
}

void fadeHex(int hexIndex, float factor) {
    if (hexIndex < 0 || hexIndex >= NUM_HEXES) return;

    const uint16_t* hex = HEXES[hexIndex];
    for (int i = 0; i < HEX_LED_COUNT; i++) {
        strip.setPixelColor(hex[i], dimColour32(strip.getPixelColor(hex[i]), factor));
    }
}
