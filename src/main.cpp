// /src/main.cpp
// =========================================================
// LED HexWave 2.0 — main program
//
// Left button  : short = cycle mode, long = reset both groups
// Right button : toggle audio mode
// Left pot     : mode parameter (colour / speed / etc.)
// Right pot    : global brightness (2% floor)
//
// All per-fixture configuration lives in board_config.h and
// hex_geometry; all modes live in src/modes/** and register
// themselves in mode_registry.cpp.
// =========================================================

#include <Arduino.h>
#include "board_config.h"
#include "hardware.h"
#include "input.h"
#include "audio_engine.h"
#include "mode_registry.h"

// The on-hardware test harness supplies its own setup()/loop()
#ifndef PIO_UNIT_TESTING

// =========================================================
// SETUP
// =========================================================
void setup() {
    Serial.begin(115200);
    initHardware();
    modeEngine.begin();
}

// =========================================================
// LOOP
// =========================================================
void loop() {
#if FRAME_INTERVAL_MS > 0
    static unsigned long lastFrame = 0;
    if (millis() - lastFrame < FRAME_INTERVAL_MS) return;
    lastFrame = millis();
#endif

    modeEngine.update();

    // Global brightness: right pot with a floor, capped per board
    float brightness = modeEngine.rightPot();
    if (brightness < BRIGHTNESS_FLOOR) brightness = BRIGHTNESS_FLOOR;
    strip.setBrightness((uint8_t)(brightness * MAX_BRIGHTNESS + 0.5f));

    if (modeEngine.audioModeEnabled()) {
        // In the audio group the left pot is mic sensitivity, not colour
        audioSetSensitivity(modeEngine.leftPot());
        audioUpdate(PIN_MIC);
        int mode = modeEngine.currentAudioMode();
        if (audioModeAutoClear(mode)) strip.clear();
        runAudioMode(mode);
    } else {
        int mode = modeEngine.currentNonAudioMode();
        if (nonAudioModeAutoClear(mode)) strip.clear();
        runNonAudioMode(mode);
    }

#ifdef DEBUG_MODES
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
        lastPrint = millis();
        if (modeEngine.audioModeEnabled()) {
            int mode = modeEngine.currentAudioMode();
            Serial.printf("Audio Mode: %d (%s)\n", mode, audioModeName(mode));
        } else {
            int mode = modeEngine.currentNonAudioMode();
            Serial.printf("Non-Audio Mode: %d (%s)\n", mode, nonAudioModeName(mode));
        }
    }
#endif

    strip.show();
}

#endif  // PIO_UNIT_TESTING
