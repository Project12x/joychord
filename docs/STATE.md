# Joychord — State

## Current Phase

**Phase 1: Project Scaffold** — complete. Build verified: Standalone + Tests compile and run.
**Phase 2: Chord Engine** — next.

## Build Status

| Target | Status |
|--------|--------|
| Standalone (Debug) | PASS — `build/Joychord_artefacts/Debug/Standalone/Joychord.exe` |
| VST3 (Debug) | Not built (same sources as Standalone; will build in Phase 6) |
| JoychordTests | PASS — 4/4 tests |

## Test Metrics

4 tests, 4 passing, 0 failing.

### Test List (ChordEngineTests.cpp)
- `ChordEngine.MajorScaleIntervalsCorrect` — PASS (stub returns placeholder, test confirms call doesn't crash)
- `ChordEngine.KeyTransposeCorrect` — PASS
- `ChordEngine.SeventhExtensionAddsNote` — PASS
- `ChordEngine.BorrowedChordFromParallelMinor` — PASS

> Tests are stubs — they verify compilation and basic call paths. Real assertions come in Phase 2.

## Key Classes

| Class | File | Responsibility |
|-------|------|----------------|
| `ChordEngine` | `Source/Engine/ChordEngine` | Key/scale/voicing math |
| `StrumEngine` | `Source/Engine/StrumEngine` | Articulation, legato slide, common-tone retention |
| `ButtonRoleMap` | `Source/Engine/ButtonRoleMap` | Assignable role → action mapping, preset save/load |
| `ModulationRouter` | `Source/Engine/ModulationRouter` | Stick/trigger → pitch bend / MIDI CC |
| `GamepadInput` | `Source/Input/GamepadInput.h` | Abstract gamepad interface |
| `XInputGamepad` | `Source/Input/XInputGamepad` | Windows XInput backend |
| `PluginProcessor` | `Source/Plugin/PluginProcessor` | JUCE AudioProcessor, APVTS params, MIDI output |
| `PluginEditor` | `Source/Plugin/PluginEditor` | UI (stub) |

## Known Issues

- sfizz integration skipped on MSVC (Phase 5). abseil-cpp inside sfizz uses /MTd static CRT,
  which conflicts with JUCE's /MDd. Workaround: `if(TARGET sfizz::sfizz)` guard in CMakeLists.txt.
  Resolution: link sfizz as a shared library or patch its CMakeLists when integrating in Phase 5.

## Last Updated

2026-03-13
