// /src/hex_controller.h
// =========================================================
// Hex LED controller
// Provides clean API for filling, clearing, rotating,
// and manipulating LEDs inside each hex panel.
// Includes global brightness scaling.
// =========================================================

#pragma once
#include <Adafruit_NeoPixel.h>

class HexController {
public:
    HexController(Adafruit_NeoPixel* strip);

    void fillHex(const int* hexMap, uint32_t color);
    void clearHex(const int* hexMap);
    void setHexPixel(const int* hexMap, int logicalIndex, uint32_t color);
    void rotateHex(const int* hexMap, int amount);
    void fadeHex(const int* hexMap, float factor);

    void setGlobalBrightness(float b);   // 0.0 → 1.0 (capped externally)
    void show();

private:
    Adafruit_NeoPixel* strip;
    float globalBrightness = 1.0f;
};
