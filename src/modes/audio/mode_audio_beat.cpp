// /src/modes/audio/mode_audio_beat.cpp

#include "modes/audio/mode_audio_beat.h"
#include "audio_engine.h"
#include "input.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"

// =========================================================
// AUDIO MODE 2: Beat Flash
// Detects bass spikes and flashes all hexes, decaying between hits.
// Left pot = color.
// =========================================================
static float beatDecay = 0.0f;
static float lastBassLevel = 0.0f;

void mode_audio_beat() {
    float bN = (float)audioBassNorm();

    if (bN > lastBassLevel + 0.15f && bN > 0.3f) {
        beatDecay = 1.0f;
    }
    lastBassLevel = bN;
    beatDecay *= 0.85f;

    int colIndex = (int)(modeEngine.leftPot() * (PALETTE_MASTER_SIZE - 1));
    uint32_t col = dimColour(PALETTE_MASTER[colIndex], beatDecay);

    for (int h = 0; h < NUM_HEXES; h++) {
        fillHex(h, col);
    }
}
