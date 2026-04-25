// /src/hex_controller.cpp
// =========================================================
// Hex LED controller implementation
// =========================================================

#include "hex_controller.h"
#include "hex_mapping.h"   // for HEX_LED_COUNT

HexController::HexController(Adafruit_NeoPixel* strip)
: strip(strip) {}

void HexController::fillHex(const int* hexMap, uint32_t color) {
    for (int i = 0; i < HEX_LED_COUNT; i++) {
        strip->setPixelColor(hexMap[i], color);
    }
}

void HexController::clearHex(const int* hexMap) {
    fillHex(hexMap, 0);
}

void HexController::setHexPixel(const int* hexMap, int logicalIndex, uint32_t color) {
    if (logicalIndex < 0 || logicalIndex >= HEX_LED_COUNT) return;
    strip->setPixelColor(hexMap[logicalIndex], color);
}

void HexController::rotateHex(const int* hexMap, int amount) {
    uint32_t temp[HEX_LED_COUNT];

    for (int i = 0; i < HEX_LED_COUNT; i++) {
        int src = (i - amount + HEX_LED_COUNT) % HEX_LED_COUNT;
        temp[i] = strip->getPixelColor(hexMap[src]);
    }

    for (int i = 0; i < HEX_LED_COUNT; i++) {
        strip->setPixelColor(hexMap[i], temp[i]);
    }
}

void HexController::fadeHex(const int* hexMap, float factor) {
    for (int i = 0; i < HEX_LED_COUNT; i++) {
        uint32_t c = strip->getPixelColor(hexMap[i]);

        uint8_t r = ((c >> 16) & 0xFF) * factor;
        uint8_t g = ((c >>  8) & 0xFF) * factor;
        uint8_t b = ( c        & 0xFF) * factor;

        strip->setPixelColor(hexMap[i], strip->Color(r, g, b));
    }
}

void HexController::setGlobalBrightness(float b) {
    globalBrightness = constrain(b, 0.0f, 1.0f);
}

void HexController::show() {
    // Apply brightness scaling to all LEDs
    for (int i = 0; i < strip->numPixels(); i++) {
        uint32_t c = strip->getPixelColor(i);

        uint8_t r = ((c >> 16) & 0xFF) * globalBrightness;
        uint8_t g = ((c >>  8) & 0xFF) * globalBrightness;
        uint8_t b = ( c        & 0xFF) * globalBrightness;

        strip->setPixelColor(i, strip->Color(r, g, b));
    }

    strip->show();
}
