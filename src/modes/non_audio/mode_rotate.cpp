// /src/modes/non_audio/mode_rotate.cpp

#include "modes/non_audio/mode_rotate.h"
#include "input.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"
#include <Arduino.h>
#include <math.h>

// =========================================================
// NON-AUDIO MODE 0: Rotate
// 6 colors march anti-clockwise around HEX7's sides and the
// connected outer hexes, crossfading continuously between
// neighbouring palette colours instead of snapping.
// Left pot = rotation speed.
// =========================================================
static float rotPos = 0.0f;         // continuous rotation offset (1.0 = one side)
static unsigned long lastMs = 0;

void mode_rotate() {
    float s = modeEngine.leftPot();
    float interval = 2500.0f - s * 2300.0f;   // ms per colour step, as before

    unsigned long now = millis();
    if (lastMs == 0) lastMs = now;
    rotPos -= (now - lastMs) / interval;      // anti-clockwise, one step per interval
    lastMs = now;
    rotPos = fmodf(rotPos, 6.0f);
    if (rotPos < 0.0f) rotPos += 6.0f;

    for (int side = 0; side < 6; side++) {
        float u = fmodf(side + rotPos, 6.0f);
        int i0 = (int)u;
        uint32_t col = lerpColour32(PALETTE_MASTER[i0],
                                    PALETTE_MASTER[(i0 + 1) % 6],
                                    u - i0);

        // Paint this side of the center hex (4 LEDs per side)...
        int start = side * 4;
        for (int i = 0; i < 4; i++) {
            setHexPixel(CENTER_HEX, start + i, col);
        }

        // ...and mirror onto the physically adjacent outer hex
        fillHex(HEX7_SIDE_TO_HEX[side] - 1, col);
    }
}
