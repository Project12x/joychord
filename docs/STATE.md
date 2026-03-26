# Joychord — State

## Current Phase

**Phase 4+: Ghostmoon Integration & Effects** — complete. Full synth engine, 9-stage effects chain, modulation router, UI polish, and controller axis mapping all functional.

## Build Status

| Target | Status |
|--------|--------|
| Standalone (Debug) | PASS — `build/Joychord_artefacts/Debug/Standalone/Joychord.exe` |
| VST3 (Debug) | Builds (same sources as Standalone) |
| JoychordTests | PASS — 13/13 tests |

## Test Metrics

13 tests, 13 passing, 0 failing.

### Test List (ChordEngineTests.cpp)
- `ChordEngine.MajorScaleIntervalsCorrect` — PASS
- `ChordEngine.KeyTransposeCorrect` — PASS
- `ChordEngine.SeventhExtensionAddsNote` — PASS
- `ChordEngine.BorrowedChordFromParallelMinor` — PASS
- `ChordEngine.Sus4ExtensionCorrect` — PASS
- `ChordEngine.InversionCorrect` — PASS
- `ChordEngine.Drop2VoicingCorrect` — PASS
- `ChordEngine.DijkstraVoicingCorrect` — PASS
- `ChordEngine.ChordNamingCorrect` — PASS
- `ChordEngine.ChromaticChordCorrect` — PASS
- `ChordEngine.AllDegreesValid` — PASS
- Plus 2 additional validation tests — PASS

## Key Classes

| Class | File | Responsibility |
|-------|------|----------------|
| `ChordEngine` | `Source/Engine/ChordEngine` | Key/scale/voicing math, Dijkstra scorer |
| `StrumEngine` | `Source/Engine/StrumEngine` | Articulation, legato slide, common-tone retention |
| `ButtonRoleMap` | `Source/Engine/ButtonRoleMap` | Assignable role -> action mapping, preset save/load |
| `ModulationRouter` | `Source/Engine/ModulationRouter` | Stick/trigger -> MIDI CC with floor modulation |
| `GhostmoonVoice` | `Source/Synth/GhostmoonVoice` | PolyBLEP synth (9 waveshapes, unison, AHDSR) |
| `TinySoundFont` | `Source/Synth/TinySoundFont` | SF2/SFZ sample playback engine |
| `XInputGamepad` | `Source/Input/XInputGamepad` | Windows XInput backend |
| `PluginProcessor` | `Source/Plugin/PluginProcessor` | JUCE AudioProcessor, APVTS, 9-stage effects chain |
| `PluginEditor` | `Source/Plugin/PluginEditor` | DarkMetallic UI with drawers, logo, controller viz |
| `EffectsDrawer` | `Source/Plugin/EffectsDrawer` | Scrollable effects chain UI (9 sections) |

## Known Issues

- sfizz integration skipped on MSVC (abseil-cpp CRT mismatch). TinySoundFont used as fallback.
- IDE lint errors in PluginEditor.cpp are false positives from JUCE unity build system.

## Last Updated

2026-03-26
