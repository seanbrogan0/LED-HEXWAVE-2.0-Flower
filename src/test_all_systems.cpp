// /src/test_all_systems.cpp
// =========================================================
// SYSTEM TEST: verifies all subsystems
// - HexController
// - ModeEngine (buttons + pots + EEPROM)
// - Audio Engine (FFT sampling + bands)
// - Global brightness pipeline
// - Palettes + hex mapping
// =========================================================

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#include "hex_mapping.h"
#include "palettes.h"
#include "audio_engine.h"
#include "mode_engine.h"
#include "hex_controller.h"

// =========================================================
// PIN DEFINITIONS
// =========================================================
#define PIN_LEDS        17
#define NUM_LEDS        168

#define PIN_LEFTBTN     4
#define PIN_RIGHTBTN    5
#define PIN_LEFTPOT     A1
#define PIN_RIGHTPOT    A2
#define PIN_MIC         A0

// =========================================================
// MODE COUNTS (temporary for testing)
// =========================================================
#define NUM_AUDIO_MODES     2
#define NUM_NON_AUDIO_MODES 2

// =========================================================
// GLOBAL OBJECTS
// =========================================================
Adafruit_NeoPixel strip(NUM_LEDS, PIN_LEDS, NEO_GRB + NEO_KHZ800);
HexController hex(&strip);

ModeEngine modeEngine(
    PIN_LEFTBTN,
    PIN_RIGHTBTN,
    PIN_LEFTPOT,
    PIN_RIGHTPOT,
    NUM_AUDIO_MODES,
    NUM_NON_AUDIO_MODES
);

// =========================================================
// TEST MODES
// =========================================================

// ---------------------------------------------------------
// NON-AUDIO MODE 0: static palette color (left pot selects)
// ---------------------------------------------------------
void test_mode_static() {
    int index = modeEngine.leftPot() * (PALETTE_MASTER_SIZE - 1);
    uint32_t color = PALETTE_MASTER[index];

    hex.fillHex(HEX1, color);
}

// ---------------------------------------------------------
// NON-AUDIO MODE 1: rotation test (left pot = speed)
// ---------------------------------------------------------
void test_mode_rotate() {
    int steps = modeEngine.leftPot() * 6; // 0–6 steps
    hex.rotateHex(HEX2, steps);
}

// ---------------------------------------------------------
// AUDIO MODE 0: show FFT bass/mid/treb as RGB
// ---------------------------------------------------------
void test_mode_audio_rgb() {
    double bass, mid, treb;
    double bN, mN, tN;

    sampleAudio(PIN_MIC);
    processFFT(bass, mid, treb);
    normalizeBands(bN, mN, tN, bass, mid, treb);

    uint32_t color = Adafruit_NeoPixel::Color(
        bN * 255,
        mN * 255,
        treb * 255
    );

    hex.fillHex(HEX3, color);
}

// ---------------------------------------------------------
// AUDIO MODE 1: FFT bar graph inside hex
// ---------------------------------------------------------
void test_mode_audio_bar() {
    double bass, mid, treb;
    double bN, mN, tN;

    sampleAudio(PIN_MIC);
    processFFT(bass, mid, treb);
    normalizeBands(bN, mN, tN, bass, mid, treb);

    int bars = bN * HEX_LED_COUNT;

    hex.clearHex(HEX4);

    for (int i = 0; i < bars; i++) {
        hex.setHexPixel(HEX4, i, Adafruit_NeoPixel::Color(255, 80, 0));
    }
}

// =========================================================
// SETUP
// =========================================================
void test_setup() {
    Serial.begin(115200);

    strip.begin();
    strip.show();

    modeEngine.begin();

    Serial.println("=== SYSTEM TEST STARTED ===");
}

// =========================================================
// LOOP
// =========================================================
void test_loop() {
    modeEngine.update();

    // Global brightness (right pot)
    float potB = modeEngine.rightPot();   // 0.0 → 1.0
    float safeBrightness = potB * 0.70f;  // 70% cap
    hex.setGlobalBrightness(safeBrightness);

    // Clear all LEDs each frame
    for (int i = 0; i < NUM_LEDS; i++) strip.setPixelColor(i, 0);

    // Select test mode
    if (modeEngine.audioModeEnabled()) {
        switch (modeEngine.currentAudioMode()) {
            case 0: test_mode_audio_rgb(); break;
            case 1: test_mode_audio_bar(); break;
        }
    } else {
        switch (modeEngine.currentNonAudioMode()) {
            case 0: test_mode_static(); break;
            case 1: test_mode_rotate(); break;
        }
    }

    hex.show();

    // Debug output
    Serial.print("AudioModeEnabled: ");
    Serial.print(modeEngine.audioModeEnabled());
    Serial.print(" | A:");
    Serial.print(modeEngine.currentAudioMode());
    Serial.print(" | N:");
    Serial.print(modeEngine.currentNonAudioMode());
    Serial.print(" | LP:");
    Serial.print(modeEngine.leftPot(), 2);
    Serial.print(" | RP:");
    Serial.println(modeEngine.rightPot(), 2);
}
