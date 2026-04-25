// /src/mode_engine.h
// =========================================================
// Dual-mode engine (audio + non-audio)
// - Left button: short = cycle active group, long = reset both
// - Right button: short = toggle audioModeEnabled
// - EEPROM persistence for all mode state
// - Pots smoothed and normalized (0.0 → 1.0)
// =========================================================

#pragma once
#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_ADDR_AUDIO_ENABLED   0
#define EEPROM_ADDR_AUDIO_MODE      1
#define EEPROM_ADDR_NON_AUDIO_MODE  2

#define LONG_PRESS_TIME_MS          600
#define BTN_DEBOUNCE_MS             180
#define POT_SMOOTH_ALPHA            0.1f

class ModeEngine {
public:
    ModeEngine(int leftBtnPin, int rightBtnPin, int leftPotPin, int rightPotPin,
               int numAudioModes, int numNonAudioModes);

    void begin();
    void update();

    bool audioModeEnabled() const { return audioEnabled; }
    int currentAudioMode() const { return audioMode; }
    int currentNonAudioMode() const { return nonAudioMode; }

    float leftPot()  const { return potLeft; }
    float rightPot() const { return potRight; }

private:
    int pinLeftBtn;
    int pinRightBtn;
    int pinLeftPot;
    int pinRightPot;

    int numAudioModes;
    int numNonAudioModes;

    bool audioEnabled = false;
    int audioMode = 0;
    int nonAudioMode = 0;

    bool leftPressed = false;
    bool rightPressed = false;

    unsigned long leftPressStart = 0;
    unsigned long rightPressStart = 0;
    unsigned long lastBtnTime = 0;

    float potLeft = 0;
    float potRight = 0;

    void loadState();
    void saveState();

    bool readButton(int pin);
};
