#pragma once
// ghostmoon UI Catalog: KnobStylesEncoder — Endless encoder knob styles E1-E4
#include "KnobStyles.h"

namespace gm {
namespace knobs {

// E1. Heavy Industrial — massive knurl teeth, deep shadow
void drawEncoderIndustrialKnob(juce::Graphics &g, int x, int y, int width,
                                       int height, float sliderPos,
                                       float rotaryStartAngle, float rotaryEndAngle,
                                       juce::Slider &slider,
                                       juce::Colour accent = juce::Colour(0xffd0d0d8));


// E2. LED Trail — afterglow ring showing position history
void drawEncoderLedTrailKnob(juce::Graphics &g, int x, int y, int width,
                                     int height, float sliderPos,
                                     float rotaryStartAngle, float rotaryEndAngle,
                                     juce::Slider &slider,
                                     juce::Colour accent = juce::Colour(0xff00ccff));


// E3. Sleek Dial — thin modern profile, minimal, single accent line
void drawEncoderSleekKnob(juce::Graphics &g, int x, int y, int width,
                                  int height, float sliderPos,
                                  float rotaryStartAngle, float rotaryEndAngle,
                                  juce::Slider &slider,
                                  juce::Colour accent = juce::Colour(0xffe0e0e8));


// E4. Radar Encoder — scope display with sweeping beam (full 360)
void drawEncoderRadarKnob(juce::Graphics &g, int x, int y, int width,
                                  int height, float sliderPos,
                                  float rotaryStartAngle, float rotaryEndAngle,
                                  juce::Slider &slider,
                                  juce::Colour accent = juce::Colour(0xff22cc44));


} // namespace knobs
} // namespace gm