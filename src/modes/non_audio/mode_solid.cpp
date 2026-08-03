// /src/modes/non_audio/mode_solid.cpp

#include "modes/non_audio/mode_solid.h"
#include "input.h"
#include "utils.h"
#include "hex_geometry.h"
#include "hardware.h"
#include <Arduino.h>

// =========================================================
// HUE → RGB CONVERSION (0–340 degrees)
// =========================================================
static uint32_t hueToRGB(int hueDeg) {
    float h = hueDeg / 60.0f;
    int i = (int)h;
    float f = h - i;
    float q = 1 - f;

    uint8_t r = 0, g = 0, b = 0;

    switch (i) {
        case 0: r = 255; g = f * 255; b = 0; break;
        case 1: r = q * 255; g = 255; b = 0; break;
        case 2: r = 0; g = 255; b = f * 255; break;
        case 3: r = 0; g = q * 255; b = 255; break;
        case 4: r = f * 255; g = 0; b = 255; break;
        default: r = 255; g = 0; b = q * 255; break;
    }

    return strip.Color(r, g, b);
}

// =========================================================
// SOLID COLOUR MODE
// Left pot sweeps hue 0–340°, then desaturates to white at
// the top of its travel (same curve as the raw-pot original,
// where the white zone began at 3500/4095).
// (Ported verbatim from the Linear fixture's Solid.)
// =========================================================
void mode_solid() {
    const float WHITE_POINT = 3500.0f / 4095.0f;
    float p = modeEngine.leftPot();
    uint32_t color;

    if (p < WHITE_POINT) {
        // Hue sweep 0 → 340 degrees
        int hue = (int)(p / WHITE_POINT * 340.0f);
        color = hueToRGB(hue);
    } else {
        // Fade saturation → white
        int sat = (int)(255.0f - (p - WHITE_POINT) / (1.0f - WHITE_POINT) * 255.0f);

        uint8_t r = 255;
        uint8_t g = 0;
        uint8_t b = 0;

        // Desaturate toward white
        r = r + ((255 - r) * (255 - sat) / 255);
        g = g + ((255 - g) * (255 - sat) / 255);
        b = b + ((255 - b) * (255 - sat) / 255);

        color = strip.Color(r, g, b);
    }

    fillAll(color);
}
