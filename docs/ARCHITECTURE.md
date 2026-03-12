# Joychord — Architecture

## Overview

Joychord is a JUCE 8 plugin (VST3 + Standalone) with four main subsystems:

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
       +---> [ModulationRouter] --> pitch bend, CC74, CC91, expression
       
       both feed into:
       
 [PluginProcessor]
       |
       +---> MIDI Output (DAW / loopMIDI)
       +---> Internal Synth (Standalone)
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
- Resolves `Chord(degree)` → set of MIDI note numbers using Mahler.c interval math
- Applies `Inversion(n)` to reorder the note set
- Applies `Extension(type)` to add/remove intervals
- Voicing options: close position, drop-2
- All math is pure (no side effects); easily unit-tested without JUCE

### StrumEngine (`Source/Engine/StrumEngine`)
- Tracks currently held chord + previously held chord
- On chord button press: compute new chord, detect common tones, emit note-offs only for non-common tones, note-ons for new tones (legato)
- On StrumDown/StrumUp role trigger: re-emit all notes of current chord with staggered timing (note spread ~10ms per note) and analog velocity
- On chord button release: emit note-off for all active voices

### ModulationRouter (`Source/Engine/ModulationRouter`)
- Polls analog stick and trigger axes from GamepadEvent
- Deadzone filtering (±0.1 by default)
- Maps L-stick X → pitch bend (14-bit MIDI)
- Maps L-stick Y → CC11 (expression)
- Maps R-stick X → CC74 (filter cutoff)
- Maps R-stick Y → CC91 (reverb send)

### Plugin Layer (`Source/Plugin/`)
- `PluginProcessor`: standard JUCE `AudioProcessor`; no audio processing in v1 (MIDI only from plugin perspective); drives internal synth in standalone
- `PluginEditor`: controller layout visualization, key/scale picker, role assignment UI, output mode selector

## Threading

| Thread | Responsibility |
|--------|---------------|
| Background (XInput poll) | Read controller state, push `GamepadEvent` to FIFO |
| Message thread | UI updates, role map edits |
| Audio thread | Consume FIFO, run StrumEngine, emit MIDI, drive synth |

No allocations on the audio thread; FIFO is lock-free.

## Data Flow (MIDI Out mode)

```
XInput poll → GamepadEvent FIFO → ButtonRoleMap → ChordEngine/StrumEngine
→ MidiBuffer → PluginProcessor::processBlock → host MIDI output
```

## Dependencies

| Library | Use | License |
|---------|-----|---------|
| JUCE 8 | Framework, MIDI, synth, UI | GPL / Commercial |
| XInput (Windows SDK) | Gamepad input | Royalty-free |
| Mahler.c (`vendor/mahler.h`) | Chord/interval math | MIT |
| spdlog | Logging | MIT |
| GoogleTest | Unit tests | BSD-3 |
| melatonin_blur | UI blur/glow effects | MIT |
