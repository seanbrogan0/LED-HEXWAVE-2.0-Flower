// File: src/hardware.cpp

#include "hardware.h"

// =========================================================
// STRIP DEFINITION
// =========================================================
Adafruit_NeoPixel strip(NUM_LEDS, PIN_LEDS, NEO_GRB + NEO_KHZ800);

// =========================================================
// HARDWARE INITIALISATION
// =========================================================
void initHardware() {
    strip.begin();
    strip.show();
    analogReadResolution(12);   // pots + mic use the full 0–4095 range
}
