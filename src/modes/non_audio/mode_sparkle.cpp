// /src/modes/non_audio/mode_sparkle.cpp

#include "modes/non_audio/mode_sparkle.h"
#include "input.h"
#include "utils.h"
#include "palette.h"
#include "hardware.h"
#include <Arduino.h>

// =========================================================
// NON-AUDIO MODE 4: Sparkle
// Random LEDs across all panels light up in random palette
// colors and fade each frame. Left pot = spark density.
//
// Maintains its own pixel buffer so the loop's per-frame
// strip.clear() (autoClear) does not erase the fade trail.
// =========================================================
static uint32_t sparkleBuf[NUM_LEDS];

void mode_sparkle() {
    // Fade all existing sparks
    for (int i = 0; i < NUM_LEDS; i++) {
        uint32_t c = sparkleBuf[i];
        if (c) {
            sparkleBuf[i] = dimColour(c, 0.80f);
        }
    }

    // Add new sparks scaled by left pot (1–20 per frame)
    int density = (int)(modeEngine.leftPot() * 19.0f) + 1;
    for (int i = 0; i < density; i++) {
        int led = random(NUM_LEDS);
        sparkleBuf[led] = PALETTE_MASTER[random(PALETTE_MASTER_SIZE)];
    }

    // Write buffer to strip (overrides the loop's clear)
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, sparkleBuf[i]);
    }
}
