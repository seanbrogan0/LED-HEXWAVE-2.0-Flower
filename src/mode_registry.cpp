// /src/mode_registry.cpp

#include "mode_registry.h"

// Audio modes
#include "modes/audio/mode_audio_pulse.h"
#include "modes/audio/mode_audio_spectrum.h"
#include "modes/audio/mode_audio_beat.h"
#include "modes/audio/mode_audio_volume_ring.h"
#include "modes/audio/mode_audio_ripple.h"

// Non-audio modes
#include "modes/non_audio/mode_rotate.h"
#include "modes/non_audio/mode_breathe.h"
#include "modes/non_audio/mode_chase.h"
#include "modes/non_audio/mode_ripple.h"
#include "modes/non_audio/mode_sparkle.h"

// =========================================================
// MODE TABLES
// All modes on this fixture repaint every LED they own each
// frame, so they all use autoClear (Sparkle keeps its own
// buffer and simply overwrites the cleared strip).
// =========================================================
static const ModeEntry audioModes[] = {
    { mode_audio_pulse,       true, "Bass Pulse" },
    { mode_audio_spectrum,    true, "Spectrum" },
    { mode_audio_beat,        true, "Beat Flash" },
    { mode_audio_volume_ring, true, "Volume Ring" },
    { mode_audio_ripple,      true, "Audio Ripple" },
};

static const ModeEntry nonAudioModes[] = {
    { mode_rotate,            true, "Rotate" },
    { mode_breathe,           true, "Breathe" },
    { mode_chase,             true, "Chase" },
    { mode_ripple,            true, "Ripple" },
    { mode_sparkle,           true, "Sparkle" },
};

// =========================================================
// REGISTRY API
// =========================================================
int audioModeCount() {
    return sizeof(audioModes) / sizeof(audioModes[0]);
}

int nonAudioModeCount() {
    return sizeof(nonAudioModes) / sizeof(nonAudioModes[0]);
}

void runAudioMode(int index) {
    if (index < 0 || index >= audioModeCount()) return;
    audioModes[index].run();
}

void runNonAudioMode(int index) {
    if (index < 0 || index >= nonAudioModeCount()) return;
    nonAudioModes[index].run();
}

bool audioModeAutoClear(int index) {
    if (index < 0 || index >= audioModeCount()) return false;
    return audioModes[index].autoClear;
}

bool nonAudioModeAutoClear(int index) {
    if (index < 0 || index >= nonAudioModeCount()) return false;
    return nonAudioModes[index].autoClear;
}
