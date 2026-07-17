// File: include/hardware.h

#pragma once
#include <Adafruit_NeoPixel.h>
#include "board_config.h"

// =========================================================
// LED STRIP (defined in hardware.cpp)
// =========================================================
extern Adafruit_NeoPixel strip;

// =========================================================
// INITIALISATION (strip + ADC)
// =========================================================
void initHardware();
