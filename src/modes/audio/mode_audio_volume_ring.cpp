// /src/modes/audio/mode_audio_volume_ring.cpp

#include "modes/audio/mode_audio_volume_ring.h"
#include "audio_engine.h"
#include "input.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"

// =========================================================
// AUDIO MODE 3: Volume Ring
// Total audio energy drives how many hex panels light up,
// filling outward from center like a VU meter (VU_ORDER
// lives in hex_geometry). Left pot = color.
// =========================================================
void mode_audio_volume_ring() {
    double energy = (audioBassNorm() + audioMidNorm() + audioTrebNorm()) / 3.0;
    int lit = (int)(energy * NUM_HEXES);  // 0–7 hexes lit

    int colIndex = (int)(modeEngine.leftPot() * (PALETTE_MASTER_SIZE - 1));

    for (int i = 0; i < NUM_HEXES; i++) {
        if (i < lit) {
            fillHex(VU_ORDER[i], PALETTE_MASTER[colIndex]);
        } else {
            clearHex(VU_ORDER[i]);
        }
    }
}
