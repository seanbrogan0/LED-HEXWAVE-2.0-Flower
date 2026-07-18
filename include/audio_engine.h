// /include/audio_engine.h
// =========================================================
// Audio FFT engine
// - 256-sample FFT at a timed 8 kHz sample rate
// - DC offset removal (first-order IIR)
// - Bands: bass < 300 Hz, mid < 1100 Hz, treble above
// The main loop calls audioUpdate() once per frame when an
// audio mode is active; modes read the cached band getters.
// =========================================================

#pragma once
#include <Arduino.h>

#define SAMPLES             256
#define SAMPLING_FREQUENCY  8000

// Normalization ceilings (from the original sketch)
extern const double BASS_MAX;
extern const double MID_MAX;
extern const double TREB_MAX;

// Sample the mic, run the FFT, and cache band values
void audioUpdate(int micPin);

// Normalized bands, constrained to 0.0–1.0
double audioBassNorm();
double audioMidNorm();
double audioTrebNorm();

// Raw band magnitude sums (for energy formulas)
double audioBassRaw();
double audioMidRaw();
double audioTrebRaw();
