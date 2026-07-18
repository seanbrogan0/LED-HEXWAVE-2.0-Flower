// /include/mode_registry.h
// =========================================================
// Function-pointer mode registry. The registry is the single
// source of truth for mode counts — the input layer queries
// it instead of hard-coding counts anywhere.
//
// autoClear: whether the main loop wipes the strip before the
// mode runs each frame. Trail/persistence modes set false.
// =========================================================

#pragma once

typedef void (*ModeFunc)();

struct ModeEntry {
    ModeFunc    run;
    bool        autoClear;
    const char* name;
};

int audioModeCount();
int nonAudioModeCount();

// Bounds-checked dispatch (out-of-range index is a no-op)
void runAudioMode(int index);
void runNonAudioMode(int index);

// Whether the loop should clear the strip before this mode's frame
bool audioModeAutoClear(int index);
bool nonAudioModeAutoClear(int index);
