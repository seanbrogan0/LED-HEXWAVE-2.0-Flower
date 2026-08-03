// hexcore/color_math.cpp

#include "color_math.h"

uint32_t packColour(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

uint32_t dimColour32(uint32_t c, float factor) {
    uint8_t r = (uint8_t)(((c >> 16) & 0xFF) * factor);
    uint8_t g = (uint8_t)(((c >> 8)  & 0xFF) * factor);
    uint8_t b = (uint8_t)(( c        & 0xFF) * factor);
    return packColour(r, g, b);
}

uint32_t lerpColour32(uint32_t a, uint32_t b, float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    uint8_t ar = (a >> 16) & 0xFF;
    uint8_t ag = (a >> 8)  & 0xFF;
    uint8_t ab =  a        & 0xFF;

    uint8_t br = (b >> 16) & 0xFF;
    uint8_t bg = (b >> 8)  & 0xFF;
    uint8_t bb =  b        & 0xFF;

    uint8_t r  = ar + (int)((br - ar) * t);
    uint8_t g  = ag + (int)((bg - ag) * t);
    uint8_t bl = ab + (int)((bb - ab) * t);

    return packColour(r, g, bl);
}

uint32_t maxColour32(uint32_t a, uint32_t b) {
    uint8_t r = ((a >> 16) & 0xFF) > ((b >> 16) & 0xFF) ? (a >> 16) & 0xFF : (b >> 16) & 0xFF;
    uint8_t g = ((a >> 8)  & 0xFF) > ((b >> 8)  & 0xFF) ? (a >> 8)  & 0xFF : (b >> 8)  & 0xFF;
    uint8_t bl = ( a       & 0xFF) > ( b        & 0xFF) ?  a        & 0xFF :  b        & 0xFF;
    return packColour(r, g, bl);
}

int clampInt(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

float clampFloat(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

int wrapIndex(int i, int max) {
    if (i < 0) return max - 1;
    if (i >= max) return 0;
    return i;
}
