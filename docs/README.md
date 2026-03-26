# Joychord

A JUCE 8 standalone application and VST3 plugin that turns an Xbox-style gamepad into a polyphonic chord instrument.

## Features

- **8 chord slots** mapped to face buttons and D-pad, key-relative (diatonic + borrowed chords)
- **Assignable button roles** — every button is user-configurable (chord, inversion, extension, strum, mute, octave shift...)
- **Legato slide** — overlap chord buttons for smooth common-tone voice leading
- **Multiple sound sources** — built-in polyphonic synth (PolyBLEP oscillator, 9 waveshapes, unison) plus **SFZ/SF2 sample playback** via TinySoundFont/sfizz; includes a built-in piano for instant use
- **Analog modulation** — assignable controller axes with floor/depth control:
  - **Left Stick** — pitch bend (X), configurable (Y)
  - **LT** — mod wheel (default); reassignable to any modulation target
  - **RT** — unassigned by default; assign to filter, reverb, wah, chorus, delay, expression, or pitch shift
  - **Right Stick** — freely assignable (both axes)
  - Effect knobs set the **floor** — the axis sweeps from the knob value up to maximum
- **Dual output** — internal polyphonic synth (standalone) + MIDI out (DAW routing)
- **XInput native** — low-latency Xbox controller support on Windows

## Controller Axes

| Axis | Default | Reassignable |
|------|---------|--------------|
| Left Stick X | Pitch Bend | Yes |
| Left Stick Y | None | Yes |
| Right Stick X | None | Yes |
| Right Stick Y | None | Yes |
| LT | Mod Wheel | Yes |
| RT | None | Yes |

Axis targets are configurable in the **Controls** drawer. Assigning an axis to an effect automatically enables that effect.

## Status

Early development. See [STATE.md](STATE.md).

## Quick Start

See [HOWTO.md](HOWTO.md) for build instructions.

## Target Platform

- Windows 10/11
- Xbox / standard XInput-compatible controllers
- VST3 host or standalone use
