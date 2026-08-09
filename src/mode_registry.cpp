// /src/mode_registry.cpp

#include "mode_registry.h"

// Audio modes
#include "modes/audio/mode_audio_pulse.h"
#include "modes/audio/mode_audio_spectrum.h"
#include "modes/audio/mode_audio_beat.h"
#include "modes/audio/mode_audio_volume_ring.h"
#include "modes/audio/mode_audio_ripple.h"
#include "modes/audio/mode_impactSparks.h"
#include "modes/audio/mode_bassTreble.h"

// Non-audio modes
#include "modes/non_audio/mode_rotate.h"
#include "modes/non_audio/mode_breathe.h"
#include "modes/non_audio/mode_chase.h"
#include "modes/non_audio/mode_ripple.h"
#include "modes/non_audio/mode_sparkle.h"
#include "modes/non_audio/mode_hexFadeRandom.h"
#include "modes/non_audio/mode_dualRingComets.h"
#include "modes/non_audio/mode_solid.h"
#include "modes/non_audio/mode_rainbowFlow.h"

// =========================================================
// MODE TABLES
// Most modes on this fixture repaint every LED they own each
// frame and use autoClear (Sparkle keeps its own buffer and
// simply overwrites the cleared strip). Dual Ring Comets and
// Impact Sparks rely on pixel persistence for their trails,
// so they run with autoClear = false. New entries are
// appended so saved EEPROM mode indices stay valid.
// =========================================================
static const ModeEntry audioModes[] = {
    { mode_audio_pulse,       true,  "Bass Pulse" },
    { mode_audio_spectrum,    true,  "Spectrum" },
    { mode_audio_beat,        true,  "Beat Flash" },
    { mode_audio_volume_ring, true,  "Volume Ring" },
    { mode_audio_ripple,      true,  "Audio Ripple" },
    { mode_impactSparks,      false, "Impact Sparks" },
    { mode_bassTreble,        true,  "Bass vs Treble" },
};

static const ModeEntry nonAudioModes[] = {
    { mode_rotate,            true,  "Rotate" },
    { mode_breathe,           true,  "Breathe" },
    { mode_chase,             true,  "Chase" },
    { mode_ripple,            true,  "Ripple" },
    { mode_sparkle,           true,  "Sparkle" },
    { mode_hexFadeRandom,     true,  "Hex Fade Random" },
    { mode_dualRingComets,    false, "Dual Ring Comets" },
    { mode_solid,             true,  "Solid" },
    { mode_rainbowFlow,       true,  "Rainbow Flow" },
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

const char* audioModeName(int index) {
    if (index < 0 || index >= audioModeCount()) return "?";
    return audioModes[index].name;
}

const char* nonAudioModeName(int index) {
    if (index < 0 || index >= nonAudioModeCount()) return "?";
    return nonAudioModes[index].name;
}
