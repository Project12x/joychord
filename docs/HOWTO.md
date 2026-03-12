# Joychord — How To

## Prerequisites

- Windows 10/11
- Visual Studio 2022 (with C++ Desktop workload)
- CMake 3.22+
- JUCE 8 (set `JUCE_DIR` or use FetchContent — see CMakeLists.txt)
- Xbox controller (XInput-compatible)

## Build

```powershell
# Configure
cmake -B build -G "Visual Studio 17 2022" -A x64

# Build (Debug)
cmake --build build --config Debug

# Build (Release)
cmake --build build --config Release
```

Build outputs:
- Standalone: `build/Joychord_artefacts/Standalone/Joychord.exe`
- VST3: `build/Joychord_artefacts/VST3/Joychord.vst3`

## Run Tests

```powershell
ctest --test-dir build --output-on-failure -C Debug
```

## Install VST3

Copy `Joychord.vst3` to:
```
C:\Program Files\Common Files\VST3\
```

Then rescan plugins in your DAW.

## MIDI Output (DAW routing)

In standalone mode, Joychord creates a virtual MIDI output port named `Joychord`.
Use [loopMIDI](https://www.tobias-erichsen.de/software/loopmidi.html) if your DAW doesn't see virtual ports directly.

## Controller Setup

Plug in your Xbox controller before launching. Joychord auto-detects the first XInput device.
Hot-plug is supported — reconnect without restarting.

## Common Tasks

### Change Key
Hold `Start` → use D-pad to cycle through keys displayed in the UI.

### Change Scale
Hold `Start` → use face buttons to select scale (major, minor, dorian, mixolydian...).

### Reassign a Button Role
Click the button in the UI's controller diagram → select role from the dropdown.

### Save / Load Preset
Use the preset bar at the top of the plugin window. Presets are stored as APVTS state.
