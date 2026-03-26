#include "ModulationRouter.h"
#include <algorithm>
#include <cmath>

ModulationRouter::ModulationRouter()
{
    // Default axis assignments (matching previous hardcoded behavior)
    axisTargets[static_cast<int>(ControlAxis::LStickX)]  = ModTarget::PitchBend;
    axisTargets[static_cast<int>(ControlAxis::LStickY)]  = ModTarget::None;
    axisTargets[static_cast<int>(ControlAxis::RStickX)]  = ModTarget::FilterCutoff;
    axisTargets[static_cast<int>(ControlAxis::RStickY)]  = ModTarget::ReverbMix;
    axisTargets[static_cast<int>(ControlAxis::LTrigger)] = ModTarget::ModWheel;
    axisTargets[static_cast<int>(ControlAxis::RTrigger)] = ModTarget::Expression;

    for (int i = 0; i < kNumAxes; ++i)
    {
        smoothed[i]   = 0.0f;
        prevOutput[i] = -1; // force first dispatch
    }
}

void ModulationRouter::setCallbacks (MidiCCCallback ccCb, PitchBendCallback bendCb, ParamModCallback paramCb)
{
    ccCallback    = std::move (ccCb);
    bendCallback  = std::move (bendCb);
    paramCallback = std::move (paramCb);
}

void ModulationRouter::setAxisTarget (ControlAxis axis, ModTarget target)
{
    int idx = static_cast<int>(axis);
    if (idx >= 0 && idx < kNumAxes)
        axisTargets[idx] = target;
}

ModTarget ModulationRouter::getAxisTarget (ControlAxis axis) const
{
    int idx = static_cast<int>(axis);
    if (idx >= 0 && idx < kNumAxes)
        return axisTargets[idx];
    return ModTarget::None;
}

void ModulationRouter::processLeftStick (float x, float y)
{
    dispatchAxis (ControlAxis::LStickX, x, true);
    dispatchAxis (ControlAxis::LStickY, y, true);
}

void ModulationRouter::processRightStick (float x, float y)
{
    dispatchAxis (ControlAxis::RStickX, x, true);
    dispatchAxis (ControlAxis::RStickY, -y, true);  // invert Y (up = positive)
}

void ModulationRouter::processTriggers (float lt, float rt)
{
    dispatchAxis (ControlAxis::LTrigger, lt, false);
    dispatchAxis (ControlAxis::RTrigger, rt, false);
}

void ModulationRouter::dispatchAxis (ControlAxis axis, float rawValue, bool bipolar)
{
    int idx = static_cast<int>(axis);
    ModTarget target = axisTargets[idx];
    if (target == ModTarget::None)
        return;

    float processed = bipolar ? applyDeadzone (rawValue) : rawValue;
    smoothed[idx] = ema (smoothed[idx], processed);

    // Route to target
    switch (target)
    {
        case ModTarget::PitchBend:
        {
            if (bendCallback)
            {
                int bend = static_cast<int> (8192.0f + smoothed[idx] * 8191.0f);
                bend = std::clamp (bend, 0, 16383);
                if (bend != prevOutput[idx])
                {
                    bendCallback (bend);
                    prevOutput[idx] = bend;
                }
            }
            break;
        }

        case ModTarget::ModWheel:
        {
            if (ccCallback)
            {
                int val = bipolar ? normalizedToCC (smoothed[idx])
                                  : normalizedToUnipolarCC (smoothed[idx]);
                if (val != prevOutput[idx])
                {
                    ccCallback (1, val);
                    prevOutput[idx] = val;
                }
            }
            break;
        }

        case ModTarget::Expression:
        {
            if (ccCallback)
            {
                int val = bipolar ? normalizedToCC (smoothed[idx])
                                  : normalizedToUnipolarCC (smoothed[idx]);
                if (val != prevOutput[idx])
                {
                    ccCallback (11, val);
                    prevOutput[idx] = val;
                }
            }
            break;
        }

        // Direct parameter modulation targets
        case ModTarget::FilterCutoff:
        case ModTarget::FilterResonance:
        case ModTarget::ReverbMix:
        case ModTarget::ChorusDepth:
        case ModTarget::DelayMix:
        case ModTarget::Volume:
        {
            // Map to MIDI CC for external use
            if (ccCallback)
            {
                int ccNum = 74; // default
                switch (target)
                {
                    case ModTarget::FilterCutoff:    ccNum = 74; break;
                    case ModTarget::FilterResonance: ccNum = 71; break;
                    case ModTarget::ReverbMix:       ccNum = 91; break;
                    case ModTarget::ChorusDepth:     ccNum = 93; break;
                    case ModTarget::DelayMix:        ccNum = 12; break;
                    case ModTarget::Volume:          ccNum = 7;  break;
                    default: break;
                }
                int val = bipolar ? normalizedToCC (smoothed[idx])
                                  : normalizedToUnipolarCC (smoothed[idx]);
                if (val != prevOutput[idx])
                {
                    ccCallback (ccNum, val);
                    prevOutput[idx] = val;
                }
            }

            // Direct APVTS parameter modulation
            if (paramCallback)
            {
                const char* paramId = targetToParamId (target);
                if (paramId)
                {
                    // Normalize to [0, 1] for APVTS
                    float normalized = bipolar ? (smoothed[idx] + 1.0f) * 0.5f
                                               : smoothed[idx];
                    paramCallback (paramId, normalized);
                }
            }
            break;
        }

        default:
            break;
    }
}

float ModulationRouter::applyDeadzone (float v) const
{
    if (std::abs (v) < deadzone) return 0.0f;
    float sign = v > 0.0f ? 1.0f : -1.0f;
    return sign * (std::abs (v) - deadzone) / (1.0f - deadzone);
}

int ModulationRouter::normalizedToCC (float v) const
{
    int cc = static_cast<int> (64.0f + v * 63.0f);
    return std::clamp (cc, 0, 127);
}

int ModulationRouter::normalizedToUnipolarCC (float v) const
{
    int cc = static_cast<int> (v * 127.0f);
    return std::clamp (cc, 0, 127);
}

float ModulationRouter::ema (float prev, float target) const
{
    return prev + emaAlpha * (target - prev);
}

const char* ModulationRouter::targetToParamId (ModTarget t)
{
    switch (t)
    {
        case ModTarget::FilterCutoff:    return "filterCutoff";
        case ModTarget::FilterResonance: return "filterResonance";
        case ModTarget::ReverbMix:       return "reverbMix";
        case ModTarget::ChorusDepth:     return "chorusDepth";
        case ModTarget::DelayMix:        return "delayMix";
        case ModTarget::Volume:          return "masterVolume";
        case ModTarget::WahPosition:     return "wahPosition";
        default:                         return nullptr;
    }
}
