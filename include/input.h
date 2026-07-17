// File: include/input.h
// =========================================================
// ModeEngine — buttons, pots, and persisted mode state.
// - Left button : short = cycle active group, long = reset both
// - Right button: toggle audio/non-audio (+ indicator LED)
// - Pots smoothed (IIR) and normalized to 0.0–1.0
// - State persisted to EEPROM and validated against the
//   mode registry on load (pure logic in hexcore/mode_state)
// =========================================================

#pragma once
#include <Arduino.h>
#include "mode_state.h"

#define LONG_PRESS_TIME_MS  600
#define BTN_DEBOUNCE_MS     180
#define POT_SMOOTH_ALPHA    0.1f

class ModeEngine {
public:
    void begin();
    void update();

    bool audioModeEnabled() const { return state.audioEnabled; }
    int currentAudioMode() const { return state.audioMode; }
    int currentNonAudioMode() const { return state.nonAudioMode; }

    float leftPot()  const { return potLeft; }
    float rightPot() const { return potRight; }

private:
    ModeState state = { false, 0, 0 };

    bool leftPressed = false;
    bool rightPressed = false;

    unsigned long leftPressStart = 0;
    unsigned long rightPressStart = 0;
    unsigned long lastBtnTime = 0;

    float potLeft = 0;
    float potRight = 0;

    void loadState();
    void saveState();
    void updateIndicator();
    bool readButton(int pin);
};

extern ModeEngine modeEngine;
