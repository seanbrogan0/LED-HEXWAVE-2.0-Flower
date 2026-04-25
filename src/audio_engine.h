// /src/audio_engine.h
// =========================================================
// Audio FFT engine
// NOTES:
//   - Uses 256‑sample FFT at 8 kHz
//   - Includes DC offset removal
//   - ESP32‑safe sampling loop
//   - Band splitting identical to previous build
// =========================================================

#pragma once
#include <Arduino.h>
#include <arduinoFFT.h>

// =========================================================
// AUDIO CONFIGURATION
// =========================================================
#define SAMPLES             256
#define SAMPLING_FREQUENCY  8000

// =========================================================
// FFT OBJECT + BUFFERS
// =========================================================
ArduinoFFT<double> FFT = ArduinoFFT<double>();
double vReal[SAMPLES];
double vImag[SAMPLES];

// =========================================================
// DC OFFSET REMOVAL
// =========================================================
float micDC = -1;
float micAlphaDC = 0.01;

// =========================================================
// NORMALIZATION CONSTANTS (FROM ORIGINAL BUILD)
// =========================================================
const double BASS_MAX = 100000.0;
const double MID_MAX  = 200000.0;
const double TREB_MAX = 150000.0;

// =========================================================
// SAMPLE AUDIO (ESP32‑SAFE TIMING)
// =========================================================
inline void sampleAudio(int micPin) {
    unsigned long sampling_period_us = 1000000 / SAMPLING_FREQUENCY;
    unsigned long t0;

    for (int i = 0; i < SAMPLES; i++) {

        t0 = micros();

        // Read microphone
        double raw = analogRead(micPin);

        // Remove DC offset (first‑order IIR)
        if (micDC < 0) micDC = raw;
        micDC = (micAlphaDC * raw) + ((1.0 - micAlphaDC) * micDC);
        raw -= micDC;

        vReal[i] = raw;
        vImag[i] = 0;

        // Wait for next sample
        while (micros() - t0 < sampling_period_us) {
            // tight loop
        }
    }
}

// =========================================================
// COMPUTE AUDIO BANDS (IDENTICAL TO ORIGINAL BUILD)
// =========================================================
inline void computeAudioBands(double &bass, double &mid, double &treb) {
    bass = mid = treb = 0;

    for (int i = 1; i < SAMPLES / 2; i++) {
        double freq = i * (SAMPLING_FREQUENCY / SAMPLES);
        double mag  = vReal[i];

        if (freq < 300) bass += mag;
        else if (freq < 1100) mid += mag;
        else treb += mag;
    }
}

// =========================================================
// PROCESS FFT (WINDOW → FFT → MAGNITUDE → BANDS)
// =========================================================
inline void processFFT(double &bass, double &mid, double &treb) {
    FFT.windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.complexToMagnitude(vReal, vImag, SAMPLES);
    computeAudioBands(bass, mid, treb);
}

// =========================================================
// NORMALIZE BANDS (0.0 → 1.0)
// =========================================================
inline void normalizeBands(double &bassNorm, double &midNorm, double &trebNorm,
                           double bass, double mid, double treb) {

    bassNorm = bass / BASS_MAX;
    midNorm  = mid  / MID_MAX;
    trebNorm = treb / TREB_MAX;

    bassNorm = constrain(bassNorm, 0.0, 1.0);
    midNorm  = constrain(midNorm, 0.0, 1.0);
    trebNorm = constrain(trebNorm, 0.0, 1.0);
}
