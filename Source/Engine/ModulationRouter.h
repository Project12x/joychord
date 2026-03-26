#pragma once
#include <functional>
#include <string>
#include <atomic>

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
    WahPosition,    // Wah pedal sweep
    PitchShiftAudio,// Audio Pitch Shifter (SignalSmith)
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
        case ModTarget::WahPosition:     return "Wah Position";
        case ModTarget::PitchShiftAudio: return "Audio Pitch Shift";
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
using ParamModCallback  = std::function<void(const char* paramId, float value)>;

// ── Router ──────────────────────────────────────────────────────────────────
class ModulationRouter
{
public:
    ModulationRouter();

    void setCallbacks (MidiCCCallback ccCb, PitchBendCallback bendCb, ParamModCallback paramCb = nullptr);

    // Axis target assignment
    void      setAxisTarget (ControlAxis axis, ModTarget target);
    ModTarget getAxisTarget (ControlAxis axis) const;

    // Pop one pending cleanup CC from the queue. Returns true if one was available.
    // Call in a loop at the top of processBlock to drain all queued cleanup CCs.
    bool popPendingCC (int& cc, int& value);

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

    // Lock-free queue for cleanup CC messages queued by setAxisTarget (UI thread)
    // and flushed into the midi buffer by processBlock (audio thread)
    struct PendingCC { int cc; int value; };

    static constexpr int kPendingQueueSize = 16;
    // Simple bounded lock-free SPSC queue using atomics
    PendingCC  pendingQueue[kPendingQueueSize];
    std::atomic<int> pendingHead{0};
    std::atomic<int> pendingTail{0};

    struct MidiQueue {
        PendingCC buf[16];
        std::atomic<int> head{0}, tail{0};
        bool push (PendingCC v) {
            int h = head.load (std::memory_order_relaxed);
            int next = (h + 1) & 15;
            if (next == tail.load (std::memory_order_acquire)) return false;
            buf[h] = v; head.store (next, std::memory_order_release); return true;
        }
        bool pop (PendingCC& v) {
            int t = tail.load (std::memory_order_relaxed);
            if (t == head.load (std::memory_order_acquire)) return false;
            v = buf[t]; tail.store ((t + 1) & 15, std::memory_order_release); return true;
        }
    } pendingCCs;
};
