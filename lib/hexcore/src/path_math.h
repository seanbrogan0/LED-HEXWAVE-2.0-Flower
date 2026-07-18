// hexcore/path_math.h
// =========================================================
// Path-position math for modes that travel along a sequence
// of hex panels (comets, bounces). Pure logic, no Arduino.
//
// A "path position" counts LEDs along the path: position p is
// LED (p % hexSize) inside the (p / hexSize)-th hex of the path.
// Path entries are 1-based hex numbers (matching the wiring docs).
// =========================================================

#pragma once
#include <stdint.h>

// Wrap a path position into [0, totalPathLen)
int wrapPathPos(int pos, int totalPathLen);

// Convert a path position into a strip LED index
int pathPosToLED(int pathPos, const int* path, int pathLen,
                 const uint16_t* const* hexes, int hexSize);
