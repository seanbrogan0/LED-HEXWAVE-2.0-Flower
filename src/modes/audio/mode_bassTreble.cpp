// /src/modes/audio/mode_bassTreble.cpp

#include "modes/audio/mode_bassTreble.h"
#include "audio_engine.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"
#include <Arduino.h>

// =========================================================
// BASS VS TREBLE MODE (dual band — mid is ignored)
// Lows at the core, highs radiating outside: bass drives the
// centre hex, treble drives the outer ring. The bass hue
// drifts slowly around the wheel (~64 s per lap) and treble
// always takes the complementary hue.
// Left pot = mic sensitivity (applied in the audio engine).
// =========================================================
void mode_bassTreble() {
    float b = (float)audioBassNorm();
    float t = (float)audioTrebNorm();

    uint8_t bassHue = (uint8_t)(millis() / 250);   // full hue lap ≈ 64 s
    uint32_t bassCol = colourFromHue(bassHue);
    uint32_t trebCol = colourFromHue((uint8_t)(bassHue + 128));  // complement

    fillHex(CENTER_HEX, dimColour(bassCol, b));
    for (int i = 0; i < 6; i++) {
        fillHex(i, dimColour(trebCol, t));
    }
}
