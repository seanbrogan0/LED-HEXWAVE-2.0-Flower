// /src/modes/audio/mode_bassTreble.cpp

#include "modes/audio/mode_bassTreble.h"
#include "audio_engine.h"
#include "input.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"
#include <Arduino.h>

// =========================================================
// BASS VS TREBLE MODE (dual band — mid is ignored)
// Lows at the core, highs radiating outside: bass drives the
// centre hex, treble drives the outer ring.
// Left pot = bass hue; treble gets the complementary hue.
// =========================================================
void mode_bassTreble() {
    float b = (float)audioBassNorm();
    float t = (float)audioTrebNorm();

    uint8_t bassHue = (uint8_t)(modeEngine.leftPot() * 255.0f);
    uint32_t bassCol = colourFromHue(bassHue);
    uint32_t trebCol = colourFromHue((uint8_t)(bassHue + 128));  // complement

    fillHex(CENTER_HEX, dimColour(bassCol, b));
    for (int i = 0; i < 6; i++) {
        fillHex(i, dimColour(trebCol, t));
    }
}
