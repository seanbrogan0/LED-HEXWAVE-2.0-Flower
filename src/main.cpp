// /src/main.cpp
// =========================================================
// MAIN ENTRY POINT
// Delegates to test_all_systems.cpp
// =========================================================

#include <Arduino.h>

void test_setup();
void test_loop();

void setup() {
    test_setup();
}

void loop() {
    test_loop();
}