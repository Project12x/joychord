// ghostmoon UI Catalog - KnobStylesEncoder.h implementations
// Auto-extracted from header-only to compiled library

#include "KnobStylesEncoder.h"

namespace gm {
namespace knobs {

void drawEncoderIndustrialKnob(juce::Graphics &g, int x, int y, int width,
                                       int height, float sliderPos,
                                       float rotaryStartAngle, float rotaryEndAngle,
                                       juce::Slider &slider,
                                       juce::Colour accent) {
  (void)rotaryStartAngle; (void)rotaryEndAngle; // encoder uses full 360
  auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto r = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX(), cy = bounds.getCentreY();
  float a = sliderPos * juce::MathConstants<float>::twoPi;
  bool isEnabled = slider.isEnabled();
  float al = isEnabled ? 1.0f : 0.35f;

  // Heavy drop shadow
  g.setColour(juce::Colours::black.withAlpha(0.45f * al));
  g.fillEllipse(cx - r + 1, cy - r + 3, r * 2.0f, r * 2.0f);

  // Dark metal body
  juce::ColourGradient bg(tintBody(juce::Colour(0xff4a4a54)), cx - r * 0.3f, cy - r * 0.35f,
                           tintBody(juce::Colour(0xff1a1a22)), cx + r, cy + r, true);
  bg.addColour(0.4, tintBody(juce::Colour(0xff363640)));
  g.setGradientFill(bg);
  g.fillEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);

  // Massive knurl teeth
  for (int i = 0; i < 30; ++i) {
    float ta = (float)i / 30.0f * juce::MathConstants<float>::twoPi;
    float inner = (i % 2 == 0) ? r - 6.0f : r - 3.0f;
    g.setColour((i % 2 == 0) ? tintBody(juce::Colour(0xff50505f)).withAlpha(0.7f * al)
                              : tintBody(juce::Colour(0xff1e1e26)).withAlpha(0.7f * al));
    g.drawLine(cx + inner * std::cos(ta), cy + inner * std::sin(ta),
               cx + r * std::cos(ta), cy + r * std::sin(ta), 2.0f);
  }
  g.setColour(tintBody(juce::Colour(0xff555560)).withAlpha(al));
  g.drawEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f, 1.5f);

  // Quad reference marks (N/E/S/W)
  for (int i = 0; i < 4; ++i) {
    float ta = (float)i / 4.0f * juce::MathConstants<float>::twoPi - juce::MathConstants<float>::halfPi;
    g.setColour(juce::Colours::white.withAlpha(0.2f * al));
    g.drawLine(cx + (r + 2.0f) * std::cos(ta), cy + (r + 2.0f) * std::sin(ta),
               cx + (r + 6.0f) * std::cos(ta), cy + (r + 6.0f) * std::sin(ta), 1.5f);
  }

  // Bright pointer notch
  auto pa = a - juce::MathConstants<float>::halfPi;
  g.setColour(juce::Colours::black.withAlpha(0.4f * al));
  g.drawLine(cx + r * 0.2f * std::cos(pa), cy + r * 0.2f * std::sin(pa),
             cx + (r - 6.0f) * std::cos(pa), cy + (r - 6.0f) * std::sin(pa), 3.0f);
  g.setColour(accent.withAlpha(al));
  g.drawLine(cx + r * 0.2f * std::cos(pa), cy + r * 0.2f * std::sin(pa),
             cx + (r - 6.0f) * std::cos(pa), cy + (r - 6.0f) * std::sin(pa), 1.5f);

  // Center cap
  g.setColour(tintBody(juce::Colour(0xff252530)).withAlpha(al));
  g.fillEllipse(cx - 4, cy - 4, 8, 8);
  g.setColour(tintBody(juce::Colour(0xff666670)).withAlpha(al));
  g.fillEllipse(cx - 1.5f, cy - 1.5f, 3, 3);
}

void drawEncoderLedTrailKnob(juce::Graphics &g, int x, int y, int width,
                                     int height, float sliderPos,
                                     float rotaryStartAngle, float rotaryEndAngle,
                                     juce::Slider &slider,
                                     juce::Colour accent) {
  (void)rotaryStartAngle; (void)rotaryEndAngle;
  auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto r = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX(), cy = bounds.getCentreY();
  bool isEnabled = slider.isEnabled();
  float al = isEnabled ? 1.0f : 0.35f;

  // Dark body
  g.setColour(tintBody(juce::Colour(0xff0a0a10)).withAlpha(al));
  g.fillEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);

  // 48 LED positions with trailing afterglow
  int numLeds = 48;
  int cur = ((int)std::round(sliderPos * numLeds)) % numLeds;
  float ledR = r - 3.0f;
  for (int i = 0; i < numLeds; ++i) {
    float ta = (float)i / numLeds * juce::MathConstants<float>::twoPi - juce::MathConstants<float>::halfPi;
    float lx = cx + ledR * std::cos(ta), ly = cy + ledR * std::sin(ta);
    int dist = std::abs(i - cur);
    if (dist > numLeds / 2) dist = numLeds - dist;
    if (i == cur) {
      g.setColour(accent.withAlpha(0.3f * al));
      g.fillEllipse(lx - 5, ly - 5, 10, 10);
      g.setColour(accent.withAlpha(al));
      g.fillEllipse(lx - 2.5f, ly - 2.5f, 5, 5);
    } else if (dist <= 6) {
      float fade = 1.0f - (float)dist / 7.0f;
      g.setColour(accent.withAlpha(fade * 0.5f * al));
      float sz = 1.5f + fade;
      g.fillEllipse(lx - sz, ly - sz, sz * 2, sz * 2);
    } else {
      g.setColour(tintBody(juce::Colour(0xff141e28)).withAlpha(0.4f * al));
      g.fillEllipse(lx - 0.75f, ly - 0.75f, 1.5f, 1.5f);
    }
  }

  // Inner disc
  float ir = r - 9.0f;
  g.setColour(tintBody(juce::Colour(0xff121218)).withAlpha(al));
  g.fillEllipse(cx - ir, cy - ir, ir * 2.0f, ir * 2.0f);

  // Value readout
  auto font = juce::Font(9.0f).withStyle(juce::Font::bold);
  g.setFont(font);
  g.setColour(accent.withAlpha(0.5f * al));
  g.drawText(juce::String((int)std::round(sliderPos * 100)), (int)(cx - 10), (int)(cy - 6), 20, 12,
             juce::Justification::centred);
}

void drawEncoderSleekKnob(juce::Graphics &g, int x, int y, int width,
                                  int height, float sliderPos,
                                  float rotaryStartAngle, float rotaryEndAngle,
                                  juce::Slider &slider,
                                  juce::Colour accent) {
  (void)rotaryStartAngle; (void)rotaryEndAngle;
  auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto r = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX(), cy = bounds.getCentreY();
  float a = sliderPos * juce::MathConstants<float>::twoPi;
  bool isEnabled = slider.isEnabled();
  float al = isEnabled ? 1.0f : 0.35f;

  // Thin body
  juce::ColourGradient bg(tintBody(juce::Colour(0xff2a2a32)), cx, cy - r,
                           tintBody(juce::Colour(0xff1a1a20)), cx, cy + r, false);
  bg.addColour(0.5, tintBody(juce::Colour(0xff222228)));
  g.setGradientFill(bg);
  g.fillEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);

  // Thin chrome rim
  g.setColour(tintBody(juce::Colour(0xff787882)).withAlpha(0.3f * al));
  g.drawEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f, 0.75f);

  // Fine tick marks (60 like a clock)
  for (int i = 0; i < 60; ++i) {
    float ta = (float)i / 60.0f * juce::MathConstants<float>::twoPi;
    bool isMajor = (i % 15 == 0), isMid = (i % 5 == 0);
    float len = isMajor ? 3.0f : isMid ? 2.0f : 1.0f;
    float w = isMajor ? 0.75f : 0.3f;
    g.setColour(juce::Colours::white.withAlpha((isMajor ? 0.15f : isMid ? 0.07f : 0.03f) * al));
    g.drawLine(cx + (r - len - 1.0f) * std::cos(ta), cy + (r - len - 1.0f) * std::sin(ta),
               cx + (r - 1.0f) * std::cos(ta), cy + (r - 1.0f) * std::sin(ta), w);
  }

  // Single accent pointer line
  auto pa = a - juce::MathConstants<float>::halfPi;
  g.setColour(tintBody(juce::Colour(0xffc8c8d2)).withAlpha(0.5f * al));
  g.drawLine(cx + r * 0.15f * std::cos(pa), cy + r * 0.15f * std::sin(pa),
             cx + (r - 3.0f) * std::cos(pa), cy + (r - 3.0f) * std::sin(pa), 0.5f);

  // Bright tip dot
  auto tipX = cx + (r - 3.0f) * std::cos(pa), tipY = cy + (r - 3.0f) * std::sin(pa);
  g.setColour(accent.withAlpha(0.3f * al));
  g.fillEllipse(tipX - 3, tipY - 3, 6, 6);
  g.setColour(accent.withAlpha(al));
  g.fillEllipse(tipX - 1.5f, tipY - 1.5f, 3, 3);

  // Tiny center dot
  g.setColour(tintBody(juce::Colour(0xff969aa0)).withAlpha(0.3f * al));
  g.fillEllipse(cx - 1, cy - 1, 2, 2);
}

void drawEncoderRadarKnob(juce::Graphics &g, int x, int y, int width,
                                  int height, float sliderPos,
                                  float rotaryStartAngle, float rotaryEndAngle,
                                  juce::Slider &slider,
                                  juce::Colour accent) {
  (void)rotaryStartAngle; (void)rotaryEndAngle;
  auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto r = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX(), cy = bounds.getCentreY();
  float a = sliderPos * juce::MathConstants<float>::twoPi;
  bool isEnabled = slider.isEnabled();
  float al = isEnabled ? 1.0f : 0.35f;

  // Scope background
  juce::ColourGradient bg(tintBody(juce::Colour(0xff0a1a0e)), cx - r * 0.3f, cy - r * 0.4f,
                           tintBody(juce::Colour(0xff040c06)), cx, cy + r, true);
  g.setGradientFill(bg);
  g.fillEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);
  g.setColour(tintBody(juce::Colour(0xff2a3a2e)).withAlpha(al));
  g.drawEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f, 1.5f);

  // Range rings
  for (int i = 1; i <= 3; ++i) {
    float rr = r * i / 4.0f;
    g.setColour(accent.withAlpha(0.04f * al));
    g.drawEllipse(cx - rr, cy - rr, rr * 2.0f, rr * 2.0f, 0.5f);
  }

  // Crosshairs
  g.setColour(accent.withAlpha(0.04f * al));
  g.drawLine(cx - r + 3, cy, cx + r - 3, cy, 0.5f);
  g.drawLine(cx, cy - r + 3, cx, cy + r - 3, 0.5f);

  // 360 tick ring
  for (int i = 0; i < 36; ++i) {
    float ta = (float)i / 36.0f * juce::MathConstants<float>::twoPi;
    bool isMajor = (i % 9 == 0);
    g.setColour(accent.withAlpha((isMajor ? 0.15f : 0.05f) * al));
    g.drawLine(cx + (r - 3.0f) * std::cos(ta), cy + (r - 3.0f) * std::sin(ta),
               cx + (r - 1.0f) * std::cos(ta), cy + (r - 1.0f) * std::sin(ta),
               isMajor ? 1.0f : 0.5f);
  }

  // Sweeping beam with trail
  float pa = a - juce::MathConstants<float>::halfPi;
  float bl = r - 4.0f;
  juce::Path trail;
  trail.startNewSubPath(cx, cy);
  trail.addCentredArc(cx, cy, bl, bl, 0.0f, pa - 0.5f, pa, true);
  trail.closeSubPath();
  g.setColour(accent.withAlpha(0.06f * al));
  g.fillPath(trail);

  // Beam line
  g.setColour(accent.withAlpha(0.6f * al));
  g.drawLine(cx, cy, cx + bl * std::cos(pa), cy + bl * std::sin(pa), 1.0f);

  // Beam tip glow
  auto tipX = cx + bl * std::cos(pa), tipY = cy + bl * std::sin(pa);
  g.setColour(accent.withAlpha(0.3f * al));
  g.fillEllipse(tipX - 4, tipY - 4, 8, 8);
  g.setColour(accent.withAlpha(al));
  g.fillEllipse(tipX - 2, tipY - 2, 4, 4);

  // Blips
  if (isEnabled) {
    float blipA[] = {0.3f, -0.8f, 1.5f};
    float blipD[] = {r * 0.4f, r * 0.7f, r * 0.55f};
    for (int i = 0; i < 3; ++i) {
      float bx = cx + blipD[i] * std::cos(blipA[i]);
      float by = cy + blipD[i] * std::sin(blipA[i]);
      g.setColour(accent.withAlpha(0.2f));
      g.fillEllipse(bx - 1.5f, by - 1.5f, 3, 3);
    }
  }

  // Center dot
  g.setColour(accent.withAlpha(0.4f * al));
  g.fillEllipse(cx - 1, cy - 1, 2, 2);
}

} // namespace knobs
} // namespace gm
