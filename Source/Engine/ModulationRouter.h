#pragma once
#include <functional>

using MidiCCCallback   = std::function<void(int cc, int value)>;
using PitchBendCallback= std::function<void(int bendValue)>; // 0-16383, 8192=center

class ModulationRouter
{
public:
    ModulationRouter() = default;

    void setCallbacks (MidiCCCallback ccCb, PitchBendCallback bendCb);

    // Call with normalized axis values [-1, 1]; applies deadzone + EMA smoothing
    void processLeftStick  (float x, float y);
    void processRightStick (float x, float y);

    // Analog triggers [0, 1]; maps to assignable CC
    void processTriggers (float lt, float rt);

    void setDeadzone       (float dz) { deadzone = dz; }
    void setPitchBendRange (int semitones) { pitchBendRange = semitones; }
    void setSmoothing      (float alpha) { emaAlpha = alpha; } // 0.0=frozen, 1.0=instant

private:
    MidiCCCallback    ccCallback;
    PitchBendCallback bendCallback;

    float deadzone      = 0.15f;
    int   pitchBendRange = 2;
    float emaAlpha      = 0.3f; // EMA smoothing factor (lower = smoother)

    // EMA state for each axis
    float smoothLX = 0.f, smoothLY = 0.f;
    float smoothRX = 0.f, smoothRY = 0.f;
    float smoothLT = 0.f, smoothRT = 0.f;

    // Previous output values (to suppress duplicate MIDI events)
    int prevBend     = 8192;
    int prevCC11     = 64;
    int prevCC74     = 64;
    int prevCC91     = 64;
    int prevCClt     = 0;
    int prevCCrt     = 0;

    float applyDeadzone (float v) const;
    int   normalizedToCC (float v) const;       // [-1,1] -> [0,127]
    int   normalizedToUnipolarCC (float v) const; // [0,1] -> [0,127]
    float ema (float prev, float target) const;
};
