// /src/modes/audio/mode_audio_volume_ring.cpp

#include "modes/audio/mode_audio_volume_ring.h"
#include "audio_engine.h"
#include "input.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"
#include <math.h>

// =========================================================
// AUDIO MODE 3: Volume Ring
// Total audio energy drives how many hex panels light up,
// filling outward from center like a VU meter (VU_ORDER
// lives in hex_geometry). A pow(e, 0.6) punch curve lets the
// meter reach full scale on real music, and the frontier hex
// fades in fractionally for a smooth edge. Left pot = color.
// =========================================================
void mode_audio_volume_ring() {
    double energy = (audioBassNorm() + audioMidNorm() + audioTrebNorm()) / 3.0;
    float e = powf((float)energy, 0.6f);   // punch curve
    float r = e * NUM_HEXES;               // 0–7 fill level

    int colIndex = (int)(modeEngine.leftPot() * (PALETTE_MASTER_SIZE - 1));
    uint32_t col = PALETTE_MASTER[colIndex];

    for (int i = 0; i < NUM_HEXES; i++) {
        // Fully lit below the level, fractional at the frontier, black above
        float level = clampFloat(r - i, 0.0f, 1.0f);
        fillHex(VU_ORDER[i], dimColour(col, level));
    }
}
