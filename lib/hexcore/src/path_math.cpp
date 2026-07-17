// hexcore/path_math.cpp

#include "path_math.h"

int wrapPathPos(int pos, int totalPathLen) {
    if (pos < 0) return pos + totalPathLen;
    if (pos >= totalPathLen) return pos - totalPathLen;
    return pos;
}

int pathPosToLED(int pathPos, const int* path, int pathLen,
                 const uint16_t* const* hexes, int hexSize) {
    int hexIndex = path[(pathPos / hexSize) % pathLen] - 1;
    int ledIndex = pathPos % hexSize;
    return hexes[hexIndex][ledIndex];
}
