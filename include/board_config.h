// include/board_config.h
// =========================================================
// Everything hardware-specific for THIS fixture lives here.
// The sister repo (9-hex Linear) differs only in this file,
// hex_geometry, and its mode set.
// =========================================================

#pragma once
#include <Arduino.h>

// =========================================================
// PIN DEFINITIONS
// =========================================================
#define PIN_LEDS        17
#define PIN_LEFTBTN     4
#define PIN_RIGHTBTN    5
// (no PIN_LED_IND — this fixture has no audio indicator LED)
#define PIN_LEFTPOT     A3
#define PIN_RIGHTPOT    A2
#define PIN_MIC         A0

// =========================================================
// STRIP + FRAME CONFIGURATION
// =========================================================
#define NUM_LEDS            168
#define MAX_BRIGHTNESS      255     // full range on this fixture
#define BRIGHTNESS_FLOOR    0.02f   // right pot never turns the strip fully off
#define FRAME_INTERVAL_MS   0       // 0 = run unpaced
