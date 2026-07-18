# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

ESP32 firmware (PlatformIO, Arduino framework) for a light sculpture of 7 hexagonal LED panels in a flower layout — a center hex surrounded by a ring of 6, 24 WS2812 LEDs each (168 total), two buttons, two pots, and a microphone for audio-reactive modes. Target board: Arduino Nano ESP32.

Sister project: **LED-HEXWAVE-2.0-Linear** (9-hex linear layout). The two repos are structurally identical except for `include/board_config.h`, `lib/hexcore/src/hex_geometry.*`, and their mode sets — keep architectural changes portable between them.

## Commands

```bash
pio run                                            # compile firmware
pio run -t upload                                  # flash the board
pio test -e native                                 # run hexcore unit tests on this computer (no hardware needed)
pio test -e arduino_nano_esp32 -f test_hardware    # flash the manual visual test harness to the board
```

There is no linter. The only tests runnable without hardware are the native Unity tests in `test/test_native/test_core/test_main.cpp`, which cover the `lib/hexcore` library.

## Architecture

The codebase is split into two layers:

1. **`lib/hexcore/`** — Arduino-free pure logic: colour math, the mode-selection state machine (`mode_state`), hex geometry (LED index tables, `HEX7_SIDE_TO_HEX` adjacency, `VU_ORDER`), and path math. No `Arduino.h` includes allowed here; this is what makes it natively unit-testable (`pio test -e native` builds it with the host compiler under `-std=gnu++17`).

2. **`include/` + `src/`** — Arduino-facing firmware. All hardware config (pins, LED count, brightness, frame pacing) lives in `include/board_config.h` and nowhere else.

Key firmware pieces:

- **`src/main.cpp`** — the ~60-line loop: pace frame (`FRAME_INTERVAL_MS`, 0 = unpaced here), `modeEngine.update()`, apply right-pot brightness (with `BRIGHTNESS_FLOOR`), run `audioUpdate()` once per frame when in audio mode, optionally `strip.clear()` per the mode's `autoClear` flag, dispatch the mode, `strip.show()`.
- **`src/mode_registry.cpp`** — two function-pointer tables (`audioModes`, `nonAudioModes`) of `{ fn, autoClear, name }`. The table sizes ARE the mode counts; button cycling and EEPROM validation derive from them automatically.
- **`ModeEngine`** (`include/input.h`, `src/input.cpp`) — buttons (left short press = cycle mode, left long press ≥600 ms = reset, right = toggle audio group), IIR-smoothed pots (α = 0.1), and EEPROM persistence. The actual state transitions and EEPROM-byte validation are pure functions in `hexcore/mode_state` (bytes: 0 = audio flag, 1 = audio mode index, 2 = non-audio mode index).
- **Audio engine** (`src/audio_engine.cpp`) — timed 8 kHz sampling, 256-sample FFT, bands cached per frame (bass < 300 Hz, mid < 1100 Hz, treble above); modes read the cached getters, never sample themselves. Normalization ceilings `BASS_MAX`/`MID_MAX`/`TREB_MAX` are hardware-tuned constants.
- **Modes** — one header in `include/modes/<group>/` + one .cpp in `src/modes/<group>/`, each a `void mode_x()` drawing one frame using static locals for its own timing/state. Modes read `modeEngine.leftPot()` for their per-mode parameter.

### Adding a mode

1. Create `include/modes/<group>/mode_myMode.h` declaring `void mode_myMode();`
2. Create `src/modes/<group>/mode_myMode.cpp` implementing it
3. Add one `{ mode_myMode, autoClear, "My Mode" }` line to the table in `src/mode_registry.cpp`

On this fixture every mode repaints all its pixels each frame, so all entries use `autoClear = true`.

### Test-harness quirk

`platformio.ini` sets `test_build_src = yes`, so the on-hardware harness (`test/test_hardware/main.cpp`) links against all of `src/` — `main.cpp`'s `setup()/loop()` are compiled out under `#ifndef PIO_UNIT_TESTING`. Any new file defining `setup()`/`loop()` needs the same guard.
