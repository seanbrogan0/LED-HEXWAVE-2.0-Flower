// /src/modes/audio/mode_audio_volume_ring.cpp

#include "modes/audio/mode_audio_volume_ring.h"
#include "audio_engine.h"
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
// fades in fractionally for a smooth edge. Classic VU
// colouring: green at the centre through yellow to red at
// full scale. Left pot = mic sensitivity (audio engine).
// =========================================================
void mode_audio_volume_ring() {
    double energy = (audioBassNorm() + audioMidNorm() + audioTrebNorm()) / 3.0;
    float e = powf((float)energy, 0.6f);   // punch curve
    float r = e * NUM_HEXES;               // 0–7 fill level

    for (int i = 0; i < NUM_HEXES; i++) {
        // Fully lit below the level, fractional at the frontier, black above
        float level = clampFloat(r - i, 0.0f, 1.0f);

        // Green (hue 85) at the centre -> red (hue 0) at the last hex
        uint8_t hue = (uint8_t)(85 - (85 * i) / (NUM_HEXES - 1));
        fillHex(VU_ORDER[i], dimColour(colourFromHue(hue), level));
    }
}
