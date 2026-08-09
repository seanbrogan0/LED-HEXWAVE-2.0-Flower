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
| Right button | pin **7** (to GND, internal pull-up) |
| Audio indicator LED | pin **6** (lit whenever an audio mode group is active) |
| Left pot | **A3** |
| Right pot | **A2** |
| Microphone | **A0** |

## Controls

- **Left button** — short press: next mode in the active group; long press (≥ 600 ms):
  reset both groups to mode 0.
- **Right button** — toggle between audio and non-audio mode groups.
- **Left pot** — non-audio modes: per-mode parameter (speed, colour, density…);
  audio modes: mic sensitivity (0.25×–4× gain, 1× at centre).
- **Right pot** — global brightness, with a 2% floor so the strip never looks dead.

The active group and both mode indices persist in EEPROM across power cycles and
are validated against the mode registry on boot, so stale or garbage values can
never select a nonexistent mode.

## Modes

| # | Non-audio | Left pot |
|---|---|---|
| 0 | **Rotate** — 6 colours march anti-clockwise around the center hex's sides, mirrored onto the adjacent outer hexes, crossfading continuously between colours | speed |
| 1 | **Breathe** — all hexes pulse on a smooth sine; each breath brings the next palette colour (swapped at the dark point) | breath speed |
| 2 | **Chase** — a soft pulse orbits the outer ring; the center hex's wall adjacent to the lit cell glows in sync; colour crossfades into the next over each lap | speed |
| 3 | **Ripple** — pulse expands smoothly center → ring → fade with continuous brightness ramps, next colour each cycle | speed |
| 4 | **Sparkle** — random sparks decay at 0.80/frame in a private buffer | density |
| 5 | **Hex Fade Random** — one hex at a time cross-fades to a new random palette colour (prioritising black hexes first); others hold their colour | fade speed |
| 6 | **Dual Ring Comets** — two comets orbit the outer ring (`HEX_PATH_RING`) with a glowing head and exponential tail; center hex glows a steady blend of both comet colours | speed |
| 7 | **Solid** — whole fixture one flat colour; sweeps hue 0–340° then desaturates to white at the top of the pot's travel | hue / white |
| 8 | **Rainbow Flow** — one continuous hue cycle scrolls around the outer-ring orbit; center hex spins its own miniature synced rainbow wheel | scroll speed |

In every audio mode the **left pot is mic sensitivity** (0.25× at the bottom,
1× stock at centre, 4× at the top — applied in the audio engine, so all modes
respond uniformly).

| # | Audio | Behaviour / colour |
|---|---|---|
| 0 | **Bass Pulse** — all hexes glow with bass level; colour drifts slowly around the hue wheel (~20 s per lap) |
| 1 | **Spectrum** — colours are the spectrum itself: center = bass (red), ring alternates mid (green) / treble (blue) |
| 2 | **Beat Flash** — bass-spike flash decaying at 0.85/frame; every beat flashes a new random palette colour |
| 3 | **Volume Ring** — energy fills 0–7 hexes VU-style, center first; classic VU gradient green → yellow → red |
| 4 | **Audio Ripple** — bass spike triggers a center → ring → fade ripple; each ripple fires in a new random palette colour |
| 5 | **Impact Sparks** — transient-driven sparks in random palette colours decay (~0.5 s) toward a dim purple background; spark rate tracks smoothed band energy |
| 6 | **Bass vs Treble** — bass drives the center hex, treble drives the outer ring; bass hue drifts slowly (~64 s per lap), treble always takes the complementary hue |

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
                      hex geometry (incl. HEX7_SIDE_TO_HEX + VU_ORDER + the
                      HEX_PATH_RING outer-ring orbit), path math —
                      natively unit-testable
src/                  implementations, one .cpp per mode under src/modes/**
test/test_native/     Unity tests for hexcore (run on your computer)
test/test_hardware/   manual visual harness (flash to the board)
```

The main loop is ~60 lines: pace the frame (`FRAME_INTERVAL_MS`, 10 ms on this
fixture), update inputs, apply brightness, run `audioUpdate()` once per frame
when in audio mode, dispatch the current mode through the registry, show. Each
registry entry carries an `autoClear` flag deciding whether the loop wipes the
strip before the mode draws — most modes repaint every pixel they own each frame
and use `autoClear = true`; Dual Ring Comets and Impact Sparks rely on pixel
persistence for their trails and run with `autoClear = false`.

### Adding a mode

1. Create `include/modes/<group>/mode_myMode.h` declaring `void mode_myMode();`
2. Create `src/modes/<group>/mode_myMode.cpp` implementing it
3. Add one `{ mode_myMode, autoClear, "My Mode" }` line to the table in
   `src/mode_registry.cpp`

Mode counts, button cycling, and EEPROM validation pick it up automatically.
New entries must be appended, not inserted, so saved EEPROM mode indices stay valid.

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
