// ghostmoon UI Catalog - KnobStylesBipolar.h implementations
// Auto-extracted from header-only to compiled library

#include "KnobStylesBipolar.h"

namespace gm {
namespace knobs {

float bipolarMidAngle(float startA, float endA) {
  return (startA + endA) * 0.5f;
}

void drawBipolarOledKnob(juce::Graphics &g, int x, int y, int width,
                                 int height, float sliderPos,
                                 float rotaryStartAngle, float rotaryEndAngle,
                                 juce::Slider &slider,
                                 juce::Colour accent) {
  auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto r = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX(), cy = bounds.getCentreY();
  auto startA = rotaryStartAngle, endA = rotaryEndAngle;
  auto midA = bipolarMidAngle(startA, endA);
  auto p = sliderPos * 2.0f - 1.0f; // -1..+1
  auto a = midA + p * (p > 0 ? (endA - midA) : (midA - startA));
  bool isEnabled = slider.isEnabled();
  float al = isEnabled ? 1.0f : 0.35f;

  // Dark body
  g.setColour(tintBody(juce::Colour(0xff111116)).withAlpha(al));
  g.fillEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);

  // OLED band track
  auto bandR = r - 2.0f;
  juce::Path track;
  track.addCentredArc(cx, cy, bandR, bandR, 0.0f, startA, endA, true);
  g.setColour(tintBody(juce::Colour(0xff060610)).withAlpha(al));
  g.strokePath(track, juce::PathStrokeType(8.0f));

  // Center tick at 12 o'clock
  auto ma = midA - juce::MathConstants<float>::halfPi;
  g.setColour(juce::Colours::white.withAlpha(0.2f * al));
  g.drawLine(cx + (r - 7.0f) * std::cos(ma), cy + (r - 7.0f) * std::sin(ma),
             cx + (r + 1.0f) * std::cos(ma), cy + (r + 1.0f) * std::sin(ma), 1.5f);

  // Lit band from center outward
  if (std::abs(p) > 0.01f) {
    auto s = p > 0 ? midA : midA + p * (midA - startA);
    auto e = p > 0 ? midA + p * (endA - midA) : midA;
    auto col = p > 0 ? tintAccent(accent) : tintAccent(juce::Colour(0xffff6622));
    juce::Path lit;
    lit.addCentredArc(cx, cy, bandR, bandR, 0.0f, s, e, true);
    g.setColour(col.withAlpha(0.8f * al));
    g.strokePath(lit, juce::PathStrokeType(5.0f));
  }

  // Inner disc
  auto ir = r - 9.0f;
  g.setColour(tintBody(juce::Colour(0xff1a1a20)).withAlpha(al));
  g.fillEllipse(cx - ir, cy - ir, ir * 2.0f, ir * 2.0f);

  // Center dot
  g.setColour(tintBody(juce::Colour(0xff111118)).withAlpha(al));
  g.fillEllipse(cx - 2, cy - 2, 4, 4);
}

void drawBipolarIvoryKnob(juce::Graphics &g, int x, int y, int width,
                                  int height, float sliderPos,
                                  float rotaryStartAngle, float rotaryEndAngle,
                                  juce::Slider &slider,
                                  juce::Colour accent) {
  auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto r = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX(), cy = bounds.getCentreY();
  auto startA = rotaryStartAngle, endA = rotaryEndAngle;
  auto midA = bipolarMidAngle(startA, endA);
  auto p = sliderPos * 2.0f - 1.0f;
  auto a = midA + p * (p > 0 ? (endA - midA) : (midA - startA));
  bool isEnabled = slider.isEnabled();
  float al = isEnabled ? 1.0f : 0.35f;

  // Gold bezel
  g.setColour(tintBody(juce::Colour(0xffdcb450)).withAlpha(0.3f * al));
  g.drawEllipse(cx - r - 1.0f, cy - r - 1.0f, (r + 1.0f) * 2.0f, (r + 1.0f) * 2.0f, 2.5f);

  // Ivory porcelain body
  juce::ColourGradient ivoryGrad(tintBody(juce::Colour(0xffddd8ce)), cx, cy - r,
                                  tintBody(juce::Colour(0xffaaa59c)), cx, cy + r, false);
  ivoryGrad.addColour(0.5, tintBody(juce::Colour(0xffccc7bd)));
  g.setGradientFill(ivoryGrad);
  g.fillEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);

  // Center tick (gold)
  auto ma = midA - juce::MathConstants<float>::halfPi;
  g.setColour(tintBody(juce::Colour(0xffccaa44)).withAlpha(0.6f * al));
  g.drawLine(cx + (r - 8.0f) * std::cos(ma), cy + (r - 8.0f) * std::sin(ma),
             cx + (r - 2.0f) * std::cos(ma), cy + (r - 2.0f) * std::sin(ma), 1.0f);

  // Arc track
  auto arcR = r - 5.0f;
  drawBgArc(g, cx, cy, arcR, startA, endA, al, tintBody(juce::Colour(0xff9a8a78)));

  // Bipolar arc
  if (std::abs(p) > 0.01f) {
    auto s = p > 0 ? midA : midA + p * (midA - startA);
    auto e = p > 0 ? midA + p * (endA - midA) : midA;
    auto col = p > 0 ? tintAccent(accent) : tintAccent(juce::Colour(0xffbb5533));
    juce::Path va;
    va.addCentredArc(cx, cy, arcR, arcR, 0.0f, s, e, true);
    g.setColour(col.withAlpha(0.7f * al));
    g.strokePath(va, juce::PathStrokeType(2.0f));
  }

  // Dark engraved pointer
  auto pa = a - juce::MathConstants<float>::halfPi;
  g.setColour(tintBody(juce::Colour(0xff503220)).withAlpha(0.65f * al));
  g.drawLine(cx + r * 0.25f * std::cos(pa), cy + r * 0.25f * std::sin(pa),
             cx + (r - 6.0f) * std::cos(pa), cy + (r - 6.0f) * std::sin(pa), 2.0f);

  // Center cap
  g.setColour(tintBody(juce::Colour(0xffc8c3ba)).withAlpha(al));
  g.fillEllipse(cx - 3, cy - 3, 6, 6);

  drawHighlightCrescent(g, cx, cy, r, al);
}

void drawBipolarNeonKnob(juce::Graphics &g, int x, int y, int width,
                                 int height, float sliderPos,
                                 float rotaryStartAngle, float rotaryEndAngle,
                                 juce::Slider &slider,
                                 juce::Colour accent) {
  auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto r = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX(), cy = bounds.getCentreY();
  auto startA = rotaryStartAngle, endA = rotaryEndAngle;
  auto midA = bipolarMidAngle(startA, endA);
  auto p = sliderPos * 2.0f - 1.0f;
  auto a = midA + p * (p > 0 ? (endA - midA) : (midA - startA));
  bool isEnabled = slider.isEnabled();
  float al = isEnabled ? 1.0f : 0.35f;

  // Dark body
  g.setColour(tintBody(juce::Colour(0xff0e0e14)).withAlpha(al));
  g.fillEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);
  g.setColour(tintBody(juce::Colour(0xff1e1e28)).withAlpha(al));
  g.drawEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f, 1.0f);

  // Center tick
  auto ma = midA - juce::MathConstants<float>::halfPi;
  g.setColour(juce::Colours::white.withAlpha(0.3f * al));
  g.drawLine(cx + (r - 6.0f) * std::cos(ma), cy + (r - 6.0f) * std::sin(ma),
             cx + (r + 1.0f) * std::cos(ma), cy + (r + 1.0f) * std::sin(ma), 1.5f);

  // Recessed track
  auto arcR = r - 4.0f;
  drawBgArc(g, cx, cy, arcR, startA, endA, al, tintBody(juce::Colour(0xff08080e)));

  // Per-segment bloom from center
  if (std::abs(p) > 0.01f) {
    auto s = p > 0 ? midA : midA + p * (midA - startA);
    auto e = p > 0 ? midA + p * (endA - midA) : midA;
    int n = juce::jmax(2, (int)(std::abs(p) * 20));
    float span = e - s;
    for (int i = 0; i < n; ++i) {
      float t = (i + 0.5f) / n;
      float a0 = s + (float)i / n * span;
      float a1 = s + (float)(i + 1) / n * span;
      auto col = p > 0
        ? tintAccent(juce::Colour::fromRGB((uint8_t)(0x88 + t * 0x77), (uint8_t)(0x22 + t * 0x22), (uint8_t)(0xcc + t * 0x33)))
        : tintAccent(juce::Colour::fromRGB((uint8_t)(0x00 + t * 0x44), (uint8_t)(0xaa + t * 0x55), (uint8_t)(0xcc + t * 0x33)));
      juce::Path seg;
      seg.addCentredArc(cx, cy, arcR, arcR, 0.0f, a0, a1, true);
      g.setColour(col.withAlpha(0.8f * al));
      g.strokePath(seg, juce::PathStrokeType(2.0f));
    }
  }

  // Pointer line + tip
  auto pa = a - juce::MathConstants<float>::halfPi;
  g.setColour((p > 0 ? tintAccent(accent) : tintAccent(juce::Colour(0xff44eeff))).withAlpha(0.5f * al));
  g.drawLine(cx + r * 0.3f * std::cos(pa), cy + r * 0.3f * std::sin(pa),
             cx + (r - 5.0f) * std::cos(pa), cy + (r - 5.0f) * std::sin(pa), 0.75f);
  auto tipX = cx + (r - 5.0f) * std::cos(pa), tipY = cy + (r - 5.0f) * std::sin(pa);
  g.setColour((p > 0 ? tintAccent(accent) : tintAccent(juce::Colour(0xff44eeff))).withAlpha(0.3f * al));
  g.fillEllipse(tipX - 4, tipY - 4, 8, 8);
  g.setColour((p > 0 ? tintAccent(accent) : tintAccent(juce::Colour(0xff44eeff))).withAlpha(al));
  g.fillEllipse(tipX - 2, tipY - 2, 4, 4);
}

void drawBipolarGaugeKnob(juce::Graphics &g, int x, int y, int width,
                                  int height, float sliderPos,
                                  float rotaryStartAngle, float rotaryEndAngle,
                                  juce::Slider &slider,
                                  juce::Colour accent) {
  auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto r = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX(), cy = bounds.getCentreY();
  auto startA = rotaryStartAngle, endA = rotaryEndAngle;
  auto midA = bipolarMidAngle(startA, endA);
  auto p = sliderPos * 2.0f - 1.0f;
  auto a = midA + p * (p > 0 ? (endA - midA) : (midA - startA));
  bool isEnabled = slider.isEnabled();
  float al = isEnabled ? 1.0f : 0.35f;

  // Dark body
  g.setColour(tintBody(juce::Colour(0xff141418)).withAlpha(al));
  g.fillEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);
  g.setColour(tintBody(juce::Colour(0xff2a2a34)).withAlpha(al));
  g.drawEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f, 1.5f);

  // Scale marks: -5..0..+5
  float tot = endA - startA;
  for (int i = 0; i <= 10; ++i) {
    float sa = startA + (float)i / 10.0f * tot;
    float pa2 = sa - juce::MathConstants<float>::halfPi;
    bool isCtr = (i == 5);
    float len = isCtr ? 7.0f : (i % 5 == 0 ? 5.0f : 3.0f);
    float w = isCtr ? 1.5f : 0.75f;
    auto col = isCtr ? juce::Colours::white.withAlpha(0.35f * al)
             : (i < 5 ? tintAccent(juce::Colour(0xffdd4444)).withAlpha(0.25f * al)
                      : tintAccent(juce::Colour(0xff44dd44)).withAlpha(0.25f * al));
    g.setColour(col);
    g.drawLine(cx + (r - len - 2.0f) * std::cos(pa2), cy + (r - len - 2.0f) * std::sin(pa2),
               cx + (r - 2.0f) * std::cos(pa2), cy + (r - 2.0f) * std::sin(pa2), w);
  }

  // Arc track + bipolar value
  auto arcR = r - 5.0f;
  drawBgArc(g, cx, cy, arcR, startA, endA, al, tintBody(juce::Colour(0xff0a0a10)));
  if (std::abs(p) > 0.01f) {
    auto s = p > 0 ? midA : midA + p * (midA - startA);
    auto e = p > 0 ? midA + p * (endA - midA) : midA;
    auto col = p > 0 ? tintAccent(accent) : tintAccent(juce::Colour(0xffdd5544));
    juce::Path va;
    va.addCentredArc(cx, cy, arcR, arcR, 0.0f, s, e, true);
    g.setColour(col.withAlpha(0.8f * al));
    g.strokePath(va, juce::PathStrokeType(2.0f));
  }

  // Needle pointer
  auto pa = a - juce::MathConstants<float>::halfPi;
  g.setColour(juce::Colours::white.withAlpha(0.6f * al));
  g.drawLine(cx + r * 0.15f * std::cos(pa), cy + r * 0.15f * std::sin(pa),
             cx + (r - 5.0f) * std::cos(pa), cy + (r - 5.0f) * std::sin(pa), 2.0f);
  g.setColour(tintBody(juce::Colour(0xffe0e0e0)).withAlpha(al));
  g.drawLine(cx + r * 0.15f * std::cos(pa), cy + r * 0.15f * std::sin(pa),
             cx + (r - 5.0f) * std::cos(pa), cy + (r - 5.0f) * std::sin(pa), 1.0f);

  // Center cap
  g.setColour(tintBody(juce::Colour(0xff1e1e24)).withAlpha(al));
  g.fillEllipse(cx - 3, cy - 3, 6, 6);
  g.setColour(tintBody(juce::Colour(0xff333340)).withAlpha(al));
  g.fillEllipse(cx - 1.5f, cy - 1.5f, 3, 3);
}

} // namespace knobs
} // namespace gm
