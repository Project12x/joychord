#pragma once
#include <functional>

using MidiCCCallback   = std::function<void(int cc, int value)>;
using PitchBendCallback= std::function<void(int bendValue)>; // 0-16383, 8192=center

class ModulationRouter
{
public:
    ModulationRouter() = default;

    void setCallbacks (MidiCCCallback ccCb, PitchBendCallback bendCb);

    // Call with normalized axis values [-1, 1]; applies deadzone internally
    void processLeftStick  (float x, float y);
    void processRightStick (float x, float y);

    void setDeadzone       (float dz) { deadzone = dz; }
    void setPitchBendRange (int semitones) { pitchBendRange = semitones; }

private:
    MidiCCCallback    ccCallback;
    PitchBendCallback bendCallback;

    float deadzone      = 0.1f;
    int   pitchBendRange = 2;

    float applyDeadzone (float v) const;
    int   normalizedToCC (float v) const; // [-1,1] → [0,127]
};
