// /src/modes/audio/mode_audio_pulse.cpp

#include "modes/audio/mode_audio_pulse.h"
#include "audio_engine.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"
#include <Arduino.h>

// =========================================================
// AUDIO MODE 0: Bass Pulse
// All hexes light up with intensity proportional to bass.
// Colour drifts slowly around the hue wheel (~20 s per lap).
// Left pot = mic sensitivity (applied in the audio engine).
// =========================================================
void mode_audio_pulse() {
    uint8_t hue = (uint8_t)(millis() / 80);   // full hue lap ≈ 20.5 s
    uint32_t col = dimColour(colourFromHue(hue), audioBassNorm());

    for (int h = 0; h < NUM_HEXES; h++) {
        fillHex(h, col);
    }
}
