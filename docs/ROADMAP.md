# Joychord -- Roadmap

## Phase 0: Planning  [COMPLETE]

- Architecture and feature spec
- Standard project documentation

## Phase 1: Project Scaffold  [COMPLETE]  (v0.1.0)

- JUCE 8 CMake project (Standalone + VST3)
- All stub source files (ChordEngine, StrumEngine, ButtonRoleMap, ModulationRouter, XInputGamepad)
- GoogleTest harness
- spdlog integration
- sfizz declared but gated (`if(TARGET sfizz::sfizz)`) due to MSVC CRT mismatch

## Phase 2: Chord Engine  [COMPLETE]

- mahler.c integrated as FetchContent dependency
- `ChordEngine`: diatonic resolution (I-vii), key transpose, 7 scale modes
- Extensions: 7th, sus4, sus2, add9
- Inversions: explicit (0-3) or auto via Dijkstra voicing scorer
- Voicing: close position + drop-2
- Borrowed chords (parallel scale), chromatic chords
- Chord naming (note + quality + extension suffix)
- 13 unit tests passing

## Phase 2.5: Playable Checkpoint  [COMPLETE]  (v0.2.0)

- XInput polling at 100Hz via juce::Timer
- Hardcoded button map: A=I, B=V, X=IV, Y=vi
- `SimpleVoice`: sine wave + ADSR, 12-voice polyphony via juce::Synthesiser
- processBlock: gamepad -> ChordEngine (Dijkstra auto-voicing) -> MidiBuffer -> Synthesiser
- Basic UI: key/scale/voicing/octave dropdowns, gamepad index selector, chord name display, Xbox diamond button indicators, connection status

## Phase 3: Button Role System + Presets

- `ButtonRole` variant dispatch in processBlock (replaces hardcoded map)
- `ButtonRoleMap`: physical button -> role mapping with modifier bitmask (LB, RB, LB+RB)
- D-pad roles: key transpose (left/right), octave shift (up/down)
- Start/Back/L3/R3 assignments (panic, mode toggle, preset cycle)
- **Factory presets**: Diatonic Rock, Pop Ballad, Jazz Voicings
- **Preset system**: save/load user presets to JSON, preset dropdown in UI, import/export
- Preset serialization via ValueTree

## Phase 4: Internal Synth (sfizz)

- Resolve sfizz MSVC CRT mismatch (link as shared DLL or patch abseil)
- Polyphonic sfizz SFZ player
- Bundle basic SFZ instruments (piano, guitar, strings, bass)
- R-stick filter routed to sfizz CC

## Phase 5: Strum & Articulation Engine

- `StrumEngine`: hold -> sustain, trigger -> strum
- StrumDown / StrumUp with per-note spread timing (~10ms/note)
- Legato slide: common-tone retention on chord change (uses Dijkstra voicing scorer)
- Analog trigger -> velocity mapping
- Rhythmic strum patterns (predefined timing grids)

## Phase 6: Modulation Router

- L-stick X -> pitch bend (14-bit)
- L-stick Y -> CC11 expression
- R-stick X -> CC74 filter
- R-stick Y -> CC91 reverb
- Deadzone filtering with EMA smoothing
- Analog trigger -> assignable CC

## Phase 7: MIDI Output (Full)

- All strum/legato events as MIDI note-on/off
- All modulation as MIDI CC + pitch bend
- MIDI channel selection
- Standalone: loopMIDI-compatible virtual port

## Phase 8: UI (Full)

- Controller layout visualization (interactive Xbox button map)
- Key wheel / scale selector
- Role assignment per button (drag or dropdown)
- Real-time chord name display powered by mahler.c
- Piano roll / note visualizer showing sounding MIDI notes
- Output mode toggle (Internal Synth / MIDI Out)
- melatonin_blur for glassmorphic panels
- Keyboard fallback (QWERTY -> degrees for testing without controller)
- Latency display

## Future / Post-v1

- Velocity humanization (random +/- offset per note)
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
