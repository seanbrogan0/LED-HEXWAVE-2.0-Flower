// /src/modes/non_audio/mode_chase.cpp

#include "modes/non_audio/mode_chase.h"
#include "input.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"
#include <Arduino.h>
#include <math.h>

// =========================================================
// NON-AUDIO MODE 2: Chase
// A soft pulse orbits the OUTER ring only (HEX_PATH_RING,
// physical rotational order), crossfading between cells
// instead of snapping. The centre hex stays dark except for
// the wall touching the lit cell, which glows in sync
// (mapping via HEX7_SIDE_TO_HEX). Colour blends into the
// next palette entry over the course of each lap.
// Left pot = speed.
// =========================================================
static float chasePos = 0.0f;       // continuous position along the ring path
static unsigned long lastMs = 0;
static int chaseColor = 0;

void mode_chase() {
    float s = modeEngine.leftPot();
    float interval = 600.0f - s * 550.0f;   // ms per hex, 600 -> 50 (as before)

    unsigned long now = millis();
    if (lastMs == 0) lastMs = now;
    chasePos += (now - lastMs) / interval;
    lastMs = now;
    if (chasePos >= PATH_LEN_RING) {
        chasePos = fmodf(chasePos, (float)PATH_LEN_RING);
        chaseColor = (chaseColor + 1) % PALETTE_MASTER_SIZE;
    }

    // Crossfade toward the next palette colour over the lap so the
    // colour advance at the wrap never pops
    uint32_t col = lerpColour32(
        PALETTE_MASTER[chaseColor],
        PALETTE_MASTER[(chaseColor + 1) % PALETTE_MASTER_SIZE],
        chasePos / PATH_LEN_RING);

    // Outer ring: asymmetric pulse — short ramp ahead of the head,
    // long fading tail behind it
    const float LEAD_LEN = 1.0f;
    const float TAIL_LEN = 2.5f;

    float hexBright[6] = { 0 };
    for (int k = 0; k < PATH_LEN_RING; k++) {
        // distance the pulse has travelled past this cell, wrapped to [0, 6)
        float behind = fmodf(chasePos - k + PATH_LEN_RING, (float)PATH_LEN_RING);
        float ahead  = PATH_LEN_RING - behind;

        float b = 0.0f;
        if (behind < TAIL_LEN) b = 1.0f - behind / TAIL_LEN;
        if (ahead < LEAD_LEN) {
            float lead = 1.0f - ahead / LEAD_LEN;
            if (lead > b) b = lead;
        }

        int hex = HEX_PATH_RING[k] - 1;
        hexBright[hex] = b;
        fillHex(hex, dimColour(col, b));
    }

    // Centre hex: each wall mirrors its adjacent outer cell's level
    for (int side = 0; side < 6; side++) {
        uint32_t c = dimColour(col, hexBright[HEX7_SIDE_TO_HEX[side] - 1]);
        int start = side * 4;
        for (int i = 0; i < 4; i++) {
            setHexPixel(CENTER_HEX, start + i, c);
        }
    }
}
