// /src/modes/non_audio/mode_rotate.cpp

#include "modes/non_audio/mode_rotate.h"
#include "input.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"
#include <Arduino.h>

// =========================================================
// NON-AUDIO MODE 0: Rotate
// 6 colors cycle around HEX7's sides and connected hexes.
// Left pot = rotation speed.
// =========================================================
static unsigned long lastRotate = 0;
static int rotateIndex = 0;

void mode_rotate() {
    float s = modeEngine.leftPot();
    unsigned long interval = (unsigned long)(2500.0f - s * 2300.0f);

    if (millis() - lastRotate >= interval) {
        lastRotate = millis();
        rotateIndex = (rotateIndex + 5) % 6;  // +5 mod 6 == -1 (anti-clockwise)
    }

    for (int side = 0; side < 6; side++) {
        int colIndex = (side + rotateIndex) % 6;
        uint32_t col = PALETTE_MASTER[colIndex];

        // Paint this side of the center hex (4 LEDs per side)...
        int start = side * 4;
        for (int i = 0; i < 4; i++) {
            setHexPixel(CENTER_HEX, start + i, col);
        }

        // ...and mirror onto the physically adjacent outer hex
        fillHex(HEX7_SIDE_TO_HEX[side] - 1, col);
    }
}
