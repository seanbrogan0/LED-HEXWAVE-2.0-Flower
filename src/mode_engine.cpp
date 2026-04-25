// /src/mode_engine.cpp
// =========================================================
// Dual-mode engine implementation
// =========================================================

#include "mode_engine.h"

ModeEngine::ModeEngine(int leftBtnPin, int rightBtnPin, int leftPotPin, int rightPotPin,
                       int numAudioModes, int numNonAudioModes)
: pinLeftBtn(leftBtnPin),
  pinRightBtn(rightBtnPin),
  pinLeftPot(leftPotPin),
  pinRightPot(rightPotPin),
  numAudioModes(numAudioModes),
  numNonAudioModes(numNonAudioModes) {}

void ModeEngine::begin() {
    pinMode(pinLeftBtn, INPUT_PULLUP);
    pinMode(pinRightBtn, INPUT_PULLUP);

    EEPROM.begin(8);
    loadState();
}

bool ModeEngine::readButton(int pin) {
    return digitalRead(pin) == LOW;
}

void ModeEngine::update() {
    unsigned long now = millis();

    // =====================================================
    // LEFT BUTTON (short = cycle, long = reset both)
    // =====================================================
    bool leftReading = readButton(pinLeftBtn);

    if (leftReading && !leftPressed) {
        leftPressed = true;
        leftPressStart = now;
    }

    if (!leftReading && leftPressed) {
        leftPressed = false;
        unsigned long duration = now - leftPressStart;

        if (duration < LONG_PRESS_TIME_MS) {
            if (audioEnabled) {
                audioMode++;
                if (audioMode >= numAudioModes) audioMode = 0;
            } else {
                nonAudioMode++;
                if (nonAudioMode >= numNonAudioModes) nonAudioMode = 0;
            }
        } else {
            audioMode = 0;
            nonAudioMode = 0;
        }

        saveState();
    }

    // =====================================================
    // RIGHT BUTTON (short = toggle audioModeEnabled)
    // =====================================================
    bool rightReading = readButton(pinRightBtn);

    if (rightReading && !rightPressed) {
        rightPressed = true;
        rightPressStart = now;
    }

    if (!rightReading && rightPressed) {
        rightPressed = false;

        audioEnabled = !audioEnabled;
        saveState();
    }

    // =====================================================
    // POT SMOOTHING
    // =====================================================
    float rawL = analogRead(pinLeftPot) / 4095.0f;
    float rawR = analogRead(pinRightPot) / 4095.0f;

    potLeft  = (POT_SMOOTH_ALPHA * rawL) + ((1.0f - POT_SMOOTH_ALPHA) * potLeft);
    potRight = (POT_SMOOTH_ALPHA * rawR) + ((1.0f - POT_SMOOTH_ALPHA) * potRight);
}

void ModeEngine::loadState() {
    audioEnabled = EEPROM.read(EEPROM_ADDR_AUDIO_ENABLED);
    audioMode    = EEPROM.read(EEPROM_ADDR_AUDIO_MODE);
    nonAudioMode = EEPROM.read(EEPROM_ADDR_NON_AUDIO_MODE);

    if (audioMode >= numAudioModes) audioMode = 0;
    if (nonAudioMode >= numNonAudioModes) nonAudioMode = 0;
}

void ModeEngine::saveState() {
    EEPROM.write(EEPROM_ADDR_AUDIO_ENABLED, audioEnabled);
    EEPROM.write(EEPROM_ADDR_AUDIO_MODE, audioMode);
    EEPROM.write(EEPROM_ADDR_NON_AUDIO_MODE, nonAudioMode);
    EEPROM.commit();
}
