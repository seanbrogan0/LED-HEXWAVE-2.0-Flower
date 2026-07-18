// /src/modes/non_audio/mode_ripple.cpp

#include "modes/non_audio/mode_ripple.h"
#include "input.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"
#include <Arduino.h>

// =========================================================
// NON-AUDIO MODE 3: Ripple
// Color pulses outward from center (HEX7) to the outer ring,
// then fades before repeating with the next palette color.
// Left pot = speed.
// =========================================================
static unsigned long lastRipple = 0;
static int rippleState = 0;   // 0 = center lit, 1 = ring lit, 2 = all off
static int rippleColor = 0;

void mode_ripple() {
    float s = modeEngine.leftPot();
    unsigned long interval = (unsigned long)(800.0f - s * 700.0f);  // 100–800 ms per step

    if (millis() - lastRipple >= interval) {
        lastRipple = millis();
        rippleState = (rippleState + 1) % 3;
        if (rippleState == 0) {
            rippleColor = (rippleColor + 1) % PALETTE_MASTER_SIZE;
        }
    }

    uint32_t col = PALETTE_MASTER[rippleColor];
    uint32_t dimCol = dimColour(col, 0.4f);

    switch (rippleState) {
        case 0:
            fillHex(CENTER_HEX, col);
            for (int i = 0; i < 6; i++) clearHex(i);
            break;
        case 1:
            fillHex(CENTER_HEX, dimCol);
            for (int i = 0; i < 6; i++) fillHex(i, col);
            break;
        case 2:
            for (int h = 0; h < NUM_HEXES; h++) clearHex(h);
            break;
    }
}
