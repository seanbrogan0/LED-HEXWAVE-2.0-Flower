// /src/modes/audio/mode_impactSparks.cpp

#include "modes/audio/mode_impactSparks.h"
#include "audio_engine.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"
#include "hardware.h"
#include <Arduino.h>

// =========================================================
// IMPACT SPARKS MODE
// Transient-driven bursts of colour across random LEDs.
// Sparks decay over ~0.5 s toward the dim purple background
// (per-LED fade with a background floor).
// Audio frames run at ~32 ms (FFT-paced), so the fade is
// tuned for ~30 fps.
// (Ported from the Linear fixture's Impact Sparks.)
// =========================================================
static float impactEnergy = 0.0f;
static unsigned long lastSparkTime = 0;

// Per-frame spark decay at the ~32 ms audio pace (hardware-tunable)
static const float SPARK_FADE = 0.85f;

void mode_impactSparks() {
    float energy = (audioBassRaw() + audioMidRaw() + audioTrebRaw())
                 / (BASS_MAX + MID_MAX + TREB_MAX);

    // Exponential smoothing of energy (matches original “laggy” feel)
    impactEnergy = impactEnergy * 0.90f + energy * 0.10f;

    // Fade every pixel toward — never below — the dim purple background
    uint32_t background = dimColour(COL_PURPLE, 0.05f);
    for (int i = 0; i < NUM_LEDS; i++) {
        uint32_t faded = dimColour(strip.getPixelColor(i), SPARK_FADE);
        strip.setPixelColor(i, maxColour32(faded, background));
    }

    // Spark rate based on energy (higher energy → more frequent sparks)
    unsigned long now = millis();
    int interval = 20 + (1 - impactEnergy) * 150;

    if (now - lastSparkTime > (unsigned long)interval) {
        lastSparkTime = now;
        int idx = random(NUM_LEDS);
        setLED(idx, randomPaletteColour());
    }
}
