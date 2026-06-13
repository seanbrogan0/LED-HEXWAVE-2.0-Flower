// /src/main.cpp
// =========================================================
// LED HexWave 2.0 – Main animation program
//
// Left button  : short = cycle mode, long = reset both groups
// Right button : toggle audio mode
// Left pot     : mode parameter (color / speed / etc.)
// Right pot    : global brightness
//
// Non-audio modes: Rotate | Breathe | Chase
// Audio modes    : Bass Pulse | Spectrum | Beat Flash
// =========================================================

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>
#include "hex_mapping.h"
#include "palettes.h"
#include "hex_controller.h"
#include "mode_engine.h"
#include "audio_engine.h"

// =========================================================
// PIN DEFINITIONS
// =========================================================
#define PIN_LEDS        17
#define NUM_LEDS        168

#define PIN_LEFTBTN     4
#define PIN_RIGHTBTN    5
#define PIN_LEFTPOT     A3
#define PIN_RIGHTPOT    A2
#define PIN_MIC         A0

#define NUM_AUDIO_MODES     3
#define NUM_NON_AUDIO_MODES 3

// =========================================================
// GLOBAL OBJECTS
// =========================================================
Adafruit_NeoPixel strip(NUM_LEDS, PIN_LEDS, NEO_GRB + NEO_KHZ800);
HexController hex(&strip);

ModeEngine modeEngine(
    PIN_LEFTBTN, PIN_RIGHTBTN,
    PIN_LEFTPOT, PIN_RIGHTPOT,
    NUM_AUDIO_MODES, NUM_NON_AUDIO_MODES
);

// =========================================================
// NON-AUDIO MODE 0: Rotate
// 6 colors cycle around HEX7's sides and connected hexes.
// Left pot = rotation speed.
// =========================================================
static unsigned long lastRotate = 0;
static int rotateIndex = 0;

void mode_rotate() {
    float s = modeEngine.leftPot();
    unsigned long interval = (unsigned long)(2500.0f - s * 2300.0f);

    if (millis() - lastRotate >= interval) {
        lastRotate = millis();
        rotateIndex = (rotateIndex + 5) % 6;  // +5 mod 6 == -1 (anti-clockwise)
    }

    for (int side = 0; side < 6; side++) {
        int colIndex = (side + rotateIndex) % 6;
        uint32_t col = PALETTE_MASTER[colIndex];

        // Color 4-LED side slice of HEX7
        int start = side * 4;
        for (int i = 0; i < 4; i++) {
            strip.setPixelColor(HEX7[start + i], col);
        }

        // Fill the connected outer hex with the same color
        hex.fillHex(HEXES[HEX7_SIDE_TO_HEX[side] - 1], col);
    }
}

// =========================================================
// NON-AUDIO MODE 1: Breathe
// All hexes pulse a single palette color in and out.
// Left pot = color selection.
// =========================================================
void mode_breathe() {
    float t = millis() / 1000.0f;
    float breath = (sinf(t * 1.5f) + 1.0f) * 0.5f;  // 0.0–1.0 sine wave, ~4 s period

    int colIndex = (int)(modeEngine.leftPot() * (PALETTE_MASTER_SIZE - 1));
    uint32_t base = PALETTE_MASTER[colIndex];

    uint8_t r = ((base >> 16) & 0xFF) * breath;
    uint8_t g = ((base >>  8) & 0xFF) * breath;
    uint8_t b = ( base        & 0xFF) * breath;
    uint32_t col = Adafruit_NeoPixel::Color(r, g, b);

    for (int h = 0; h < 7; h++) {
        hex.fillHex(HEXES[h], col);
    }
}

// =========================================================
// NON-AUDIO MODE 2: Chase
// One hex lights up at a time, stepping around all 7 panels.
// Left pot = speed.
// =========================================================
static unsigned long lastChase = 0;
static int chaseIndex = 0;
static int chaseColor = 0;

void mode_chase() {
    float s = modeEngine.leftPot();
    unsigned long interval = (unsigned long)(600.0f - s * 550.0f);  // 50–600 ms

    if (millis() - lastChase >= interval) {
        lastChase = millis();
        chaseIndex = (chaseIndex + 1) % 7;
        if (chaseIndex == 0) {
            chaseColor = (chaseColor + 1) % PALETTE_MASTER_SIZE;
        }
    }

    for (int h = 0; h < 7; h++) {
        if (h == chaseIndex) {
            hex.fillHex(HEXES[h], PALETTE_MASTER[chaseColor]);
        } else {
            hex.clearHex(HEXES[h]);
        }
    }
}

// =========================================================
// AUDIO MODE 0: Bass Pulse
// All hexes light up with intensity proportional to bass.
// Left pot = color.
// =========================================================
void mode_audio_pulse() {
    double bass, mid, treb, bN, mN, tN;
    sampleAudio(PIN_MIC);
    processFFT(bass, mid, treb);
    normalizeBands(bN, mN, tN, bass, mid, treb);

    int colIndex = (int)(modeEngine.leftPot() * (PALETTE_MASTER_SIZE - 1));
    uint32_t base = PALETTE_MASTER[colIndex];

    uint8_t r = ((base >> 16) & 0xFF) * bN;
    uint8_t g = ((base >>  8) & 0xFF) * bN;
    uint8_t b = ( base        & 0xFF) * bN;
    uint32_t col = Adafruit_NeoPixel::Color(r, g, b);

    for (int h = 0; h < 7; h++) {
        hex.fillHex(HEXES[h], col);
    }
}

// =========================================================
// AUDIO MODE 1: Spectrum
// Center hex (HEX7) shows bass, outer ring alternates mid / treble.
// =========================================================
void mode_audio_spectrum() {
    double bass, mid, treb, bN, mN, tN;
    sampleAudio(PIN_MIC);
    processFFT(bass, mid, treb);
    normalizeBands(bN, mN, tN, bass, mid, treb);

    // Center = bass (red)
    hex.fillHex(HEXES[6], Adafruit_NeoPixel::Color((uint8_t)(255 * bN), 0, 0));

    // Outer ring: even = mid (green), odd = treble (blue)
    for (int i = 0; i < 6; i++) {
        uint32_t col = (i % 2 == 0)
            ? Adafruit_NeoPixel::Color(0, (uint8_t)(255 * mN), 0)
            : Adafruit_NeoPixel::Color(0, 0, (uint8_t)(255 * tN));
        hex.fillHex(HEXES[i], col);
    }
}

// =========================================================
// AUDIO MODE 2: Beat Flash
// Detects bass spikes and flashes all hexes, decaying between hits.
// Left pot = color.
// =========================================================
static float beatDecay = 0.0f;
static float lastBassLevel = 0.0f;

void mode_audio_beat() {
    double bass, mid, treb, bN, mN, tN;
    sampleAudio(PIN_MIC);
    processFFT(bass, mid, treb);
    normalizeBands(bN, mN, tN, bass, mid, treb);

    // Trigger on a significant bass spike above the previous frame's level
    if ((float)bN > lastBassLevel + 0.15f && (float)bN > 0.3f) {
        beatDecay = 1.0f;
    }
    lastBassLevel = (float)bN;
    beatDecay *= 0.85f;

    int colIndex = (int)(modeEngine.leftPot() * (PALETTE_MASTER_SIZE - 1));
    uint32_t base = PALETTE_MASTER[colIndex];

    uint8_t r = ((base >> 16) & 0xFF) * beatDecay;
    uint8_t g = ((base >>  8) & 0xFF) * beatDecay;
    uint8_t b = ( base        & 0xFF) * beatDecay;
    uint32_t col = Adafruit_NeoPixel::Color(r, g, b);

    for (int h = 0; h < 7; h++) {
        hex.fillHex(HEXES[h], col);
    }
}

// =========================================================
// SETUP
// =========================================================
void setup() {
    Serial.begin(115200);
    strip.begin();
    strip.show();
    analogReadResolution(12);
    modeEngine.begin();
}

// =========================================================
// LOOP
// =========================================================
void loop() {
    modeEngine.update();

    float brightness = modeEngine.rightPot();
    if (brightness < 0.02f) brightness = 0.02f;  // always a faint glow at minimum
    hex.setGlobalBrightness(brightness);

    strip.clear();

    if (modeEngine.audioModeEnabled()) {
        switch (modeEngine.currentAudioMode()) {
            case 0: mode_audio_pulse();    break;
            case 1: mode_audio_spectrum(); break;
            case 2: mode_audio_beat();     break;
        }
    } else {
        switch (modeEngine.currentNonAudioMode()) {
            case 0: mode_rotate();  break;
            case 1: mode_breathe(); break;
            case 2: mode_chase();   break;
        }
    }

    hex.show();
}
