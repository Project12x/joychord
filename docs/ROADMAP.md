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

## Phase 3: Button Role System + Presets  [COMPLETE]

- `ButtonRole` variant dispatch in processBlock (replaces hardcoded map)
- `ButtonRoleMap`: physical button -> role mapping with modifier bitmask (LB, RB, LB+RB)
- D-pad roles: key transpose (left/right), octave shift (up/down)
- Start/Back/L3/R3 assignments (panic, mode toggle, preset cycle)
- Factory presets: Diatonic Rock, Pop Ballad, Jazz Voicings
- Preset dropdown in UI

## Phase 4: Ghostmoon Integration  [COMPLETE]  (v0.3.0)

- ghostmoon_dsp: SafetyLimiter, CpuMeter, MeterSource
- ghostmoon_ui: DarkMetallicTheme, LEDMeter, Typography, Knob (LED Ladder style)
- Master volume knob with dB-to-linear gain
- CMake library migration (add_subdirectory, no more copy-paste)
- Upgraded to ghostmoon v0.5.1 (default DarkMetallicTheme, Typography delegation)
- JoychordTheme: Neon ComboBox styling + themed popup menus
- First-pass UI polish: title glow, section dividers, gamepad button glow, Typography fonts

---

## Phase 5: Effects Chain  [COMPLETE]  (v0.4.0)

9-stage per-sample effects chain with ghostmoon DSP:

- Filter -> Compressor -> Chorus -> Flanger -> Phaser -> Delay -> Reverb -> Shimmer -> Dither
- 30 new APVTS parameters with enable toggles
- Scrollable EffectsDrawer UI (juce::Viewport) with all 9 sections
- SYN button with mutual exclusion (FX/SYN drawers)

## Phase 6: Analog Stick Expression  [COMPLETE]

Controller axis-to-parameter modulation with floor control:

- **6 assignable axes**: LStickX, LStickY, RStickX, RStickY, LT, RT
- **9 modulation targets**: FilterCutoff, ReverbMix, ChorusDepth, DelayMix, WahPosition, Volume, Expression, PitchShift, None
- **Floor modulation**: UI knob sets minimum value, axis sweeps from floor to maximum
- EMA smoothing + deadzone filtering on all axes
- Auto-enable effects on axis assignment
- Lock-free CC cleanup via `popPendingCC` API
- Defaults: LStickX=PitchBend, LT=ModWheel; all others unassigned

## Phase 7: Nashville Number System + Expanded Chord Vocabulary

Bring chord vocabulary to HiChord-level (882+ voicings).

- Nashville Number System as primary chord model (1-7 degrees)
- Full quality palette: major, minor, dim, aug, dom7, maj7, min7, dim7, 6, 9, 11, 13
- Slash chords (inversions as bass note specification)
- Modal interchange (borrow from parallel minor/major freely)
- Secondary dominants (V/V, V/vi, etc.)
- Chord display showing Nashville number + quality + root note

## Phase 8: Synth Engine (Ghostmoon-Powered)  [COMPLETE]

GhostmoonVoice synth engine replacing SimpleVoice:

- `gm::PolyBLEPOscillator` via UnisonEngine (9 waveshapes, up to 16 unison voices)
- `gm::AHDSREnvelope` — per-voice amplitude envelope
- SubOscillator, Portamento, DriftModulator
- 13 synth APVTS params with per-voice dispatch
- SynthDrawer UI with Oscillator, Unison, Sub Osc, Envelope, Portamento, Drift sections
- TinySoundFont SF2/SFZ playback as alternative sound source (built-in piano)
- Waveform selector in UI

## Phase 9: Strum & Articulation Engine  [NEXT]

- `StrumEngine`: hold -> sustain, trigger -> strum
- StrumDown / StrumUp with per-note spread timing (~10ms/note)
- Legato slide: common-tone retention on chord change
- Analog trigger -> velocity mapping
- Rhythmic strum patterns (predefined timing grids)

## Phase 10: Arpeggiator

- Pattern modes: up, down, up-down, random, as-played
- Tempo-synced or free-running rate
- Gate length control
- Octave range (1-4 octaves)
- Hold mode (arp continues after button release)

## Phase 11: MIDI Output (Full)

- All strum/legato events as MIDI note-on/off
- All modulation as MIDI CC + pitch bend
- MIDI channel selection
- Standalone: loopMIDI-compatible virtual port

## Phase 12: UI (Full)

- Interactive Xbox controller visualization (buttons + sticks + triggers)
- Key wheel / scale selector
- Role assignment per button (drag or dropdown)
- Piano roll / note visualizer showing sounding MIDI notes
- Output mode toggle (Internal Synth / MIDI Out / SFZ)
- melatonin_blur for glassmorphic panels
- Keyboard fallback (QWERTY -> degrees for testing without controller)
- Latency display

---

## Future / Post-v1

- Velocity humanization (random +/- offset per note)
- Chord sets / song sections (A/B/C banks switchable mid-performance)
- MIDI capture (record MIDI output to file)
- Keys Lock mode (melody over held chord)
- Auto-bass accompaniment (Omnichord-inspired)
- Drum patterns / rhythm mode
- Smart Suggest mode (next-chord recommendations, informed by ChordSeqAI research)
- SDL2 backend (PS/Switch controller support)
- macOS / AU support
- **Poompatoom shared engine** -- extract common harmony/voicing layer when both projects mature

---

## Inspirations

| Source | What We Take |
| ------ | ------------ |
| [HiChord](https://hichord.shop) | Nashville Number System, joystick chord modification, 882-chord vocabulary, built-in effects chain |
| [Omnichord](https://suzukimusic-global.com) | Strumplate concept, auto-accompaniment, accessible chord buttons |
| [Orba](https://artiphon.com) | Gesture-driven expression, compact instrument philosophy |

## Research & Reference Library

| Library | What We Take From It |
| ------- | -------------------- |
| [Teoria.js](https://github.com/saebekassebil/teoria) (MIT) | API design reference for ChordEngine interface. Chord symbol parser grammar for UI display. |
| [optimal-voice-leading](https://github.com/willdickerson/optimal-voice-leading) (MIT) | Dijkstra algorithm for minimum-movement voicing selection. Porting concept, not code. |
| [ChordSeqAI](https://github.com/PetrIvan/chord-seq-ai-app) | Chord encoding vocabulary. Neural progression suggestion for future "Smart Suggest" feature. |
| [music-gen](https://github.com/austonst/music-gen) (MIT) | Hierarchical repetition model (Motif->Theme->Piece). Relevant if adding auto-accompaniment. |
| [Poompatoom](../../../Poompatoom) | 3-tier musicality constraints, gestural expression mapping. Will share code when both projects mature. |
| [ghostmoon](../../../ghostmoon) | DSP library (48 headers) + UI catalog (53 components). Synth core, effects, analysis, theming. |
