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
// colors and fade out over a few hundred ms. Left pot =
// spark density.
//
// Maintains its own pixel buffer so the loop's per-frame
// strip.clear() (autoClear) does not erase the fade trail.
// =========================================================
static uint32_t sparkleBuf[NUM_LEDS];

// Hardware-tunable: per-frame fade at the 10 ms pace (~200 ms tail)
static const float SPARKLE_FADE = 0.95f;
// Hardware-tunable: spark spawn rate range in sparks/second
static const float SPARK_RATE_MIN = 10.0f;
static const float SPARK_RATE_MAX = 200.0f;

static float sparkAcc = 0.0f;

void mode_sparkle() {
    // Fade all existing sparks
    for (int i = 0; i < NUM_LEDS; i++) {
        uint32_t c = sparkleBuf[i];
        if (c) {
            sparkleBuf[i] = dimColour(c, SPARKLE_FADE);
        }
    }

    // Spawn new sparks at a pot-scaled rate (accumulator keeps the
    // fractional remainder so low rates still fire)
    float rate = SPARK_RATE_MIN
               + modeEngine.leftPot() * (SPARK_RATE_MAX - SPARK_RATE_MIN);
    sparkAcc += rate * (FRAME_INTERVAL_MS / 1000.0f);
    while (sparkAcc >= 1.0f) {
        sparkAcc -= 1.0f;
        sparkleBuf[random(NUM_LEDS)] = PALETTE_MASTER[random(PALETTE_MASTER_SIZE)];
    }

    // Write buffer to strip (overrides the loop's clear)
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, sparkleBuf[i]);
    }
}
