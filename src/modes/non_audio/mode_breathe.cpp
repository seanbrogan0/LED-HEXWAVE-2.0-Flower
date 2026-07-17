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
// All hexes pulse a single palette color in and out.
// Left pot = color selection.
// =========================================================
void mode_breathe() {
    float t = millis() / 1000.0f;
    float breath = (sinf(t * 1.5f) + 1.0f) * 0.5f;  // 0.0–1.0 sine, ~4 s period

    int colIndex = (int)(modeEngine.leftPot() * (PALETTE_MASTER_SIZE - 1));
    uint32_t col = dimColour(PALETTE_MASTER[colIndex], breath);

    for (int h = 0; h < NUM_HEXES; h++) {
        fillHex(h, col);
    }
}
