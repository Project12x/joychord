# Joychord — Architecture

## Overview

Joychord is a JUCE 8 plugin (VST3 + Standalone) with six main subsystems:

```
[XInput Gamepad]
       |
       v
 [ButtonRoleMap]  <-- user-assignable roles per button
       |
       +---> [ChordEngine]      --> note set (MIDI pitches + velocities)
       |           |
       |     [StrumEngine]      --> articulate / legato / timing spread
       |
       +---> [ModulationRouter] --> axis-to-param mapping with floor modulation
       
       both feed into:
       
 [PluginProcessor]
       |
       +---> [GhostmoonVoice / TinySoundFont]  --> audio synthesis
       +---> [9-Stage Effects Chain]            --> filter, comp, chorus, flanger, phaser, delay, reverb, shimmer, dither
       +---> MIDI Output (DAW / loopMIDI)
```

## Subsystems

### Input Layer (`Source/Input/`)
- `GamepadInput.h` — abstract interface, polled on a background thread (~100Hz)
- `XInputGamepad` — Windows XInput backend; reports button states (pressed/held/released) and analog axes normalized to [-1, 1]
- Background thread posts `GamepadEvent` structs to a lock-free FIFO consumed by `ButtonRoleMap`

### ButtonRoleMap (`Source/Engine/ButtonRoleMap`)
- Maps each physical button ID to a `ButtonRole` variant
- Role types: `Chord`, `Borrowed`, `Chromatic`, `Inversion`, `Extension`, `OctaveShift`, `KeyTranspose`, `StrumDown`, `StrumUp`, `Mute`
- Modifier buttons (LB, RB) are accumulated as a bitmask before resolving chord + extension
- Preset serialization: JSON via JUCE `ValueTree` / APVTS

### ChordEngine (`Source/Engine/ChordEngine`)
- Maintains global key + scale selection
- Resolves `Chord(degree)` -> set of MIDI note numbers using mahler.c interval math
- Applies inversions, extensions (7th, sus4, sus2, add9), drop-2 voicing
- **Dijkstra voicing scorer**: minimum-movement voicing selection on chord change
- **Reverse chord identification**: powers UI chord name display (`Cmaj7/E`)
- All math is pure (no side effects); easily unit-tested without JUCE

### StrumEngine (`Source/Engine/StrumEngine`)
- Tracks currently held chord + previously held chord
- Legato: common-tone retention on chord change
- Strum: staggered note timing (~10ms/note) with analog velocity

### ModulationRouter (`Source/Engine/ModulationRouter`)
- **6 axes**: LStickX, LStickY, RStickX, RStickY, LT, RT
- **9 targets**: FilterCutoff, ReverbMix, ChorusDepth, DelayMix, WahPosition, Volume, Expression, PitchShift, None
- **Floor modulation**: knob sets minimum value, axis sweeps from floor to range maximum
- `ParamSmoother` on all RT parameters, `snapTo()` on prepare for startup seeding
- Lock-free CC cleanup via `popPendingCC` API
- Deadzone filtering (+/-0.1) + EMA smoothing

### Synth Engine (`Source/Synth/`)
- `GhostmoonVoice` — PolyBLEP oscillator (9 waveshapes), UnisonEngine (up to 16 voices), AHDSR envelope, SubOscillator, Portamento, DriftModulator
- `TinySoundFont` — SF2/SFZ sample playback engine (built-in piano)
- Selectable in UI: Synth mode vs Sample mode

### Effects Chain (`PluginProcessor::processBlock`)

9-stage per-sample chain with ghostmoon DSP:

1. Filter (MoogLadder) — cutoff, resonance (capped at 1.3x to prevent self-oscillation)
2. Compressor
3. Chorus
4. Flanger
5. Phaser
6. Delay
7. Reverb
8. Shimmer Reverb
9. Dither

Each section has an enable toggle and dedicated APVTS parameters. 30 total effect parameters.

### UI (`Source/Plugin/`)
- `PluginEditor` — DarkMetallicTheme (ghostmoon), textured background with vignette, logo with premultiplied-alpha edge dissolve
- `EffectsDrawer` — scrollable Viewport with all 9 effect sections in chain order
- `SynthDrawer` — Oscillator, Unison, Sub Osc, Envelope, Portamento, Drift sections
- Controls drawer — axis assignment dropdowns for all 6 controller axes
- Xbox diamond button indicators, chord name display, connection status

## Threading

| Thread | Responsibility |
|--------|---------------|
| Background (XInput poll) | Read controller state, push `GamepadEvent` to FIFO |
| Message thread | UI updates, role map edits |
| Audio thread | Consume FIFO, run StrumEngine, emit MIDI, drive synth + effects |

No allocations on the audio thread; FIFO is lock-free. All modulation uses `std::atomic` + `ParamSmoother`.

## Data Flow

```
XInput poll -> GamepadEvent FIFO -> ButtonRoleMap -> ChordEngine/StrumEngine
-> MidiBuffer -> PluginProcessor::processBlock -> Synth -> Effects Chain -> Audio Out
                                                       \-> host MIDI output
```

## Dependencies

| Library | Use | License | Integration |
|---------|-----|---------|-------------|
| JUCE 8 | Framework, MIDI, synth, UI | GPL / Commercial | FetchContent |
| ghostmoon | DSP effects + UI catalog | Internal | add_subdirectory |
| XInput (Windows SDK) | Gamepad input | Royalty-free | System lib |
| mahler.c | Chord/interval/scale math | MIT | FetchContent |
| TinySoundFont | SF2/SFZ playback | MIT | Header-only |
| nlohmann/json | Preset serialization | MIT | FetchContent |
| spdlog | Logging | MIT | FetchContent |
| GoogleTest | Unit tests | BSD-3 | FetchContent |
| signalsmith-stretch | Pitch shifting | MIT | FetchContent |
