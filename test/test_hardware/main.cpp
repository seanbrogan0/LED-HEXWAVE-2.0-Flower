// test/test_hardware/main.cpp
// =========================================================
// MANUAL ON-HARDWARE HARNESS — not run by `pio test -e native`.
// Flash with hardware attached:
//   pio test -e arduino_nano_esp32 -f test_hardware
//
// Exercises the real subsystems visually:
//   Non-audio 0 : static palette colour on hex 1 (left pot selects)
//   Non-audio 1 : side-rotation test on hex 2 (left pot = steps)
//   Audio 0     : FFT bass/mid/treb as RGB on hex 3
//   Audio 1     : bass bar graph inside hex 4
// Buttons/pots/EEPROM go through the real ModeEngine; the
// current state is printed on serial every loop.
// =========================================================

#include <Arduino.h>
#include "board_config.h"
#include "hardware.h"
#include "input.h"
#include "audio_engine.h"
#include "utils.h"
#include "palette.h"
#include "hex_geometry.h"

// ---------------------------------------------------------
// NON-AUDIO 0: static palette colour (left pot selects)
// ---------------------------------------------------------
static void test_mode_static() {
    int index = (int)(modeEngine.leftPot() * (PALETTE_MASTER_SIZE - 1));
    fillHex(0, PALETTE_MASTER[index]);
}

// ---------------------------------------------------------
// NON-AUDIO 1: rotation test (left pot = rotation steps)
// ---------------------------------------------------------
static void test_mode_rotate() {
    // Fill hex 2 with 6 palette colours (4 LEDs each) before rotating
    for (int side = 0; side < 6; side++) {
        uint32_t col = PALETTE_MASTER[side];
        for (int i = 0; i < 4; i++) {
            setHexPixel(1, side * 4 + i, col);
        }
    }
    int steps = (int)(modeEngine.leftPot() * 6);  // 0–6 steps
    rotateHex(1, steps);
}

// ---------------------------------------------------------
// AUDIO 0: show FFT bass/mid/treb as RGB
// ---------------------------------------------------------
static void test_mode_audio_rgb() {
    uint32_t color = packColour(
        (uint8_t)(audioBassNorm() * 255),
        (uint8_t)(audioMidNorm()  * 255),
        (uint8_t)(audioTrebNorm() * 255)
    );
    fillHex(2, color);
}

// ---------------------------------------------------------
// AUDIO 1: bass bar graph inside a hex
// ---------------------------------------------------------
static void test_mode_audio_bar() {
    int bars = (int)(audioBassNorm() * HEX_LED_COUNT);

    clearHex(3);
    for (int i = 0; i < bars; i++) {
        setHexPixel(3, i, packColour(255, 80, 0));
    }
}

// =========================================================
// SETUP
// =========================================================
void setup() {
    Serial.begin(115200);
    initHardware();
    modeEngine.begin();
    Serial.println("=== HARDWARE HARNESS STARTED ===");
}

// =========================================================
// LOOP
// =========================================================
void loop() {
    modeEngine.update();

    // Right pot = brightness, capped at 70% for bench safety
    float brightness = modeEngine.rightPot() * 0.70f;
    if (brightness < BRIGHTNESS_FLOOR) brightness = BRIGHTNESS_FLOOR;
    strip.setBrightness((uint8_t)(brightness * MAX_BRIGHTNESS + 0.5f));

    strip.clear();

    if (modeEngine.audioModeEnabled()) {
        audioUpdate(PIN_MIC);
        switch (modeEngine.currentAudioMode() % 2) {
            case 0: test_mode_audio_rgb(); break;
            case 1: test_mode_audio_bar(); break;
        }
    } else {
        switch (modeEngine.currentNonAudioMode() % 2) {
            case 0: test_mode_static(); break;
            case 1: test_mode_rotate(); break;
        }
    }

    strip.show();

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
