// ghostmoon UI Catalog - KnobStylesStepped.h implementations
// Auto-extracted from header-only to compiled library

#include "KnobStylesStepped.h"

namespace gm {
namespace knobs {

void drawSteppedKnurledKnob(juce::Graphics &g, int x, int y, int width,
                                    int height, float sliderPos,
                                    float rotaryStartAngle, float rotaryEndAngle,
                                    juce::Slider &slider,
                                    juce::Colour accent,
                                    int numSteps) {
  auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto r = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX(), cy = bounds.getCentreY();
  auto startA = rotaryStartAngle, endA = rotaryEndAngle;
  int step = juce::jlimit(0, numSteps - 1, (int)std::round(sliderPos * (numSteps - 1)));
  float sp = (float)step / (numSteps - 1);
  float a = startA + sp * (endA - startA);
  bool isEnabled = slider.isEnabled();
  float al = isEnabled ? 1.0f : 0.35f;

  // Matte body with convex gradient
  juce::ColourGradient bg(tintBody(juce::Colour(0xff383844)), cx - r * 0.25f, cy - r * 0.3f,
                           tintBody(juce::Colour(0xff222230)), cx + r, cy + r, true);
  g.setGradientFill(bg);
  g.fillEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);

  // Knurl teeth
  for (int i = 0; i < 48; ++i) {
    float t = (float)i / 48.0f * juce::MathConstants<float>::twoPi;
    float inner = (i % 2 == 0) ? r - 4.0f : r - 2.0f;
    g.setColour((i % 2 == 0) ? tintBody(juce::Colour(0xff464655)).withAlpha(0.5f * al)
                              : tintBody(juce::Colour(0xff282834)).withAlpha(0.5f * al));
    g.drawLine(cx + inner * std::cos(t), cy + inner * std::sin(t),
               cx + (r - 0.5f) * std::cos(t), cy + (r - 0.5f) * std::sin(t), 1.0f);
  }

  // Detent ticks
  float tot = endA - startA;
  for (int i = 0; i < numSteps; ++i) {
    float ta = startA + (float)i / (numSteps - 1) * tot;
    float pa2 = ta - juce::MathConstants<float>::halfPi;
    g.setColour(i == step ? accent.withAlpha(0.7f * al) : juce::Colours::white.withAlpha(0.15f * al));
    g.drawLine(cx + (r + 2.0f) * std::cos(pa2), cy + (r + 2.0f) * std::sin(pa2),
               cx + (r + 5.0f) * std::cos(pa2), cy + (r + 5.0f) * std::sin(pa2),
               i == step ? 1.5f : 0.75f);
  }

  // Value arc
  auto arcR = r - 6.0f;
  drawBgArc(g, cx, cy, arcR, startA, endA, al, tintBody(juce::Colour(0xff0a0a10)));
  if (sp > 0.001f) {
    juce::Path va;
    va.addCentredArc(cx, cy, arcR, arcR, 0.0f, startA, a, true);
    g.setColour(accent.withAlpha(0.75f * al));
    g.strokePath(va, juce::PathStrokeType(1.5f));
  }

  // Bright pointer dot
  auto pa = a - juce::MathConstants<float>::halfPi;
  g.setColour(juce::Colours::white.withAlpha(0.3f * al));
  g.fillEllipse(cx + r * 0.55f * std::cos(pa) - 4, cy + r * 0.55f * std::sin(pa) - 4, 8, 8);
  g.setColour(juce::Colours::white.withAlpha(al));
  g.fillEllipse(cx + r * 0.55f * std::cos(pa) - 2, cy + r * 0.55f * std::sin(pa) - 2, 4, 4);

  // Center cap
  g.setColour(tintBody(juce::Colour(0xff222230)).withAlpha(al));
  g.fillEllipse(cx - 3, cy - 3, 6, 6);
}

void drawSteppedCarbonKnob(juce::Graphics &g, int x, int y, int width,
                                   int height, float sliderPos,
                                   float rotaryStartAngle, float rotaryEndAngle,
                                   juce::Slider &slider,
                                   juce::Colour accent,
                                   int numSteps) {
  auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto r = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX(), cy = bounds.getCentreY();
  auto startA = rotaryStartAngle, endA = rotaryEndAngle;
  int step = juce::jlimit(0, numSteps - 1, (int)std::round(sliderPos * (numSteps - 1)));
  bool isEnabled = slider.isEnabled();
  float al = isEnabled ? 1.0f : 0.35f;

  // Carbon fiber body
  g.setColour(tintBody(juce::Colour(0xff161618)).withAlpha(al));
  g.fillEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);
  g.setColour(tintBody(juce::Colour(0xff3a3a40)).withAlpha(al));
  g.drawEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f, 1.0f);

  // Step segments as lit blocks
  float tot = endA - startA;
  float gap = 0.06f;
  float seg = (tot - gap * numSteps) / numSteps;
  auto arcR = r - 4.0f;
  for (int i = 0; i < numSteps; ++i) {
    float ss = startA + i * (seg + gap);
    float se = ss + seg;
    bool lit = (i <= step);
    juce::Path block;
    block.addCentredArc(cx, cy, arcR, arcR, 0.0f, ss, se, true);
    g.setColour(lit ? accent.withAlpha(0.75f * al) : tintBody(juce::Colour(0xff101014)).withAlpha(0.4f * al));
    g.strokePath(block, juce::PathStrokeType(3.0f));
  }

  // Pointer dot at current step
  float sp = (float)step / (numSteps - 1);
  float a = startA + sp * tot;
  auto pa = a - juce::MathConstants<float>::halfPi;
  g.setColour(accent.withAlpha(al));
  g.fillEllipse(cx + r * 0.6f * std::cos(pa) - 2, cy + r * 0.6f * std::sin(pa) - 2, 4, 4);

  // Center cap
  g.setColour(tintBody(juce::Colour(0xff0e0e12)).withAlpha(al));
  g.fillEllipse(cx - 2.5f, cy - 2.5f, 5, 5);
}

void drawRotarySwitchKnob(juce::Graphics &g, int x, int y, int width,
                                  int height, float sliderPos,
                                  float rotaryStartAngle, float rotaryEndAngle,
                                  juce::Slider &slider,
                                  juce::Colour accent,
                                  int numSteps) {
  auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto r = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX(), cy = bounds.getCentreY();
  auto startA = rotaryStartAngle, endA = rotaryEndAngle;
  int step = juce::jlimit(0, numSteps - 1, (int)std::round(sliderPos * (numSteps - 1)));
  float sp = (float)step / (numSteps - 1);
  float a = startA + sp * (endA - startA);
  bool isEnabled = slider.isEnabled();
  float al = isEnabled ? 1.0f : 0.35f;

  // Dark bakelite body
  juce::ColourGradient bg(tintBody(juce::Colour(0xff2a2420)), cx, cy - r * 0.2f,
                           tintBody(juce::Colour(0xff181410)), cx, cy + r, true);
  g.setGradientFill(bg);
  g.fillEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);
  g.setColour(tintBody(juce::Colour(0xff3a3428)).withAlpha(al));
  g.drawEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f, 1.5f);

  // Position numbers + detent dots
  float tot = endA - startA;
  auto font = juce::Font(7.0f);
  g.setFont(font);
  for (int i = 0; i < numSteps; ++i) {
    float ta = startA + (float)i / (numSteps - 1) * tot;
    float pa2 = ta - juce::MathConstants<float>::halfPi;
    // Number
    float tx = cx + (r + 7.0f) * std::cos(pa2);
    float ty = cy + (r + 7.0f) * std::sin(pa2);
    g.setColour(i == step ? tintBody(juce::Colour(0xffffdc8c)).withAlpha(0.8f * al)
                          : tintBody(juce::Colour(0xffb4a078)).withAlpha(0.3f * al));
    g.drawText(juce::String(i + 1), (int)(tx - 5), (int)(ty - 5), 10, 10, juce::Justification::centred);
    // Detent dot
    g.setColour(i == step ? accent.withAlpha(0.5f * al) : tintBody(juce::Colour(0xff3c3223)).withAlpha(0.4f * al));
    g.fillEllipse(cx + (r - 3.0f) * std::cos(pa2) - 1.5f, cy + (r - 3.0f) * std::sin(pa2) - 1.5f, 3, 3);
  }

  // Chicken-head pointer (triangle)
  auto pa = a - juce::MathConstants<float>::halfPi;
  float pLen = r * 0.6f;
  juce::Path pointer;
  pointer.addTriangle(pLen, 0.0f, -4.0f, -5.0f, -4.0f, 5.0f);
  pointer.applyTransform(juce::AffineTransform::rotation(pa).translated(cx, cy));
  g.setColour(tintBody(juce::Colour(0xffc0b898)).withAlpha(al));
  g.fillPath(pointer);
  g.setColour(juce::Colours::black.withAlpha(0.3f * al));
  g.strokePath(pointer, juce::PathStrokeType(0.75f));

  // Center cap
  g.setColour(tintBody(juce::Colour(0xff3a3428)).withAlpha(al));
  g.fillEllipse(cx - 4, cy - 4, 8, 8);
  g.setColour(tintBody(juce::Colour(0xff8a7a60)).withAlpha(al));
  g.fillEllipse(cx - 1.5f, cy - 1.5f, 3, 3);
}

void drawSteppedLedRingKnob(juce::Graphics &g, int x, int y, int width,
                                    int height, float sliderPos,
                                    float rotaryStartAngle, float rotaryEndAngle,
                                    juce::Slider &slider,
                                    juce::Colour accent,
                                    int numSteps) {
  auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto r = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX(), cy = bounds.getCentreY();
  auto startA = rotaryStartAngle, endA = rotaryEndAngle;
  int step = juce::jlimit(0, numSteps - 1, (int)std::round(sliderPos * (numSteps - 1)));
  bool isEnabled = slider.isEnabled();
  float al = isEnabled ? 1.0f : 0.35f;

  // Dark body
  g.setColour(tintBody(juce::Colour(0xff0e0e14)).withAlpha(al));
  g.fillEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);
  g.setColour(tintBody(juce::Colour(0xff222230)).withAlpha(al));
  g.drawEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f, 1.0f);

  // LED dots
  float tot = endA - startA;
  float ledR = r - 4.0f;
  for (int i = 0; i < numSteps; ++i) {
    float ta = startA + (float)i / (numSteps - 1) * tot;
    float pa2 = ta - juce::MathConstants<float>::halfPi;
    float lx = cx + ledR * std::cos(pa2), ly = cy + ledR * std::sin(pa2);
    if (i == step) {
      // Lit LED with glow
      g.setColour(accent.withAlpha(0.2f * al));
      g.fillEllipse(lx - 5, ly - 5, 10, 10);
      g.setColour(accent.withAlpha(al));
      g.fillEllipse(lx - 2.5f, ly - 2.5f, 5, 5);
    } else {
      g.setColour(tintBody(juce::Colour(0xff1e1e28)).withAlpha(0.6f * al));
      g.fillEllipse(lx - 1.5f, ly - 1.5f, 3, 3);
    }
  }

  // Value label in center
  auto font = juce::Font(10.0f).withStyle(juce::Font::bold);
  g.setFont(font);
  g.setColour(accent.withAlpha(0.6f * al));
  g.drawText(juce::String(step + 1), (int)(cx - 8), (int)(cy - 6), 16, 12, juce::Justification::centred);
}

} // namespace knobs
} // namespace gm
