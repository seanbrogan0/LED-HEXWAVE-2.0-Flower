// File: src/input.cpp

#include "input.h"
#include "board_config.h"
#include "mode_registry.h"
#include <EEPROM.h>

ModeEngine modeEngine;

// EEPROM layout (unchanged from the original sketches):
// byte 0 = audio flag, byte 1 = audio mode, byte 2 = non-audio mode
#define EEPROM_ADDR_AUDIO_ENABLED   0
#define EEPROM_ADDR_AUDIO_MODE      1
#define EEPROM_ADDR_NON_AUDIO_MODE  2

static ModeCounts registryCounts() {
    return ModeCounts{ audioModeCount(), nonAudioModeCount() };
}

void ModeEngine::begin() {
    pinMode(PIN_LEFTBTN, INPUT_PULLUP);
    pinMode(PIN_RIGHTBTN, INPUT_PULLUP);
#ifdef PIN_LED_IND
    pinMode(PIN_LED_IND, OUTPUT);
#endif

    EEPROM.begin(8);
    loadState();
    updateIndicator();
}

bool ModeEngine::readButton(int pin) {
    return digitalRead(pin) == LOW;
}

void ModeEngine::update() {
    unsigned long now = millis();

    // =====================================================
    // LEFT BUTTON (short = cycle, long = reset both)
    // =====================================================
    bool leftReading = readButton(PIN_LEFTBTN);

    if (leftReading && !leftPressed && (now - lastBtnTime) >= BTN_DEBOUNCE_MS) {
        leftPressed = true;
        leftPressStart = now;
        lastBtnTime = now;
    }

    if (!leftReading && leftPressed) {
        leftPressed = false;
        unsigned long duration = now - leftPressStart;

        if (duration < LONG_PRESS_TIME_MS) {
            modeStateCycle(state, registryCounts());
        } else {
            modeStateReset(state);
        }

        saveState();
    }

    // =====================================================
    // RIGHT BUTTON (short = toggle audioModeEnabled)
    // =====================================================
    bool rightReading = readButton(PIN_RIGHTBTN);

    if (rightReading && !rightPressed && (now - lastBtnTime) >= BTN_DEBOUNCE_MS) {
        rightPressed = true;
        rightPressStart = now;
        lastBtnTime = now;
    }

    if (!rightReading && rightPressed) {
        rightPressed = false;

        modeStateToggleAudio(state);
        updateIndicator();
        saveState();
    }

    // =====================================================
    // POT SMOOTHING
    // =====================================================
    float rawL = analogRead(PIN_LEFTPOT) / 4095.0f;
    float rawR = analogRead(PIN_RIGHTPOT) / 4095.0f;

    potLeft  = (POT_SMOOTH_ALPHA * rawL) + ((1.0f - POT_SMOOTH_ALPHA) * potLeft);
    potRight = (POT_SMOOTH_ALPHA * rawR) + ((1.0f - POT_SMOOTH_ALPHA) * potRight);
}

void ModeEngine::updateIndicator() {
#ifdef PIN_LED_IND
    digitalWrite(PIN_LED_IND, state.audioEnabled ? HIGH : LOW);
#endif
}

void ModeEngine::loadState() {
    state = modeStateFromBytes(
        EEPROM.read(EEPROM_ADDR_AUDIO_ENABLED),
        EEPROM.read(EEPROM_ADDR_AUDIO_MODE),
        EEPROM.read(EEPROM_ADDR_NON_AUDIO_MODE),
        registryCounts()
    );
}

void ModeEngine::saveState() {
    EEPROM.write(EEPROM_ADDR_AUDIO_ENABLED, state.audioEnabled ? 1 : 0);
    EEPROM.write(EEPROM_ADDR_AUDIO_MODE, (uint8_t)state.audioMode);
    EEPROM.write(EEPROM_ADDR_NON_AUDIO_MODE, (uint8_t)state.nonAudioMode);
    EEPROM.commit();
}
