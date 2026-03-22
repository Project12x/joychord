#pragma once
// ghostmoon UI Catalog: KnobStylesBipolar — Bipolar knob styles B1-B4
#include "KnobStyles.h"

namespace gm {
namespace knobs {

// Shared: compute midpoint angle (12 o'clock)
float bipolarMidAngle(float startA, float endA);


// B1. Bipolar OLED Band — digital display band, lit from center outward
void drawBipolarOledKnob(juce::Graphics &g, int x, int y, int width,
                                 int height, float sliderPos,
                                 float rotaryStartAngle, float rotaryEndAngle,
                                 juce::Slider &slider,
                                 juce::Colour accent = juce::Colour(0xff22ddff));


// B2. Bipolar Ivory — light porcelain body, warm/cool split arc
void drawBipolarIvoryKnob(juce::Graphics &g, int x, int y, int width,
                                  int height, float sliderPos,
                                  float rotaryStartAngle, float rotaryEndAngle,
                                  juce::Slider &slider,
                                  juce::Colour accent = juce::Colour(0xff4488aa));


// B3. Bipolar Neon Arc — futuristic per-segment bloom from center
void drawBipolarNeonKnob(juce::Graphics &g, int x, int y, int width,
                                 int height, float sliderPos,
                                 float rotaryStartAngle, float rotaryEndAngle,
                                 juce::Slider &slider,
                                 juce::Colour accent = juce::Colour(0xffff44cc));


// B4. Bipolar Gauge — VU-style +/- meter with scale marks
void drawBipolarGaugeKnob(juce::Graphics &g, int x, int y, int width,
                                  int height, float sliderPos,
                                  float rotaryStartAngle, float rotaryEndAngle,
                                  juce::Slider &slider,
                                  juce::Colour accent = juce::Colour(0xff44dd88));


} // namespace knobs
} // namespace gm