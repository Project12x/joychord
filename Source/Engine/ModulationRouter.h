#pragma once
#include <functional>
#include <string>

// ── Modulation target destinations ──────────────────────────────────────────
enum class ModTarget
{
    None = 0,
    PitchBend,
    FilterCutoff,
    FilterResonance,
    ReverbMix,
    ChorusDepth,
    DelayMix,
    Volume,
    ModWheel,       // CC1
    Expression,     // CC11
    Count
};

inline const char* modTargetName (ModTarget t)
{
    switch (t)
    {
        case ModTarget::None:            return "None";
        case ModTarget::PitchBend:       return "Pitch Bend";
        case ModTarget::FilterCutoff:    return "Filter Cutoff";
        case ModTarget::FilterResonance: return "Filter Resonance";
        case ModTarget::ReverbMix:       return "Reverb Mix";
        case ModTarget::ChorusDepth:     return "Chorus Depth";
        case ModTarget::DelayMix:        return "Delay Mix";
        case ModTarget::Volume:          return "Volume";
        case ModTarget::ModWheel:        return "Mod Wheel (CC1)";
        case ModTarget::Expression:      return "Expression (CC11)";
        default:                         return "Unknown";
    }
}

// ── Controller axes ─────────────────────────────────────────────────────────
enum class ControlAxis
{
    LStickX = 0,
    LStickY,
    RStickX,
    RStickY,
    LTrigger,
    RTrigger,
    Count
};

inline const char* controlAxisName (ControlAxis a)
{
    switch (a)
    {
        case ControlAxis::LStickX:  return "L-Stick X";
        case ControlAxis::LStickY:  return "L-Stick Y";
        case ControlAxis::RStickX:  return "R-Stick X";
        case ControlAxis::RStickY:  return "R-Stick Y";
        case ControlAxis::LTrigger: return "L-Trigger";
        case ControlAxis::RTrigger: return "R-Trigger";
        default:                    return "Unknown";
    }
}

// ── Callbacks ───────────────────────────────────────────────────────────────
using MidiCCCallback    = std::function<void(int cc, int value)>;
using PitchBendCallback = std::function<void(int bendValue)>; // 0-16383, 8192=center
using ParamModCallback  = std::function<void(const std::string& paramId, float value)>;

// ── Router ──────────────────────────────────────────────────────────────────
class ModulationRouter
{
public:
    ModulationRouter();

    void setCallbacks (MidiCCCallback ccCb, PitchBendCallback bendCb, ParamModCallback paramCb = nullptr);

    // Axis target assignment
    void      setAxisTarget (ControlAxis axis, ModTarget target);
    ModTarget getAxisTarget (ControlAxis axis) const;

    // Call with normalized axis values [-1, 1] or [0, 1] for triggers
    void processLeftStick  (float x, float y);
    void processRightStick (float x, float y);
    void processTriggers   (float lt, float rt);

    void setDeadzone       (float dz) { deadzone = dz; }
    void setPitchBendRange (int semitones) { pitchBendRange = semitones; }
    void setSmoothing      (float alpha) { emaAlpha = alpha; }

    static constexpr int kNumAxes = static_cast<int>(ControlAxis::Count);

private:
    MidiCCCallback    ccCallback;
    PitchBendCallback bendCallback;
    ParamModCallback  paramCallback;

    float deadzone       = 0.15f;
    int   pitchBendRange = 2;
    float emaAlpha       = 0.3f;

    // Per-axis config
    ModTarget axisTargets[kNumAxes] {};

    // EMA state per axis
    float smoothed[kNumAxes] {};

    // Previous output values per axis (suppress duplicate MIDI events)
    int prevOutput[kNumAxes] {};

    void  dispatchAxis (ControlAxis axis, float rawValue, bool bipolar);
    float applyDeadzone (float v) const;
    int   normalizedToCC (float v) const;       // [-1,1] -> [0,127]
    int   normalizedToUnipolarCC (float v) const; // [0,1] -> [0,127]
    float ema (float prev, float target) const;

    // Map ModTarget to APVTS parameter ID (for direct param modulation)
    static const char* targetToParamId (ModTarget t);
};
