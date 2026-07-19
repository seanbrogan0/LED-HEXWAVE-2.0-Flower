// hexcore/hex_geometry.cpp
// (Index lists preserved exactly from the verified mapping —
//  the non-sequential runs correct for each panel's wiring.)

#include "hex_geometry.h"

const uint16_t HEX1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
     18, 19, 20, 21, 22, 23};
const uint16_t HEX2[] = {44, 45, 46, 47, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
     36, 37, 38, 39, 40, 41, 42, 43};
const uint16_t HEX3[] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
     64, 65, 66, 67, 68, 69, 70, 71};
const uint16_t HEX4[] = {72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87,
     88, 89, 90, 91, 92, 93, 94, 95};
const uint16_t HEX5[] = {100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
     113, 114, 115, 116, 117, 118, 119, 96, 97, 98, 99};
const uint16_t HEX6[] = {128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
     141, 142, 143, 120, 121, 122, 123, 124, 125, 126, 127};
const uint16_t HEX7[] = {164, 165, 166, 167, 144, 145, 146, 147, 148, 149, 150, 151, 152,
     153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163};

const uint16_t* const HEXES[NUM_HEXES] = {
    HEX1, HEX2, HEX3, HEX4, HEX5, HEX6, HEX7
};

// Side colours (0–5): Red, Orange, Yellow, Green, Blue, Purple
// Physical adjacency (verified via rotation test):
//   side 0 → HEX1, side 1 → HEX6, side 2 → HEX5,
//   side 3 → HEX4, side 4 → HEX3, side 5 → HEX2
const int HEX7_SIDE_TO_HEX[6] = { 1, 6, 5, 4, 3, 2 };

// VU fill: center first, then outer ring panels 0–5
const int VU_ORDER[NUM_HEXES] = { 6, 0, 1, 2, 3, 4, 5 };

// Outer-ring orbit in physical rotational order (closed cycle)
const int HEX_PATH_RING[] = { 1, 6, 5, 4, 3, 2 };
