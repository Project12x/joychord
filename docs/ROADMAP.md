# Joychord — Roadmap

## Phase 0: Planning ✅
- Architecture and feature spec
- Standard project documentation

## Phase 1: Project Scaffold
- JUCE 8 CMake project (Standalone + VST3)
- XInput gamepad polling on background thread
- Lock-free FIFO between input and audio threads
- GoogleTest harness
- spdlog integration
- All standard docs in place

## Phase 2: Chord Engine
- `ChordEngine`: key + scale model, Mahler.c integration
- Diatonic chord resolution (scale degrees I–vii)
- Borrowed chord support (parallel minor)
- `Inversion(n)` support
- `Extension` types: 7th, sus4, sus2, add9
- Voicing: close position + drop-2
- Full unit test coverage with mutation verification

## Phase 3: Button Role System
- `ButtonRole` variant type
- `ButtonRoleMap`: physical button → role mapping
- Modifier bitmask accumulation (LB, RB, LB+RB)
- Default "Diatonic Rock" preset
- Preset serialization (APVTS / JSON)

## Phase 4: Strum & Articulation Engine
- `StrumEngine`: hold → sustain, trigger → strum
- Legato slide: common-tone retention on chord change
- StrumDown / StrumUp with note spread timing (~10ms/note)
- Analog trigger → velocity mapping

## Phase 5: Modulation Router
- L-stick X → pitch bend (14-bit)
- L-stick Y → CC11 expression
- R-stick X → CC74 filter
- R-stick Y → CC91 reverb
- Deadzone filtering

## Phase 6: Internal Synth
- Polyphonic JUCE `Synthesiser` (12 voices)
- Simple wavetable voice (piano-adjacent)
- Amp envelope tuned for chord/legato feel
- R-stick filter routed to internal synth

## Phase 7: MIDI Output
- `MidiOutput` from `PluginProcessor`
- All strum/legato events as MIDI note-on/off
- All modulation as MIDI CC + pitch bend
- MIDI channel selection
- Standalone: loopMIDI-compatible virtual port

## Phase 8: UI
- Controller layout visualization (Xbox button map)
- Key wheel / scale selector
- Role assignment per button (drag or dropdown)
- Chord slot display (current chord name)
- Output mode toggle (Internal Synth / MIDI Out)

## Future / Post-v1
- Rhythmic strum patterns (predefined + custom timing grids)
- Velocity humanization (random ±offset per note)
- Chord sets / song sections (A/B/C banks switchable mid-performance)
- MIDI capture (record MIDI output to file)
- Keys Lock mode (melody over held chord)
- SDL2 backend (PS controller support)
- macOS / AU support
