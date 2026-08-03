// /src/modes/non_audio/mode_hexFadeRandom.cpp

#include "modes/non_audio/mode_hexFadeRandom.h"
#include "input.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"
#include "hardware.h"
#include <Arduino.h>

// =========================================================
// HEX FADE RANDOM MODE
// Picks ONE hex, fades from its current colour to a new
// random palette colour. Other hexes keep their colours.
// Left pot = fade speed. Right pot = brightness.
// (Colour blending uses hexcore's lerpColour32.)
// (Ported from the Linear fixture; repaints every hex each
// frame, so it works under this fixture's autoClear.)
// =========================================================

// Per-hex current colours
static uint32_t hexColours[NUM_HEXES];

// Active fade state
static int currentHex = -1;
static uint32_t startColor = 0;
static uint32_t targetColor = 0;
static float blend = 0.0f;
static bool firstRun = true;

// =========================================================
// SELECT NEXT HEX (PRIORITISE BLACK HEXES)
// =========================================================
static int pickNextHex() {
    // First collect all black hexes
    int blackList[NUM_HEXES];
    int blackCount = 0;

    for (int i = 0; i < NUM_HEXES; i++) {
        if (hexColours[i] == 0) {
            blackList[blackCount++] = i;
        }
    }

    // If any black hexes exist → choose one of them
    if (blackCount > 0) {
        return blackList[random(blackCount)];
    }

    // Otherwise choose any hex
    return random(0, NUM_HEXES);
}

// =========================================================
// HEX FADE RANDOM MODE
// =========================================================
void mode_hexFadeRandom() {

    // ---------------------------------------------------------
    // INITIALISE PER-HEX COLOURS ON FIRST RUN
    // ---------------------------------------------------------
    if (firstRun) {
        for (int i = 0; i < NUM_HEXES; i++) {
            hexColours[i] = 0;  // start from black
        }
        currentHex = pickNextHex();
        startColor = hexColours[currentHex];
        targetColor = randomPaletteColour();
        blend = 0.0f;
        firstRun = false;
    }

    // ---------------------------------------------------------
    // LEFT POT = FADE SPEED (same 1 → 20 curve as the raw-pot map)
    // ---------------------------------------------------------
    float fadeSpeed = (1.0f + modeEngine.leftPot() * 19.0f) / 500.0f;

    // ---------------------------------------------------------
    // ADVANCE BLEND
    // ---------------------------------------------------------
    blend += fadeSpeed;

    if (blend > 1.0f) blend = 1.0f;

    // Update the active hex colour
    hexColours[currentHex] = lerpColour32(startColor, targetColor, blend);

    // ---------------------------------------------------------
    // WHEN FADE COMPLETES → PICK NEW HEX + NEW TARGET COLOUR
    // ---------------------------------------------------------
    if (blend >= 1.0f) {
        // Start next fade from the colour we just reached
        startColor = hexColours[currentHex];

        // Pick next hex (prioritise black ones)
        currentHex = pickNextHex();

        // New target colour
        targetColor = randomPaletteColour();

        blend = 0.0f;
    }

    // ---------------------------------------------------------
    // DRAW ALL HEXES FROM THEIR STORED COLOURS
    // ---------------------------------------------------------
    for (int h = 0; h < NUM_HEXES; h++) {
        fillHex(h, hexColours[h]);
    }
}
