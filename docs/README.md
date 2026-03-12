# Joychord

A JUCE 8 standalone application and VST3 plugin that turns an Xbox-style gamepad into a polyphonic chord instrument.

## Features

- **8 chord slots** mapped to face buttons and D-pad, key-relative (diatonic + borrowed chords)
- **Assignable button roles** — every button is user-configurable (chord, inversion, extension, strum, mute, octave shift...)
- **Strum + hold** — chord buttons sustain; analog triggers articulate with velocity
- **StrumDown / StrumUp** — RT and LT spread notes low-to-high or high-to-low with analog velocity
- **Legato slide** — overlap chord buttons for smooth common-tone voice leading
- **Analog modulation** — sticks control pitch bend, expression, filter, reverb
- **Dual output** — internal polyphonic synth (standalone) + MIDI out (DAW routing)
- **XInput native** — low-latency Xbox controller support on Windows

## Status

Early development. See [STATE.md](STATE.md).

## Quick Start

See [HOWTO.md](HOWTO.md) for build instructions.

## Target Platform

- Windows 10/11
- Xbox / standard XInput-compatible controllers
- VST3 host or standalone use
