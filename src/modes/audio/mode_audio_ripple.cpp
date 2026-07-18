// /src/modes/audio/mode_audio_ripple.cpp

#include "modes/audio/mode_audio_ripple.h"
#include "audio_engine.h"
#include "input.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"
#include <Arduino.h>

// =========================================================
// AUDIO MODE 4: Audio Ripple
// A bass beat triggers a ripple that expands from center
// outward through the outer ring, then fades.
// Left pot = color.
// =========================================================
static int audioRippleState = -1;   // -1 = idle
static float audioRipplePrevBass = 0.0f;
static unsigned long lastAudioRippleStep = 0;

void mode_audio_ripple() {
    float bN = (float)audioBassNorm();

    // Trigger a new ripple on a bass spike (only when idle)
    if (audioRippleState < 0
        && bN > audioRipplePrevBass + 0.15f
        && bN > 0.3f) {
        audioRippleState = 0;
        lastAudioRippleStep = millis();
    }
    audioRipplePrevBass = bN;

    // Advance ripple step every 150 ms
    if (audioRippleState >= 0 && millis() - lastAudioRippleStep >= 150) {
        lastAudioRippleStep = millis();
        audioRippleState++;
        if (audioRippleState > 2) audioRippleState = -1;
    }

    int colIndex = (int)(modeEngine.leftPot() * (PALETTE_MASTER_SIZE - 1));
    uint32_t col = PALETTE_MASTER[colIndex];
    uint32_t dimCol = dimColour(col, 0.35f);

    for (int h = 0; h < NUM_HEXES; h++) clearHex(h);

    switch (audioRippleState) {
        case 0:
            fillHex(CENTER_HEX, col);                            // center full
            break;
        case 1:
            fillHex(CENTER_HEX, dimCol);                         // center fading
            for (int i = 0; i < 6; i++) fillHex(i, col);         // ring full
            break;
        case 2:
            for (int i = 0; i < 6; i++) fillHex(i, dimCol);      // ring fading
            break;
        default:
            break;
    }
}
