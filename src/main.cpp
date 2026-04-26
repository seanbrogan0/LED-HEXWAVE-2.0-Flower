// /src/main.cpp
// =========================================================
// ROTATING SIDE–COLOUR TEST (STANDALONE)
// - Left pot  = rotation speed
// - Right pot = brightness
// - Uses PALETTE_MASTER directly
// =========================================================

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "hex_mapping.h"
#include "palettes.h"

// =========================================================
// PIN DEFINITIONS (correct for your hardware)
// =========================================================
#define PIN_LEDS        17
#define PIN_LEFTPOT     A3   // GPIO 4
#define PIN_RIGHTPOT    A2   // GPIO 3

#define NUM_LEDS        168

Adafruit_NeoPixel strip(NUM_LEDS, PIN_LEDS, NEO_GRB + NEO_KHZ800);

// =========================================================
// SIDE COLOURS (from PALETTE_MASTER)
// =========================================================
static const uint32_t SIDE_COLORS[6] = {
    PALETTE_MASTER[0],   // Red
    PALETTE_MASTER[1],   // Deep Pink
    PALETTE_MASTER[2],   // Cyan
    PALETTE_MASTER[3],   // Neon Green
    PALETTE_MASTER[4],   // Yellow
    PALETTE_MASTER[5]    // Orange
};

// =========================================================
// ROTATION STATE
// =========================================================
unsigned long lastRotate = 0;
float rotateInterval = 1000;
int rotateIndex = 0;

// =========================================================
// SAFE POT READ
// =========================================================
float readPot(int pin) {
    int raw = analogRead(pin);
    if (raw < 0) raw = 0;
    if (raw > 4095) raw = 4095;
    return raw / 4095.0f;
}

// =========================================================
// SETUP
// =========================================================
void setup() {
    strip.begin();
    strip.show();
    analogReadResolution(12);
}

// =========================================================
// LOOP
// =========================================================
void loop() {

    // RIGHT POT → BRIGHTNESS
    float b = readPot(PIN_RIGHTPOT);
    uint8_t brightness = (uint8_t)(b * 255);
    if (brightness < 5) brightness = 5;
    strip.setBrightness(brightness);

    // LEFT POT → SPEED
    float s = readPot(PIN_LEFTPOT);
    rotateInterval = 2500 - (s * 2300);

    // ROTATION TIMER
    if (millis() - lastRotate >= rotateInterval) {
        lastRotate = millis();
        rotateIndex = (rotateIndex + 5) % 6;   // anti‑clockwise
    }

    strip.clear();

    // HEX7 SIDES
    for (int side = 0; side < 6; side++) {
        int colIndex = (side + rotateIndex) % 6;
        uint32_t col = SIDE_COLORS[colIndex];

        int start = side * 4;
        for (int i = 0; i < 4; i++) {
            strip.setPixelColor(HEX7[start + i], col);
        }
    }

    // CONNECTED HEXES
    for (int side = 0; side < 6; side++) {
        int colIndex = (side + rotateIndex) % 6;
        uint32_t col = SIDE_COLORS[colIndex];

        int hexNum = HEX7_SIDE_TO_HEX[side];
        const int* hex = HEXES[hexNum - 1];

        for (int i = 0; i < HEX_LED_COUNT; i++) {
            strip.setPixelColor(hex[i], col);
        }
    }

    strip.show();
}
