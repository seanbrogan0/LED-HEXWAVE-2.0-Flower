// /src/modes/non_audio/mode_ripple.cpp

#include "modes/non_audio/mode_ripple.h"
#include "input.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"
#include <Arduino.h>
#include <math.h>

// =========================================================
// NON-AUDIO MODE 3: Ripple
// A colour pulse expands smoothly from the centre (HEX7) out
// to the ring and fades away — continuous brightness ramps
// instead of the old three on/off states. The next palette
// colour is selected while everything is dark.
// Left pot = speed.
// =========================================================
static float ripplePhase = 0.0f;    // 0..3: centre rise / handoff to ring / fade out
static unsigned long lastMs = 0;
static int rippleColor = 0;

void mode_ripple() {
    float s = modeEngine.leftPot();
    float interval = 800.0f - s * 700.0f;   // ms per stage, 800 -> 100 (as before)

    unsigned long now = millis();
    if (lastMs == 0) lastMs = now;
    ripplePhase += (now - lastMs) / interval;
    lastMs = now;
    if (ripplePhase >= 3.0f) {
        ripplePhase = fmodf(ripplePhase, 3.0f);
        rippleColor = (rippleColor + 1) % PALETTE_MASTER_SIZE;   // change while dark
    }

    float p = ripplePhase;
    float centreB, ringB;
    if (p < 1.0f) {           // pulse rises in the centre
        centreB = p;
        ringB   = 0.0f;
    } else if (p < 2.0f) {    // pulse hands off to the ring
        centreB = 1.0f - 0.6f * (p - 1.0f);   // 1 -> 0.4
        ringB   = p - 1.0f;                   // 0 -> 1
    } else {                  // everything fades out
        centreB = 0.4f * (3.0f - p);          // 0.4 -> 0
        ringB   = 3.0f - p;                   // 1 -> 0
    }

    uint32_t col = PALETTE_MASTER[rippleColor];
    fillHex(CENTER_HEX, dimColour(col, centreB));

    uint32_t ringCol = dimColour(col, ringB);
    for (int i = 0; i < 6; i++) {
        fillHex(i, ringCol);
    }
}
