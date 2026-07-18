// test/test_native/test_core/test_main.cpp
// =========================================================
// Native unit tests for the Arduino-free hexcore library.
// Run with:  pio test -e native
// =========================================================

#include <unity.h>
#include "color_math.h"
#include "mode_state.h"
#include "hex_geometry.h"
#include "path_math.h"

void setUp() {}
void tearDown() {}

// =========================================================
// color_math
// =========================================================
void test_packColour_bit_layout() {
    TEST_ASSERT_EQUAL_HEX32(0x00FF8040u, packColour(0xFF, 0x80, 0x40));
    TEST_ASSERT_EQUAL_HEX32(0x00000000u, packColour(0, 0, 0));
    TEST_ASSERT_EQUAL_HEX32(0x00FFFFFFu, packColour(255, 255, 255));
}

void test_dimColour32_factors() {
    TEST_ASSERT_EQUAL_HEX32(0u, dimColour32(packColour(255, 255, 255), 0.0f));
    TEST_ASSERT_EQUAL_HEX32(packColour(255, 100, 0),
                            dimColour32(packColour(255, 100, 0), 1.0f));
    // Channels truncate, matching the modes' float→uint8 cast semantics
    TEST_ASSERT_EQUAL_HEX32(packColour(102, 40, 0),
                            dimColour32(packColour(255, 100, 0), 0.4f));
}

void test_lerpColour32_endpoints_and_clamping() {
    uint32_t a = packColour(10, 20, 30);
    uint32_t b = packColour(200, 100, 50);
    TEST_ASSERT_EQUAL_HEX32(a, lerpColour32(a, b, 0.0f));
    TEST_ASSERT_EQUAL_HEX32(b, lerpColour32(a, b, 1.0f));
    TEST_ASSERT_EQUAL_HEX32(a, lerpColour32(a, b, -5.0f));   // t clamps low
    TEST_ASSERT_EQUAL_HEX32(b, lerpColour32(a, b, 5.0f));    // t clamps high
    TEST_ASSERT_EQUAL_HEX32(packColour(127, 127, 127),
                            lerpColour32(0, packColour(255, 255, 255), 0.5f));
}

void test_clamp_helpers() {
    TEST_ASSERT_EQUAL(5, clampInt(5, 0, 10));
    TEST_ASSERT_EQUAL(0, clampInt(-3, 0, 10));
    TEST_ASSERT_EQUAL(10, clampInt(42, 0, 10));
    TEST_ASSERT_EQUAL_FLOAT(0.5f, clampFloat(0.5f, 0.0f, 1.0f));
    TEST_ASSERT_EQUAL_FLOAT(0.0f, clampFloat(-1.0f, 0.0f, 1.0f));
    TEST_ASSERT_EQUAL_FLOAT(1.0f, clampFloat(9.0f, 0.0f, 1.0f));
}

void test_wrapIndex() {
    TEST_ASSERT_EQUAL(5, wrapIndex(-1, 6));
    TEST_ASSERT_EQUAL(0, wrapIndex(6, 6));
    TEST_ASSERT_EQUAL(3, wrapIndex(3, 6));
}

// =========================================================
// mode_state
// =========================================================
static const ModeCounts COUNTS = { 5, 5 };

void test_cycle_wraps_and_only_touches_active_group() {
    ModeState s = { true, 4, 2 };
    modeStateCycle(s, COUNTS);
    TEST_ASSERT_EQUAL(0, s.audioMode);      // wrapped at count
    TEST_ASSERT_EQUAL(2, s.nonAudioMode);   // untouched

    s = { false, 1, 4 };
    modeStateCycle(s, COUNTS);
    TEST_ASSERT_EQUAL(1, s.audioMode);      // untouched
    TEST_ASSERT_EQUAL(0, s.nonAudioMode);   // wrapped at count
}

void test_toggle_flips_flag_only() {
    ModeState s = { false, 2, 1 };
    modeStateToggleAudio(s);
    TEST_ASSERT_TRUE(s.audioEnabled);
    TEST_ASSERT_EQUAL(2, s.audioMode);
    TEST_ASSERT_EQUAL(1, s.nonAudioMode);
    modeStateToggleAudio(s);
    TEST_ASSERT_FALSE(s.audioEnabled);
}

void test_reset_preserves_audioEnabled() {
    ModeState s = { true, 3, 2 };
    modeStateReset(s);
    TEST_ASSERT_TRUE(s.audioEnabled);
    TEST_ASSERT_EQUAL(0, s.audioMode);
    TEST_ASSERT_EQUAL(0, s.nonAudioMode);
}

void test_fromBytes_validates_garbage() {
    // Garbage EEPROM (e.g. factory 0xFF) must land on safe defaults
    ModeState s = modeStateFromBytes(255, 255, 7, COUNTS);
    TEST_ASSERT_FALSE(s.audioEnabled);      // only exactly 1 means enabled
    TEST_ASSERT_EQUAL(0, s.audioMode);
    TEST_ASSERT_EQUAL(0, s.nonAudioMode);
}

void test_fromBytes_keeps_valid_state() {
    ModeState s = modeStateFromBytes(1, 4, 2, COUNTS);
    TEST_ASSERT_TRUE(s.audioEnabled);
    TEST_ASSERT_EQUAL(4, s.audioMode);
    TEST_ASSERT_EQUAL(2, s.nonAudioMode);
}

// =========================================================
// hex_geometry (data integrity for THIS fixture)
// =========================================================
void test_hexes_cover_every_led_exactly_once() {
    const int TOTAL = NUM_HEXES * HEX_LED_COUNT;
    bool seen[NUM_HEXES * HEX_LED_COUNT] = { false };

    for (int h = 0; h < NUM_HEXES; h++) {
        for (int i = 0; i < HEX_LED_COUNT; i++) {
            int idx = HEXES[h][i];
            TEST_ASSERT_TRUE_MESSAGE(idx >= 0 && idx < TOTAL, "LED index out of range");
            TEST_ASSERT_FALSE_MESSAGE(seen[idx], "LED index appears twice");
            seen[idx] = true;
        }
    }
    for (int i = 0; i < TOTAL; i++) {
        TEST_ASSERT_TRUE_MESSAGE(seen[i], "LED not covered by any hex");
    }
}

void test_side_to_hex_is_permutation_of_outer_ring() {
    bool used[7] = { false };
    for (int s = 0; s < 6; s++) {
        int h = HEX7_SIDE_TO_HEX[s];
        TEST_ASSERT_TRUE_MESSAGE(h >= 1 && h <= 6, "side maps outside outer ring");
        TEST_ASSERT_FALSE_MESSAGE(used[h], "side maps to a hex twice");
        used[h] = true;
    }
}

void test_vu_order_is_permutation_starting_at_center() {
    TEST_ASSERT_EQUAL(CENTER_HEX, VU_ORDER[0]);   // fills from the center out
    bool used[NUM_HEXES] = { false };
    for (int i = 0; i < NUM_HEXES; i++) {
        int h = VU_ORDER[i];
        TEST_ASSERT_TRUE_MESSAGE(h >= 0 && h < NUM_HEXES, "VU entry out of range");
        TEST_ASSERT_FALSE_MESSAGE(used[h], "VU entry repeated");
        used[h] = true;
    }
}

void test_center_side_slices_stay_inside_hex() {
    // Rotate mode paints side s as logical LEDs s*4 .. s*4+3 of HEX7
    for (int s = 0; s < 6; s++) {
        TEST_ASSERT_TRUE(s * 4 + 3 < HEX_LED_COUNT);
    }
}

// =========================================================
// path_math (generic — this fixture defines no built-in paths)
// =========================================================
static const int TEST_PATH[] = { 1, 2, 3, 2, 4, 5, 6, 7 };
static const int TEST_PATH_LEN = 8;

void test_pathPosToLED_hex_boundaries() {
    TEST_ASSERT_EQUAL(HEX1[0],
        pathPosToLED(0, TEST_PATH, TEST_PATH_LEN, HEXES, HEX_LED_COUNT));
    TEST_ASSERT_EQUAL(HEX1[HEX_LED_COUNT - 1],
        pathPosToLED(HEX_LED_COUNT - 1, TEST_PATH, TEST_PATH_LEN, HEXES, HEX_LED_COUNT));
    TEST_ASSERT_EQUAL(HEX2[0],
        pathPosToLED(HEX_LED_COUNT, TEST_PATH, TEST_PATH_LEN, HEXES, HEX_LED_COUNT));
    // The test path revisits HEX2 at slot 3
    TEST_ASSERT_EQUAL(HEX2[0],
        pathPosToLED(3 * HEX_LED_COUNT, TEST_PATH, TEST_PATH_LEN, HEXES, HEX_LED_COUNT));
}

void test_wrapPathPos_seam() {
    const int TOTAL = TEST_PATH_LEN * HEX_LED_COUNT;
    TEST_ASSERT_EQUAL(TOTAL - 1, wrapPathPos(-1, TOTAL));
    TEST_ASSERT_EQUAL(0, wrapPathPos(TOTAL, TOTAL));
    TEST_ASSERT_EQUAL(5, wrapPathPos(5, TOTAL));
    TEST_ASSERT_EQUAL(TOTAL - 10, wrapPathPos(0 - 10, TOTAL));
}

// =========================================================
// RUNNER
// =========================================================
int main(int, char**) {
    UNITY_BEGIN();

    RUN_TEST(test_packColour_bit_layout);
    RUN_TEST(test_dimColour32_factors);
    RUN_TEST(test_lerpColour32_endpoints_and_clamping);
    RUN_TEST(test_clamp_helpers);
    RUN_TEST(test_wrapIndex);

    RUN_TEST(test_cycle_wraps_and_only_touches_active_group);
    RUN_TEST(test_toggle_flips_flag_only);
    RUN_TEST(test_reset_preserves_audioEnabled);
    RUN_TEST(test_fromBytes_validates_garbage);
    RUN_TEST(test_fromBytes_keeps_valid_state);

    RUN_TEST(test_hexes_cover_every_led_exactly_once);
    RUN_TEST(test_side_to_hex_is_permutation_of_outer_ring);
    RUN_TEST(test_vu_order_is_permutation_starting_at_center);
    RUN_TEST(test_center_side_slices_stay_inside_hex);

    RUN_TEST(test_pathPosToLED_hex_boundaries);
    RUN_TEST(test_wrapPathPos_seam);

    return UNITY_END();
}
