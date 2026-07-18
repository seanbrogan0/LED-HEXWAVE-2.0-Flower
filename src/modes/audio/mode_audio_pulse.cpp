// /src/modes/audio/mode_audio_pulse.cpp

#include "modes/audio/mode_audio_pulse.h"
#include "audio_engine.h"
#include "input.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"

// =========================================================
// AUDIO MODE 0: Bass Pulse
// All hexes light up with intensity proportional to bass.
// Left pot = color.
// =========================================================
void mode_audio_pulse() {
    int colIndex = (int)(modeEngine.leftPot() * (PALETTE_MASTER_SIZE - 1));
    uint32_t col = dimColour(PALETTE_MASTER[colIndex], audioBassNorm());

    for (int h = 0; h < NUM_HEXES; h++) {
        fillHex(h, col);
    }
}
