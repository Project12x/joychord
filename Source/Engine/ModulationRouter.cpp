#include "ModulationRouter.h"
#include <cmath>
#include <algorithm>

void ModulationRouter::setCallbacks (MidiCCCallback ccCb, PitchBendCallback bendCb)
{
    ccCallback   = std::move (ccCb);
    bendCallback = std::move (bendCb);
}

void ModulationRouter::processLeftStick (float x, float y)
{
    // L-stick X → pitch bend
    float dx = applyDeadzone (x);
    if (bendCallback)
    {
        // Scale to ±pitchBendRange semitones, map to 0-16383
        float normalized = dx; // [-1, 1]
        int bend = static_cast<int> (8192.0f + normalized * 8191.0f);
        bend = std::clamp (bend, 0, 16383);
        bendCallback (bend);
    }

    // L-stick Y → CC11 (expression)
    float dy = applyDeadzone (-y); // invert so up = more expression
    if (ccCallback)
        ccCallback (11, normalizedToCC (dy));
}

void ModulationRouter::processRightStick (float x, float y)
{
    // R-stick X → CC74 (filter cutoff)
    float dx = applyDeadzone (x);
    if (ccCallback)
        ccCallback (74, normalizedToCC (dx));

    // R-stick Y → CC91 (reverb send)
    float dy = applyDeadzone (-y);
    if (ccCallback)
        ccCallback (91, normalizedToCC (dy));
}

float ModulationRouter::applyDeadzone (float v) const
{
    if (std::abs (v) < deadzone) return 0.0f;
    float sign = v > 0.0f ? 1.0f : -1.0f;
    return sign * (std::abs (v) - deadzone) / (1.0f - deadzone);
}

int ModulationRouter::normalizedToCC (float v) const
{
    // [-1, 1] → [0, 127], center at 64
    int cc = static_cast<int> (64.0f + v * 63.0f);
    return std::clamp (cc, 0, 127);
}
