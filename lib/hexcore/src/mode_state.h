// hexcore/mode_state.h
// =========================================================
// Mode-selection state machine (pure logic, no Arduino).
// The input layer (ModeEngine) feeds button events in;
// EEPROM bytes are validated here so garbage can never
// select an out-of-range mode.
// =========================================================

#pragma once
#include <stdint.h>

struct ModeCounts {
    int audio;
    int nonAudio;
};

struct ModeState {
    bool audioEnabled;
    int  audioMode;
    int  nonAudioMode;
};

// Short left press: advance the active group's mode, wrapping at its count
void modeStateCycle(ModeState& s, const ModeCounts& c);

// Right press: flip between audio and non-audio groups
void modeStateToggleAudio(ModeState& s);

// Long left press: reset both mode indices (audioEnabled is preserved)
void modeStateReset(ModeState& s);

// Clamp out-of-range indices to 0 (used after EEPROM load)
void modeStateValidate(ModeState& s, const ModeCounts& c);

// Build a validated state from raw EEPROM bytes
// (byte 0 = audio flag, 1 = audio mode, 2 = non-audio mode)
ModeState modeStateFromBytes(uint8_t b0, uint8_t b1, uint8_t b2,
                             const ModeCounts& c);
