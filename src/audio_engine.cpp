// /src/audio_engine.cpp

#include "audio_engine.h"
#include <arduinoFFT.h>

// =========================================================
// FFT OBJECT + BUFFERS (file-local — defined once, here)
// =========================================================
static ArduinoFFT<double> FFT = ArduinoFFT<double>();
static double vReal[SAMPLES];
static double vImag[SAMPLES];

// DC offset removal
static float micDC = -1;
static const float micAlphaDC = 0.01f;

// Normalization ceilings (from the original sketch)
const double BASS_MAX = 100000.0;
const double MID_MAX  = 200000.0;
const double TREB_MAX = 150000.0;

// Cached band values, refreshed by audioUpdate()
static double bassRaw = 0, midRaw = 0, trebRaw = 0;
static double bassN = 0, midN = 0, trebN = 0;

// =========================================================
// SAMPLE AUDIO (ESP32-SAFE TIMED 8 kHz LOOP)
// =========================================================
static void sampleAudio(int micPin) {
    unsigned long sampling_period_us = 1000000 / SAMPLING_FREQUENCY;
    unsigned long t0;

    for (int i = 0; i < SAMPLES; i++) {
        t0 = micros();

        double raw = analogRead(micPin);

        // Remove DC offset (first-order IIR)
        if (micDC < 0) micDC = raw;
        micDC = (micAlphaDC * raw) + ((1.0f - micAlphaDC) * micDC);
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
// SUM FFT MAGNITUDES INTO BASS / MID / TREBLE BUCKETS
// =========================================================
static void computeAudioBands() {
    bassRaw = midRaw = trebRaw = 0;

    for (int i = 1; i < SAMPLES / 2; i++) {
        double freq = i * (SAMPLING_FREQUENCY / SAMPLES);
        double mag  = vReal[i];

        if (freq < 300) bassRaw += mag;
        else if (freq < 1100) midRaw += mag;
        else trebRaw += mag;
    }
}

// =========================================================
// UPDATE (SAMPLE → WINDOW → FFT → MAGNITUDE → BANDS)
// =========================================================
void audioUpdate(int micPin) {
    sampleAudio(micPin);

    FFT.windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.complexToMagnitude(vReal, vImag, SAMPLES);

    computeAudioBands();

    bassN = constrain(bassRaw / BASS_MAX, 0.0, 1.0);
    midN  = constrain(midRaw  / MID_MAX,  0.0, 1.0);
    trebN = constrain(trebRaw / TREB_MAX, 0.0, 1.0);
}

// =========================================================
// CACHED BAND GETTERS
// =========================================================
double audioBassNorm() { return bassN; }
double audioMidNorm()  { return midN; }
double audioTrebNorm() { return trebN; }

double audioBassRaw() { return bassRaw; }
double audioMidRaw()  { return midRaw; }
double audioTrebRaw() { return trebRaw; }
