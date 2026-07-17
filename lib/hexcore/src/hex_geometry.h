// hexcore/hex_geometry.h
// =========================================================
// Physical LED geometry for THIS fixture: a flower of 7 hex
// panels — HEX7 in the center, HEX1–HEX6 around it — with
// 24 LEDs each, 168 LEDs total. Pure data, no Arduino.
//
// Each HEXn array lists strip indices in the panel's logical
// rotational order (verified against the physical wiring).
// =========================================================

#pragma once
#include <stdint.h>

constexpr int NUM_HEXES     = 7;
constexpr int HEX_LED_COUNT = 24;

extern const uint16_t HEX1[];
extern const uint16_t HEX2[];
extern const uint16_t HEX3[];
extern const uint16_t HEX4[];
extern const uint16_t HEX5[];
extern const uint16_t HEX6[];
extern const uint16_t HEX7[];

// Master array for iteration (index 0 = HEX1; index 6 = center HEX7)
extern const uint16_t* const HEXES[NUM_HEXES];

// Index of the center hex within HEXES
constexpr int CENTER_HEX = 6;

// =========================================================
// FLOWER ADJACENCY (verified via rotation test)
// =========================================================
// HEX7 side s (4 LEDs starting at s*4) touches outer hex
// HEX7_SIDE_TO_HEX[s] (1-based hex number).
extern const int HEX7_SIDE_TO_HEX[6];

// VU-meter fill order: center first, then the outer ring
// (0-based indices into HEXES)
extern const int VU_ORDER[NUM_HEXES];
