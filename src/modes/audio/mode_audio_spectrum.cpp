// /src/modes/audio/mode_audio_spectrum.cpp

#include "modes/audio/mode_audio_spectrum.h"
#include "audio_engine.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"

// =========================================================
// AUDIO MODE 1: Spectrum
// Center hex (HEX7) = bass, outer ring alternates mid / treble.
// =========================================================
void mode_audio_spectrum() {
    // Center = bass (red)
    fillHex(CENTER_HEX, packColour((uint8_t)(255 * audioBassNorm()), 0, 0));

    // Outer ring: even = mid (green), odd = treble (blue)
    for (int i = 0; i < 6; i++) {
        uint32_t col = (i % 2 == 0)
            ? packColour(0, (uint8_t)(255 * audioMidNorm()), 0)
            : packColour(0, 0, (uint8_t)(255 * audioTrebNorm()));
        fillHex(i, col);
    }
}
