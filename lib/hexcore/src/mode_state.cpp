// hexcore/mode_state.cpp

#include "mode_state.h"

void modeStateCycle(ModeState& s, const ModeCounts& c) {
    if (s.audioEnabled) {
        s.audioMode++;
        if (s.audioMode >= c.audio) s.audioMode = 0;
    } else {
        s.nonAudioMode++;
        if (s.nonAudioMode >= c.nonAudio) s.nonAudioMode = 0;
    }
}

void modeStateToggleAudio(ModeState& s) {
    s.audioEnabled = !s.audioEnabled;
}

void modeStateReset(ModeState& s) {
    s.audioMode = 0;
    s.nonAudioMode = 0;
}

void modeStateValidate(ModeState& s, const ModeCounts& c) {
    if (s.audioMode < 0 || s.audioMode >= c.audio) s.audioMode = 0;
    if (s.nonAudioMode < 0 || s.nonAudioMode >= c.nonAudio) s.nonAudioMode = 0;
}

ModeState modeStateFromBytes(uint8_t b0, uint8_t b1, uint8_t b2,
                             const ModeCounts& c) {
    ModeState s;
    s.audioEnabled = (b0 == 1);
    s.audioMode    = b1;
    s.nonAudioMode = b2;
    modeStateValidate(s, c);
    return s;
}
