#pragma once
// ghostmoon UI Catalog: KnobStylesStepped — Stepped/detented knob styles S1-S4
#include "KnobStyles.h"

namespace gm {
namespace knobs {

// S1. Stepped Knurled — mode selector with knurl teeth and detent ticks
void drawSteppedKnurledKnob(juce::Graphics &g, int x, int y, int width,
                                    int height, float sliderPos,
                                    float rotaryStartAngle, float rotaryEndAngle,
                                    juce::Slider &slider,
                                    juce::Colour accent = juce::Colour(0xff00cccc),
                                    int numSteps = 8);


// S2. Stepped Carbon Fiber — waveform selector, segment highlights
void drawSteppedCarbonKnob(juce::Graphics &g, int x, int y, int width,
                                   int height, float sliderPos,
                                   float rotaryStartAngle, float rotaryEndAngle,
                                   juce::Slider &slider,
                                   juce::Colour accent = juce::Colour(0xffee6622),
                                   int numSteps = 6);


// S3. Rotary Switch — bakelite body with chicken-head pointer and numbers
void drawRotarySwitchKnob(juce::Graphics &g, int x, int y, int width,
                                  int height, float sliderPos,
                                  float rotaryStartAngle, float rotaryEndAngle,
                                  juce::Slider &slider,
                                  juce::Colour accent = juce::Colour(0xffddcc88),
                                  int numSteps = 5);


// S4. Stepped LED Ring — dark body with individual LED dots
void drawSteppedLedRingKnob(juce::Graphics &g, int x, int y, int width,
                                    int height, float sliderPos,
                                    float rotaryStartAngle, float rotaryEndAngle,
                                    juce::Slider &slider,
                                    juce::Colour accent = juce::Colour(0xff00ddff),
                                    int numSteps = 12);


} // namespace knobs
} // namespace gm