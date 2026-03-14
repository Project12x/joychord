#include "ModulationRouter.h"
#include <cmath>
#include <algorithm>

void ModulationRouter::setCallbacks (MidiCCCallback ccCb, PitchBendCallback bendCb)
{
    ccCallback   = std::move (ccCb);
    bendCallback = std::move (bendCb);
}

void ModulationRouter::processLeftStick (float x, float /*y*/)
{
    // L-stick X -> pitch bend (14-bit) only.
    // L-stick Y is intentionally unused to avoid crosstalk artifacts.
    float dx = applyDeadzone (x);
    smoothLX = ema (smoothLX, dx);

    if (bendCallback)
    {
        int bend = static_cast<int> (8192.0f + smoothLX * 8191.0f);
        bend = std::clamp (bend, 0, 16383);

        if (bend != prevBend)
        {
            bendCallback (bend);
            prevBend = bend;
        }
    }
}

void ModulationRouter::processRightStick (float x, float y)
{
    // R-stick X -> CC74 (filter cutoff)
    float dx = applyDeadzone (x);
    smoothRX = ema (smoothRX, dx);

    if (ccCallback)
    {
        int val = normalizedToCC (smoothRX);
        if (val != prevCC74)
        {
            ccCallback (74, val);
            prevCC74 = val;
        }
    }

    // R-stick Y -> CC91 (reverb send)
    float dy = applyDeadzone (-y);
    smoothRY = ema (smoothRY, dy);

    if (ccCallback)
    {
        int val = normalizedToCC (smoothRY);
        if (val != prevCC91)
        {
            ccCallback (91, val);
            prevCC91 = val;
        }
    }
}

void ModulationRouter::processTriggers (float lt, float rt)
{
    // LT -> CC1 (mod wheel), RT -> CC2 (breath controller / assignable)
    smoothLT = ema (smoothLT, lt);
    smoothRT = ema (smoothRT, rt);

    if (ccCallback)
    {
        int valLT = normalizedToUnipolarCC (smoothLT);
        if (valLT != prevCClt)
        {
            ccCallback (1, valLT); // CC1 = mod wheel
            prevCClt = valLT;
        }

        int valRT = normalizedToUnipolarCC (smoothRT);
        if (valRT != prevCCrt)
        {
            ccCallback (11, valRT); // CC11 = expression (volume swell)
            prevCCrt = valRT;
        }
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
    // [-1, 1] -> [0, 127], center at 64
    int cc = static_cast<int> (64.0f + v * 63.0f);
    return std::clamp (cc, 0, 127);
}

int ModulationRouter::normalizedToUnipolarCC (float v) const
{
    // [0, 1] -> [0, 127]
    int cc = static_cast<int> (v * 127.0f);
    return std::clamp (cc, 0, 127);
}

float ModulationRouter::ema (float prev, float target) const
{
    return prev + emaAlpha * (target - prev);
}
