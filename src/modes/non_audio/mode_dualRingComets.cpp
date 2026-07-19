// /src/modes/non_audio/mode_dualRingComets.cpp

#include "modes/non_audio/mode_dualRingComets.h"
#include "input.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"
#include "path_math.h"
#include "hardware.h"
#include <Arduino.h>
#include <math.h>

// =========================================================
// DUAL RING COMETS MODE
// Two comets orbit the outer ring (HEX_PATH_RING,
// 1 → 6 → 5 → 4 → 3 → 2, a true closed cycle — the wrap is
// physically adjacent, so the orbit never teleports) with:
// - 15% header glow
// - exponential tail (k = 0.04)
// - Only first 10 LEDs freshly drawn (prevents colour pop)
// - Original 0.97 global fade
// The centre hex glows a steady dim blend of the two comet
// colours, refreshing whenever a comet wraps to a new colour.
// Left pot = speed. Right pot = brightness.
// (Ported from the Linear fixture's Dual Comets.)
// =========================================================

static const int TOTAL_PATH = PATH_LEN_RING * HEX_LED_COUNT;

// Convert path position → LED index (path data lives in hex_geometry)
static int getPathLED(int pathPos) {
    return pathPosToLED(pathPos, HEX_PATH_RING, PATH_LEN_RING,
                        HEXES, HEX_LED_COUNT);
}

// Comet positions + colours
static int pos1 = 0;
static int pos2 = TOTAL_PATH / 2;

static uint32_t col1 = 0;
static uint32_t col2 = 0;

static bool firstRun = true;

// =========================================================
// APPLY GLOW FOR A SINGLE COMET
// =========================================================
static void drawCometGlow(int headPos, uint32_t colour) {
    // Header glow (in front of head)
    int headerPos = wrapPathPos(headPos + 1, TOTAL_PATH);
    strip.setPixelColor(getPathLED(headerPos), dimColour(colour, 0.15f));

    // Head
    strip.setPixelColor(getPathLED(headPos), colour);

    // Exponential tail: only the first 10 LEDs are freshly drawn;
    // older tail LEDs fade out via the global 0.97 fade.
    const float k = 0.04f;

    for (int t = 1; t <= 10; t++) {
        float level = expf(-k * t);
        int tailPos = wrapPathPos(headPos - t, TOTAL_PATH);
        strip.setPixelColor(getPathLED(tailPos), dimColour(colour, level));
    }
}

// =========================================================
// DUAL RING COMETS MODE
// =========================================================
void mode_dualRingComets() {

    if (firstRun) {
        col1 = randomPaletteColour();
        col2 = randomPaletteColour();
        firstRun = false;
    }

    // Left pot = speed (same 40 → 5 ms curve as the Linear original)
    int speed = (int)(40.0f - modeEngine.leftPot() * 35.0f);

    static unsigned long lastMove = 0;
    unsigned long now = millis();

    if (now - lastMove >= (unsigned long)speed) {
        lastMove = now;

        // Global fade (original 0.97 tail fade)
        for (int i = 0; i < NUM_LEDS; i++) {
            strip.setPixelColor(i, dimColour(strip.getPixelColor(i), 0.97f));
        }

        drawCometGlow(pos1, col1);
        pos1++;
        if (pos1 >= TOTAL_PATH) {
            pos1 = 0;
            col1 = randomPaletteColour();   // new colour on wrap
        }

        drawCometGlow(pos2, col2);
        pos2++;
        if (pos2 >= TOTAL_PATH) {
            pos2 = 0;
            col2 = randomPaletteColour();   // new colour on wrap
        }
    }

    // Centre hex: steady dim blend of the two current comet colours
    fillHex(CENTER_HEX, dimColour(lerpColour32(col1, col2, 0.5f), 0.10f));
}
