// /src/main.cpp
// =========================================================
// LED HexWave 2.0 – Main animation program
//
// Left button  : short = cycle mode, long = reset both groups
// Right button : toggle audio mode
// Left pot     : mode parameter (color / speed / etc.)
// Right pot    : global brightness
//
// Non-audio modes: Rotate | Breathe | Chase | Ripple | Sparkle
// Audio modes    : Bass Pulse | Spectrum | Beat Flash | Volume Ring | Audio Ripple
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

#define NUM_AUDIO_MODES     5
#define NUM_NON_AUDIO_MODES 5

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

        int start = side * 4;
        for (int i = 0; i < 4; i++) {
            strip.setPixelColor(HEX7[start + i], col);
        }

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
    float breath = (sinf(t * 1.5f) + 1.0f) * 0.5f;  // 0.0–1.0 sine, ~4 s period

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
// NON-AUDIO MODE 3: Ripple
// Color pulses outward from center (HEX7) to the outer ring,
// then fades before repeating with the next palette color.
// Left pot = speed.
// =========================================================
static unsigned long lastRipple = 0;
static int rippleState = 0;   // 0 = center lit, 1 = ring lit, 2 = all off
static int rippleColor = 0;

void mode_ripple() {
    float s = modeEngine.leftPot();
    unsigned long interval = (unsigned long)(800.0f - s * 700.0f);  // 100–800 ms per step

    if (millis() - lastRipple >= interval) {
        lastRipple = millis();
        rippleState = (rippleState + 1) % 3;
        if (rippleState == 0) {
            rippleColor = (rippleColor + 1) % PALETTE_MASTER_SIZE;
        }
    }

    uint32_t col = PALETTE_MASTER[rippleColor];
    uint8_t dr = ((col >> 16) & 0xFF) * 0.4f;
    uint8_t dg = ((col >>  8) & 0xFF) * 0.4f;
    uint8_t db = ( col        & 0xFF) * 0.4f;
    uint32_t dimCol = Adafruit_NeoPixel::Color(dr, dg, db);

    switch (rippleState) {
        case 0:
            hex.fillHex(HEXES[6], col);
            for (int i = 0; i < 6; i++) hex.clearHex(HEXES[i]);
            break;
        case 1:
            hex.fillHex(HEXES[6], dimCol);
            for (int i = 0; i < 6; i++) hex.fillHex(HEXES[i], col);
            break;
        case 2:
            for (int h = 0; h < 7; h++) hex.clearHex(HEXES[h]);
            break;
    }
}

// =========================================================
// NON-AUDIO MODE 4: Sparkle
// Random LEDs across all panels light up in random palette
// colors and fade each frame. Left pot = spark density.
//
// Maintains its own pixel buffer so the per-frame strip.clear()
// in loop() does not erase the fade trail.
// =========================================================
static uint32_t sparkleBuf[NUM_LEDS];

void mode_sparkle() {
    // Fade all existing sparks
    for (int i = 0; i < NUM_LEDS; i++) {
        uint32_t c = sparkleBuf[i];
        if (c) {
            uint8_t r = ((c >> 16) & 0xFF) * 0.80f;
            uint8_t g = ((c >>  8) & 0xFF) * 0.80f;
            uint8_t b = ( c        & 0xFF) * 0.80f;
            sparkleBuf[i] = Adafruit_NeoPixel::Color(r, g, b);
        }
    }

    // Add new sparks scaled by left pot (1–20 per frame)
    int density = (int)(modeEngine.leftPot() * 19.0f) + 1;
    for (int i = 0; i < density; i++) {
        int led = random(NUM_LEDS);
        sparkleBuf[led] = PALETTE_MASTER[random(PALETTE_MASTER_SIZE)];
    }

    // Write buffer to strip (overrides the strip.clear() from loop)
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, sparkleBuf[i]);
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
// Center hex (HEX7) = bass, outer ring alternates mid / treble.
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
// AUDIO MODE 3: Volume Ring
// Total audio energy drives how many hex panels light up,
// filling outward from center like a VU meter.
// Left pot = color.
// =========================================================
// Hex fill order: center first, then outer ring panels 0–5
static const int VU_ORDER[7] = { 6, 0, 1, 2, 3, 4, 5 };

void mode_audio_volume_ring() {
    double bass, mid, treb, bN, mN, tN;
    sampleAudio(PIN_MIC);
    processFFT(bass, mid, treb);
    normalizeBands(bN, mN, tN, bass, mid, treb);

    double energy = (bN + mN + tN) / 3.0;
    int lit = (int)(energy * 7.0);  // 0–7 hexes lit

    int colIndex = (int)(modeEngine.leftPot() * (PALETTE_MASTER_SIZE - 1));

    for (int i = 0; i < 7; i++) {
        if (i < lit) {
            hex.fillHex(HEXES[VU_ORDER[i]], PALETTE_MASTER[colIndex]);
        } else {
            hex.clearHex(HEXES[VU_ORDER[i]]);
        }
    }
}

// =========================================================
// AUDIO MODE 4: Audio Ripple
// A bass beat triggers a ripple that expands from center
// outward through the outer ring, then fades.
// Left pot = color.
// =========================================================
static int audioRippleState = -1;   // -1 = idle
static float audioRipplePrevBass = 0.0f;
static unsigned long lastAudioRippleStep = 0;

void mode_audio_ripple() {
    double bass, mid, treb, bN, mN, tN;
    sampleAudio(PIN_MIC);
    processFFT(bass, mid, treb);
    normalizeBands(bN, mN, tN, bass, mid, treb);

    // Trigger a new ripple on a bass spike (only when idle)
    if (audioRippleState < 0
        && (float)bN > audioRipplePrevBass + 0.15f
        && (float)bN > 0.3f) {
        audioRippleState = 0;
        lastAudioRippleStep = millis();
    }
    audioRipplePrevBass = (float)bN;

    // Advance ripple step every 150 ms
    if (audioRippleState >= 0 && millis() - lastAudioRippleStep >= 150) {
        lastAudioRippleStep = millis();
        audioRippleState++;
        if (audioRippleState > 2) audioRippleState = -1;
    }

    int colIndex = (int)(modeEngine.leftPot() * (PALETTE_MASTER_SIZE - 1));
    uint32_t col = PALETTE_MASTER[colIndex];
    uint8_t dr = ((col >> 16) & 0xFF) * 0.35f;
    uint8_t dg = ((col >>  8) & 0xFF) * 0.35f;
    uint8_t db = ( col        & 0xFF) * 0.35f;
    uint32_t dimCol = Adafruit_NeoPixel::Color(dr, dg, db);

    for (int h = 0; h < 7; h++) hex.clearHex(HEXES[h]);

    switch (audioRippleState) {
        case 0:
            hex.fillHex(HEXES[6], col);                              // center full
            break;
        case 1:
            hex.fillHex(HEXES[6], dimCol);                           // center fading
            for (int i = 0; i < 6; i++) hex.fillHex(HEXES[i], col); // ring full
            break;
        case 2:
            for (int i = 0; i < 6; i++) hex.fillHex(HEXES[i], dimCol); // ring fading
            break;
        default:
            break;
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
    memset(sparkleBuf, 0, sizeof(sparkleBuf));
}

// =========================================================
// LOOP
// =========================================================
void loop() {
    modeEngine.update();

    float brightness = modeEngine.rightPot();
    if (brightness < 0.02f) brightness = 0.02f;
    hex.setGlobalBrightness(brightness);

    strip.clear();

    if (modeEngine.audioModeEnabled()) {
        switch (modeEngine.currentAudioMode()) {
            case 0: mode_audio_pulse();       break;
            case 1: mode_audio_spectrum();    break;
            case 2: mode_audio_beat();        break;
            case 3: mode_audio_volume_ring(); break;
            case 4: mode_audio_ripple();      break;
        }
    } else {
        switch (modeEngine.currentNonAudioMode()) {
            case 0: mode_rotate();  break;
            case 1: mode_breathe(); break;
            case 2: mode_chase();   break;
            case 3: mode_ripple();  break;
            case 4: mode_sparkle(); break;
        }
    }

    hex.show();
}
