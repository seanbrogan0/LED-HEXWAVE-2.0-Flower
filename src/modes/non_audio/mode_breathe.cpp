// /src/modes/non_audio/mode_breathe.cpp

#include "modes/non_audio/mode_breathe.h"
#include "input.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"
#include <Arduino.h>
#include <math.h>

// =========================================================
// NON-AUDIO MODE 1: Breathe
// All hexes pulse in and out on a smooth sine; each breath
// starts a new palette colour, swapped at the dark point so
// the change is invisible. Left pot = breath speed.
// =========================================================
static const float TWO_PI_F = 6.2831853f;

static float phase = 0.0f;          // 0 = dark, PI = full, 2*PI = dark again
static unsigned long lastMs = 0;
static int colIdx = 0;

void mode_breathe() {
    float s = modeEngine.leftPot();
    float period = 8.0f - s * 6.5f;   // seconds per breath, 8 s slow -> 1.5 s fast

    unsigned long now = millis();
    if (lastMs == 0) lastMs = now;
    phase += (now - lastMs) / 1000.0f * (TWO_PI_F / period);
    lastMs = now;

    if (phase >= TWO_PI_F) {
        phase = fmodf(phase, TWO_PI_F);
        colIdx = (colIdx + 1) % PALETTE_MASTER_SIZE;   // new colour while dark
    }

    float breath = (1.0f - cosf(phase)) * 0.5f;   // 0 -> 1 -> 0 over one breath
    uint32_t col = dimColour(PALETTE_MASTER[colIdx], breath);

    for (int h = 0; h < NUM_HEXES; h++) {
        fillHex(h, col);
    }
}
