// /src/modes/non_audio/mode_chase.cpp

#include "modes/non_audio/mode_chase.h"
#include "input.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"
#include <Arduino.h>

// =========================================================
// NON-AUDIO MODE 2: Chase
// One hex lights up at a time, stepping around all 7 panels.
// Left pot = speed.
// =========================================================
static unsigned long lastChase = 0;
static int chaseIndex = 0;
static int chaseColor = 0;

void mode_chase() {
    float s = modeEngine.leftPot();
    unsigned long interval = (unsigned long)(600.0f - s * 550.0f);  // 50–600 ms

    if (millis() - lastChase >= interval) {
        lastChase = millis();
        chaseIndex = (chaseIndex + 1) % NUM_HEXES;
        if (chaseIndex == 0) {
            chaseColor = (chaseColor + 1) % PALETTE_MASTER_SIZE;
        }
    }

    for (int h = 0; h < NUM_HEXES; h++) {
        if (h == chaseIndex) {
            fillHex(h, PALETTE_MASTER[chaseColor]);
        } else {
            clearHex(h);
        }
    }
}
