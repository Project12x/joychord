# Joychord -- Roadmap

## Phase 0: Planning  [COMPLETE]

- Architecture and feature spec
- Standard project documentation

## Phase 1: Project Scaffold  [COMPLETE]  (v0.1.0)

- JUCE 8 CMake project (Standalone + VST3)
- All stub source files (ChordEngine, StrumEngine, ButtonRoleMap, ModulationRouter, XInputGamepad)
- GoogleTest harness -- 4 stub tests passing
- spdlog integration
- sfizz declared but gated (`if(TARGET sfizz::sfizz)`) due to MSVC CRT mismatch

## Phase 2: Chord Engine

- Integrate **mahler.c** as a real FetchContent dependency (replaces `vendor/mahler.h` placeholder)
- `ChordEngine`: key + scale model using mahler.c `mah_get_scale()` / `mah_get_chord()`
- Diatonic chord resolution (scale degrees I-vii)
- Borrowed chord support (parallel minor via `mah_get_scale()` on parallel mode)
- `Inversion(n)` support via `mah_invert_chord()`
- `Extension` types: 7th, sus4, sus2, add9
- Voicing: close position + drop-2
- **Dijkstra voicing scorer** -- enumerate inversions, score by total semitone distance from previous chord, pick minimum-movement voicing
- **Reverse chord identification** via `mah_return_chord()` -- powers the UI chord name display
- Full unit test coverage with mutation verification

## Phase 2.5: Playable Checkpoint (v0.2.0)

Minimal end-to-end wiring to get a playable instrument. Goal: hold a controller, press buttons, hear chords in a DAW.

- **XInput polling** -- implement real `XInputGamepad::poll()` on a background thread (~100Hz)
- **Hardcoded button map** -- face buttons A/B/X/Y = degrees I/IV/V/vi, no modifier system yet
- **Basic JUCE synth** -- `juce::Synthesiser` with 12 `SimpleVoice` (sine + ADSR). Hear chords directly from the Standalone, no DAW needed.
- **MIDI output** -- ChordEngine result -> `juce::MidiBuffer` note-on/off (also works in DAW)
- **No strum timing** -- instant chord changes (StrumEngine deferred)
- **No modulation** -- analog sticks ignored for now
- **No UI** -- just the default JUCE window with chord name label
- Tag as v0.2.0 when controller plays chords through speakers

## Phase 3: Button Role System

- `ButtonRole` variant type (already stubbed)
- `ButtonRoleMap`: physical button -> role mapping
- Modifier bitmask accumulation (LB, RB, LB+RB)
- Default "Diatonic Rock" preset (already stubbed)
- Preset serialization (APVTS / JSON via ValueTree)

## Phase 4: Strum & Articulation Engine

- `StrumEngine`: hold -> sustain, trigger -> strum
- Legato slide: **common-tone retention** on chord change (uses Dijkstra voicing scorer output)
- StrumDown / StrumUp with note spread timing (~10ms/note)
- Analog trigger -> velocity mapping

## Phase 5: Modulation Router

- L-stick X -> pitch bend (14-bit)
- L-stick Y -> CC11 expression
- R-stick X -> CC74 filter
- R-stick Y -> CC91 reverb
- Deadzone filtering with EMA smoothing

## Phase 6: Internal Synth (sfizz)

- Resolve sfizz MSVC CRT mismatch (link as shared DLL or patch abseil CMakeLists)
- Polyphonic sfizz SFZ player
- Bundle basic SFZ instruments (piano, guitar, strings, bass)
- R-stick filter routed to sfizz CC

## Phase 7: MIDI Output (Full)

- All strum/legato events as MIDI note-on/off
- All modulation as MIDI CC + pitch bend
- MIDI channel selection
- Standalone: loopMIDI-compatible virtual port

## Phase 8: UI

- Controller layout visualization (Xbox button map)
- Key wheel / scale selector
- Role assignment per button (drag or dropdown)
- **Chord name display** -- real-time `Cmaj7/E` powered by mahler.c `mah_return_chord()`
- Output mode toggle (Internal Synth / MIDI Out)
- melatonin_blur for glassmorphic panels

## Future / Post-v1

- Rhythmic strum patterns (predefined + custom timing grids)
- Velocity humanization (random +/-offset per note)
- Chord sets / song sections (A/B/C banks switchable mid-performance)
- MIDI capture (record MIDI output to file)
- Keys Lock mode (melody over held chord)
- Smart Suggest mode (highlight next-chord recommendations, informed by ChordSeqAI research)
- SDL2 backend (PS/Switch controller support)
- macOS / AU support
- **Poompatoom shared engine** -- extract common harmony/voicing layer when both projects mature

---

## Research & Reference Library

Libraries and projects studied during planning. Not integrated but inform design decisions.

| Library | What We Take From It |
| ------- | -------------------- |
| [Teoria.js](https://github.com/saebekassebil/teoria) (MIT) | API design reference for ChordEngine interface. Chord symbol parser grammar for UI display. |
| [optimal-voice-leading](https://github.com/willdickerson/optimal-voice-leading) (MIT) | Dijkstra algorithm for minimum-movement voicing selection. Porting concept, not code. |
| [ChordSeqAI](https://github.com/PetrIvan/chord-seq-ai-app) | Chord encoding vocabulary. Neural progression suggestion for future "Smart Suggest" feature. |
| [music-gen](https://github.com/austonst/music-gen) (MIT) | Hierarchical repetition model (Motif->Theme->Piece). Relevant if adding auto-accompaniment. |
| [Poompatoom](../../../Poompatoom) | 3-tier musicality constraints, gestural expression mapping. Will share code when both projects mature. |
