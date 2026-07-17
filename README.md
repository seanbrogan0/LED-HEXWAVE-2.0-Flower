# LED HexWave 2.0 — Flower (7-hex fixture)

ESP32 firmware for a light sculpture of **7 hexagonal LED panels in a flower layout** —
a center hex (HEX7) surrounded by a ring of 6 — with 24 WS2812 LEDs each (168 total),
two buttons, two pots, and a microphone for audio-reactive modes.

> Sister project: **LED-HEXWAVE-2.0-Linear** — the same architecture driving a
> 9-hex linear layout. The two repos are structurally identical except for
> `include/board_config.h`, `lib/hexcore/src/hex_geometry.*`, and their mode sets.

## Hardware

| Item | Value |
|---|---|
| Board | Arduino Nano ESP32 (`arduino_nano_esp32`) |
| LEDs | 168 × WS2812 (GRB, 800 kHz), data on pin **17** |
| Left button | pin **4** (to GND, internal pull-up) |
| Right button | pin **5** (to GND, internal pull-up) |
| Left pot | **A3** |
| Right pot | **A2** |
| Microphone | **A0** |

This fixture has no audio-mode indicator LED (the sister repo's `PIN_LED_IND`
is simply not defined in `board_config.h`, and the indicator code compiles out).

## Controls

- **Left button** — short press: next mode in the active group; long press (≥ 600 ms):
  reset both groups to mode 0.
- **Right button** — toggle between audio and non-audio mode groups.
- **Left pot** — per-mode parameter (speed, colour, density…).
- **Right pot** — global brightness, with a 2% floor so the strip never looks dead.

The active group and both mode indices persist in EEPROM across power cycles and
are validated against the mode registry on boot, so stale or garbage values can
never select a nonexistent mode.

## Modes

| # | Non-audio | Left pot |
|---|---|---|
| 0 | **Rotate** — 6 colours march anti-clockwise around the center hex's sides, mirrored onto the adjacent outer hexes | speed |
| 1 | **Breathe** — all hexes pulse one palette colour on a ~4 s sine | colour |
| 2 | **Chase** — one lit hex steps around all 7; colour advances each lap | speed |
| 3 | **Ripple** — pulse expands center → ring → off, next colour | speed |
| 4 | **Sparkle** — random sparks decay at 0.80/frame in a private buffer | density |

| # | Audio | Behaviour |
|---|---|---|
| 0 | **Bass Pulse** — all hexes glow with bass level (left pot = colour) | |
| 1 | **Spectrum** — center = bass (red), ring alternates mid (green) / treble (blue) | |
| 2 | **Beat Flash** — bass-spike flash decaying at 0.85/frame (left pot = colour) | |
| 3 | **Volume Ring** — energy fills 0–7 hexes VU-style, center first (left pot = colour) | |
| 4 | **Audio Ripple** — bass spike triggers a center → ring → fade ripple (left pot = colour) | |

## Architecture

```
include/
  board_config.h      all pins + per-fixture constants (the ONLY hardware config point)
  hardware.h          the NeoPixel strip + init
  input.h             ModeEngine: buttons, pots, EEPROM persistence
  audio_engine.h      timed 8 kHz FFT sampling + cached band getters
  palette.h utils.h   colours and LED helpers
  mode_registry.h     function-pointer mode tables (owns the mode counts)
  modes/**            one header per mode
lib/hexcore/          Arduino-free pure logic: colour math, mode-state machine,
                      hex geometry (incl. HEX7_SIDE_TO_HEX + VU_ORDER), path math —
                      natively unit-testable
src/                  implementations, one .cpp per mode under src/modes/**
test/test_native/     Unity tests for hexcore (run on your computer)
test/test_hardware/   manual visual harness (flash to the board)
```

The main loop is ~60 lines: pace the frame (`FRAME_INTERVAL_MS`, 0 = unpaced on
this fixture), update inputs, apply brightness, run `audioUpdate()` once per frame
when in audio mode, dispatch the current mode through the registry, show. Each
registry entry carries an `autoClear` flag deciding whether the loop wipes the
strip before the mode draws — on this fixture every mode repaints all its pixels,
so all entries are `true` (Sparkle keeps its own buffer and overwrites the clear).

### Adding a mode

1. Create `include/modes/<group>/mode_myMode.h` declaring `void mode_myMode();`
2. Create `src/modes/<group>/mode_myMode.cpp` implementing it
3. Add one `{ mode_myMode, autoClear, "My Mode" }` line to the table in
   `src/mode_registry.cpp`

Mode counts, button cycling, and EEPROM validation pick it up automatically.

### EEPROM layout

| Byte | Meaning |
|---|---|
| 0 | audio mode enabled (1 = yes) |
| 1 | current audio mode index |
| 2 | current non-audio mode index |

## Build & test

```bash
pio run                        # compile firmware
pio run -t upload              # flash the board
pio test -e native             # run hexcore unit tests on your computer
pio test -e arduino_nano_esp32 -f test_hardware   # flash the visual harness
```

## Behaviour notes

- Audio sampling is paced to a true 8 kHz (256 samples ≈ 32 ms per frame), so
  audio modes run at ~23 fps; retune `BASS_MAX`/`MID_MAX`/`TREB_MAX` in
  `src/audio_engine.cpp` on hardware if needed.
- Mode cycling fires on button **release**; a hold ≥ 600 ms is the reset gesture.
- Pots are IIR-smoothed (α = 0.1), adding a slight, deliberate lag to knob moves.
