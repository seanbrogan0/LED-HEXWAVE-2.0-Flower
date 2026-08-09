// /src/modes/audio/mode_audio_beat.cpp

#include "modes/audio/mode_audio_beat.h"
#include "audio_engine.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"
#include <Arduino.h>

// =========================================================
// AUDIO MODE 2: Beat Flash
// Detects bass spikes and flashes all hexes, decaying between
// hits. Every beat flashes a new random palette colour.
// Left pot = mic sensitivity (applied in the audio engine).
// =========================================================
static float beatDecay = 0.0f;
static float lastBassLevel = 0.0f;
static uint32_t flashCol = 0;

void mode_audio_beat() {
    float bN = (float)audioBassNorm();

    if (bN > lastBassLevel + 0.15f && bN > 0.3f) {
        beatDecay = 1.0f;
        flashCol = randomPaletteColour();   // new colour per beat
    }
    lastBassLevel = bN;
    beatDecay *= 0.85f;

    uint32_t col = dimColour(flashCol, beatDecay);
    for (int h = 0; h < NUM_HEXES; h++) {
        fillHex(h, col);
    }
}
