# Joychord — State

## Current Phase

**Phase 0: Planning** — architecture and feature spec complete, project scaffold not yet started.

## Build Status

| Target | Status |
|--------|--------|
| Standalone | Not built |
| VST3 | Not built |
| Tests | Not written |

## Test Metrics

No tests yet.

## Key Classes (planned)

| Class | File | Responsibility |
|-------|------|----------------|
| `ChordEngine` | `Source/Engine/ChordEngine` | Key/scale/voicing math using Mahler.c |
| `StrumEngine` | `Source/Engine/StrumEngine` | Articulation, legato slide, common-tone retention |
| `ButtonRoleMap` | `Source/Engine/ButtonRoleMap` | Assignable role → action mapping, preset save/load |
| `ModulationRouter` | `Source/Engine/ModulationRouter` | Stick/trigger → pitch bend / MIDI CC |
| `GamepadInput` | `Source/Input/GamepadInput.h` | Abstract gamepad interface |
| `XInputGamepad` | `Source/Input/XInputGamepad` | Windows XInput backend |
| `PluginProcessor` | `Source/Plugin/PluginProcessor` | JUCE AudioProcessor, MIDI output |
| `PluginEditor` | `Source/Plugin/PluginEditor` | UI, controller layout visualization |

## Known Issues

None yet.

## Last Updated

2026-03-12
