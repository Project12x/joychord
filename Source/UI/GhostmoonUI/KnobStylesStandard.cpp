// ghostmoon UI Catalog - KnobStylesStandard.h implementations
// Auto-extracted from header-only to compiled library

#include "KnobStylesStandard.h"

namespace gm {
namespace knobs {

void drawDotRingKnob(juce::Graphics &g, int x, int y, int width,
                             int height, float sliderPos,
                             float rotaryStartAngle, float rotaryEndAngle,
                             juce::Slider &slider,
                             juce::Colour accent) {
  auto bounds =
      juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX();
  auto cy = bounds.getCentreY();
  auto angle =
      rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  bool isEnabled = slider.isEnabled();
  bool isHovered = slider.isMouseOverOrDragging();
  float alpha = isEnabled ? 1.0f : 0.35f;

  auto ringRadius = radius - 2.0f;

  // Outer ring
  g.setColour(tintBody(juce::Colour(0xff333340)).withAlpha(alpha));
  g.drawEllipse(cx - ringRadius, cy - ringRadius, ringRadius * 2.0f,
                ringRadius * 2.0f, 2.0f);

  // Hover glow
  if (isHovered && isEnabled) {
    g.setColour(accent.withAlpha(0.08f));
    g.drawEllipse(cx - ringRadius - 1.0f, cy - ringRadius - 1.0f,
                  (ringRadius + 1.0f) * 2.0f, (ringRadius + 1.0f) * 2.0f,
                  3.0f);
  }

  // Value arc
  if (sliderPos > 0.001f) {
    juce::Path valueArc;
    valueArc.addCentredArc(cx, cy, ringRadius, ringRadius, 0.0f,
                           rotaryStartAngle, angle, true);
    g.setColour(accent.withAlpha(0.12f * alpha));
    g.strokePath(valueArc, juce::PathStrokeType(6.0f));
    g.setColour(accent.withAlpha(0.7f * alpha));
    g.strokePath(valueArc, juce::PathStrokeType(2.0f));
  }

  // Orbiting dot (softDot-style glow)
  {
    auto dotRadius = juce::jmax(3.5f, radius * 0.12f);
    auto pa = angle - juce::MathConstants<float>::halfPi;
    auto dotX = cx + ringRadius * std::cos(pa);
    auto dotY = cy + ringRadius * std::sin(pa);

    // Outer glow halo
    g.setColour(accent.withAlpha(0.15f * alpha));
    g.fillEllipse(dotX - dotRadius * 2.5f, dotY - dotRadius * 2.5f,
                  dotRadius * 5.0f, dotRadius * 5.0f);
    g.setColour(accent.withAlpha(0.7f * alpha));
    g.fillEllipse(dotX - dotRadius, dotY - dotRadius, dotRadius * 2.0f,
                  dotRadius * 2.0f);
    auto innerR = dotRadius * 0.4f;
    g.setColour(tintBody(juce::Colour(0xffffddff)).withAlpha(0.9f * alpha));
    g.fillEllipse(dotX - innerR, dotY - innerR, innerR * 2.0f,
                  innerR * 2.0f);
  }

  // Arc glow bleed onto body
  if (sliderPos > 0.001f) {
    auto pa = angle - juce::MathConstants<float>::halfPi;
    auto gx = cx + ringRadius * std::cos(pa);
    auto gy = cy + ringRadius * std::sin(pa);
    juce::ColourGradient rg(accent.withAlpha(0.06f * alpha), gx, gy,
        juce::Colours::transparentBlack, cx, cy, true);
    g.setGradientFill(rg);
    g.fillEllipse(cx - ringRadius + 3, cy - ringRadius + 3,
                  (ringRadius - 3) * 2.0f, (ringRadius - 3) * 2.0f);
  }

  // Center pivot
  g.setColour(tintBody(juce::Colour(0xff1a1a24)).withAlpha(alpha));
  g.fillEllipse(cx - 2, cy - 2, 4, 4);
  g.setColour(tintBody(juce::Colour(0xff333340)).withAlpha(alpha));
  g.fillEllipse(cx - 1, cy - 1, 2, 2);
}

void drawBronzeKnob(juce::Graphics &g, int x, int y, int width,
                            int height, float sliderPos,
                            float rotaryStartAngle, float rotaryEndAngle,
                            juce::Slider &slider,
                            juce::Colour accent) {
  auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX();
  auto cy = bounds.getCentreY();
  auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  bool isEnabled = slider.isEnabled();
  float alpha = isEnabled ? 1.0f : 0.4f;

  // Drop shadow
  g.setColour(juce::Colours::black.withAlpha(0.35f * alpha));
  g.fillEllipse(cx - radius + 0.5f, cy - radius + 1.5f, radius * 2.0f, radius * 2.0f);

  // Body gradient (warm bronze)
  {
    juce::ColourGradient bodyGrad(tintBody(juce::Colour(0xff5a4a38)), cx, cy - radius,
                                   tintBody(juce::Colour(0xff2a2018)), cx, cy + radius, false);
    g.setGradientFill(bodyGrad);
    g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
  }

  // Radial brush lines (metallic texture)
  {
    g.saveState();
    juce::Path clip;
    clip.addEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
    g.reduceClipRegion(clip);
    drawRadialBrush(g, cx, cy, radius, 60, juce::Colour(0xffC8AA78).withAlpha(0.08f), 0.75f);
    drawRadialBrush(g, cx, cy, radius, 30, juce::Colours::black.withAlpha(0.1f), 0.75f);
    g.restoreState();
  }

  // Bronze bevel ring
  g.setColour(tintBody(juce::Colour(0xff6a5540)).withAlpha(alpha));
  g.drawEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f, 1.25f);

  // Recessed arc track
  auto arcRadius = radius - 5.0f;
  drawBgArc(g, cx, cy, arcRadius, rotaryStartAngle, rotaryEndAngle, alpha,
            juce::Colour(0xff140e06));

  // Per-segment bloom gradient arc: amber -> bright gold
  if (isEnabled && sliderPos > 0.001f) {
    int n = juce::jmax(2, (int)(sliderPos * 30));
    float span = angle - rotaryStartAngle;
    for (int i = 0; i < n; ++i) {
      float t = (i + 0.5f) / n;
      float a0 = rotaryStartAngle + (float)i / n * span;
      float a1 = rotaryStartAngle + (float)(i + 1) / n * span;
      auto col = juce::Colour::fromRGB(
          (uint8_t)(0xaa + t * 0x55), (uint8_t)(0x77 + t * 0x55), (uint8_t)(0x22 + t * 0x33));
      juce::Path seg;
      seg.addCentredArc(cx, cy, arcRadius, arcRadius, 0.0f, a0, a1, true);
      // Glow halo
      g.setColour(col.withAlpha(0.2f * alpha));
      g.strokePath(seg, juce::PathStrokeType(6.0f));
      // Core
      g.setColour(col.withAlpha(0.85f * alpha));
      g.strokePath(seg, juce::PathStrokeType(2.0f));
    }
  }

  // Etched groove pointer (dark line + bright line)
  {
    auto pa = angle - juce::MathConstants<float>::halfPi;
    auto innerDist = radius * 0.28f;
    auto outerDist = radius - 6.0f;
    g.setColour(juce::Colour(0xff0a0804).withAlpha(0.6f * alpha));
    g.drawLine(cx + innerDist * std::cos(pa), cy + innerDist * std::sin(pa),
               cx + outerDist * std::cos(pa), cy + outerDist * std::sin(pa), 2.0f);
    g.setColour(juce::Colour(0xffddd0b8).withAlpha(alpha));
    g.drawLine(cx + innerDist * std::cos(pa), cy + innerDist * std::sin(pa),
               cx + outerDist * std::cos(pa), cy + outerDist * std::sin(pa), 1.0f);
  }

  // Center cap
  g.setColour(tintBody(juce::Colour(0xff3a2a1a)).withAlpha(alpha));
  g.fillEllipse(cx - 3, cy - 3, 6, 6);
  g.setColour(tintBody(juce::Colour(0xff8a7a60)).withAlpha(alpha));
  g.fillEllipse(cx - 1.5f, cy - 1.5f, 3, 3);

  // Advanced polish: bevel + envBand + overlapping warm gradient + topHighlight
  drawBevel(g, cx, cy, radius, 0.04f, 0.08f);
  drawEnvBand(g, cx, cy, radius, 0.03f);
  // Overlapping warm gradient for depth
  {
    juce::ColourGradient wg(juce::Colour(0xff281a0a).withAlpha(0.15f),
        cx + radius * 0.2f, cy + radius * 0.3f,
        juce::Colours::transparentBlack, cx, cy, true);
    g.setGradientFill(wg);
    g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
  }
  drawTopHighlight(g, cx, cy, radius, 0.05f);
}

void drawLedLadderKnob(juce::Graphics &g, int x, int y, int width,
                               int height, float sliderPos,
                               float rotaryStartAngle, float rotaryEndAngle,
                               juce::Slider &slider,
                               juce::Colour accent) {
  auto bounds =
      juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX();
  auto cy = bounds.getCentreY();
  bool isEnabled = slider.isEnabled();
  float alpha = isEnabled ? 1.0f : 0.35f;

  // Dark body
  g.setColour(tintBody(juce::Colour(0xff1a1a20)).withAlpha(alpha));
  g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
  g.setColour(tintBody(juce::Colour(0xff333338)).withAlpha(alpha));
  g.drawEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f, 1.0f);

  // LED segments
  int numSegments = 21;
  float arcRadius = radius - 3.0f;
  float totalArc = rotaryEndAngle - rotaryStartAngle;
  float segGap = 0.04f; // radians gap between segments
  float segArc = (totalArc - segGap * (float)numSegments) / (float)numSegments;

  for (int i = 0; i < numSegments; ++i) {
    float segStart = rotaryStartAngle + (float)i * (segArc + segGap);
    float segEnd = segStart + segArc;
    float segNorm = (float)(i + 1) / (float)numSegments;
    bool isLit = segNorm <= sliderPos + 0.001f;

    juce::Path seg;
    seg.addCentredArc(cx, cy, arcRadius, arcRadius, 0.0f, segStart, segEnd,
                      true);

    if (isLit && isEnabled) {
      // 3-zone color: cyan, yellow, red (matching HTML reference)
      float t = (float)i / (float)numSegments;
      juce::Colour segCol = t < 0.55f ? tintAccent(juce::Colour(0xff00ddcc))
                           : t < 0.78f ? tintAccent(juce::Colour(0xffcccc00))
                                       : tintAccent(juce::Colour(0xffdd3300));
      // Glow halo
      g.setColour(segCol.withAlpha(0.15f));
      g.strokePath(seg, juce::PathStrokeType(7.0f));
      // Lit segment
      g.setColour(segCol);
      g.strokePath(seg, juce::PathStrokeType(3.0f));
    } else {
      // Dim segment
      g.setColour(tintBody(juce::Colour(0xff2a2a30)).withAlpha(alpha));
      g.strokePath(seg, juce::PathStrokeType(3.0f));
    }
  }

  // Subtle surface texture (concentric fine rings)
  for (int i = 1; i <= 4; ++i) {
    float rr = radius * (float)i / 5.0f;
    g.setColour(tintBody(juce::Colour(0xff1e1e26)).withAlpha(0.3f * alpha));
    g.drawEllipse(cx - rr, cy - rr, rr * 2.0f, rr * 2.0f, 0.3f);
  }

  // Center screw detail
  g.setColour(tintBody(juce::Colour(0xff0e0e12)).withAlpha(alpha));
  g.fillEllipse(cx - 3, cy - 3, 6, 6);
  g.setColour(tintBody(juce::Colour(0xff222228)).withAlpha(alpha));
  g.drawEllipse(cx - 3, cy - 3, 6, 6, 0.5f);
}

void drawGlassKnob(juce::Graphics &g, int x, int y, int width,
                           int height, float sliderPos,
                           float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider &slider,
                           juce::Colour accent) {
  auto bounds =
      juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX();
  auto cy = bounds.getCentreY();
  auto angle =
      rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  bool isHovered = slider.isMouseOverOrDragging();
  bool isEnabled = slider.isEnabled();
  float alpha = isEnabled ? 1.0f : 0.35f;

  // Backlight glow (behind the knob)
  if (isEnabled) {
    float glowIntensity = 0.12f + sliderPos * 0.08f;
    g.setColour(accent.withAlpha(glowIntensity));
    g.fillEllipse(cx - radius - 3.0f, cy - radius - 3.0f,
                  (radius + 3.0f) * 2.0f, (radius + 3.0f) * 2.0f);
  }

  // Glass body (translucent)
  {
    auto glassAlpha = isHovered ? 0.35f : 0.25f;
    juce::ColourGradient glassGrad(
        tintBody(juce::Colour(0xffffffff)).withAlpha(glassAlpha * alpha * 0.6f),
        cx, cy - radius,
        tintBody(juce::Colour(0xff222230)).withAlpha(glassAlpha * alpha),
        cx, cy + radius, false);
    g.setGradientFill(glassGrad);
    g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
  }

  // Glass edge (thin bright border)
  g.setColour(juce::Colours::white.withAlpha(0.15f * alpha));
  g.drawEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f, 1.0f);

  // Glass edge rim light (upper portion, matching HTML)
  {
    juce::Path rimLight;
    rimLight.addCentredArc(cx, cy, radius - 1.0f, radius - 1.0f, 0.0f,
        juce::MathConstants<float>::pi * 1.1f,
        juce::MathConstants<float>::pi * 1.9f, true);
    g.setColour(juce::Colours::white.withAlpha(0.1f * alpha));
    g.strokePath(rimLight, juce::PathStrokeType(1.5f));
  }

  // Background arc
  auto arcRadius = radius - 4.0f;
  {
    juce::Path bgArc;
    bgArc.addCentredArc(cx, cy, arcRadius, arcRadius, 0.0f, rotaryStartAngle,
                        rotaryEndAngle, true);
    g.setColour(juce::Colours::white.withAlpha(0.08f * alpha));
    g.strokePath(bgArc, juce::PathStrokeType(2.5f));
  }

  // Value arc
  if (isEnabled && sliderPos > 0.001f) {
    juce::Path valueArc;
    valueArc.addCentredArc(cx, cy, arcRadius, arcRadius, 0.0f,
                           rotaryStartAngle, angle, true);
    g.setColour(accent.withAlpha(0.9f));
    g.strokePath(valueArc, juce::PathStrokeType(
        2.5f, juce::PathStrokeType::curved,
        juce::PathStrokeType::rounded));
  }

  // Pointer line (visible through glass)
  {
    auto pa = angle - juce::MathConstants<float>::halfPi;
    auto inner = radius * 0.25f;
    auto outer = radius - 5.0f;
    g.setColour(juce::Colours::white.withAlpha(0.55f * alpha));
    g.drawLine(cx + inner * std::cos(pa), cy + inner * std::sin(pa),
               cx + outer * std::cos(pa), cy + outer * std::sin(pa), 1.25f);
    // Tip dot on arc
    auto tipX = cx + (radius - 4.0f) * std::cos(pa);
    auto tipY = cy + (radius - 4.0f) * std::sin(pa);
    g.setColour(accent.withAlpha(0.45f * alpha));
    g.fillEllipse(tipX - 2.5f, tipY - 2.5f, 5.0f, 5.0f);
  }

  // Rainbow refraction band across surface (increased alpha for visible refraction)
  {
    g.saveState();
    juce::Path clip;
    clip.addEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
    g.reduceClipRegion(clip);
    juce::ColourGradient rg(juce::Colour(0x2864b4ff), cx - radius, cy - radius * 0.3f,
                            juce::Colour(0x2864ffb4), cx + radius, cy + radius * 0.3f, false);
    rg.addColour(0.2, juce::Colour(0x228866ff));
    rg.addColour(0.4, juce::Colour(0x20cc66ff));
    rg.addColour(0.5, juce::Colour(0x1eff66cc));
    rg.addColour(0.6, juce::Colour(0x20ffaa66));
    rg.addColour(0.8, juce::Colour(0x2266ffaa));
    g.setGradientFill(rg);
    g.fillRect(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
    g.restoreState();
  }

  // Center bubble
  g.setColour(tintBody(juce::Colour(0x0fb4c8e6)));
  g.fillEllipse(cx - 2.5f, cy - 2.5f, 5, 5);
  g.setColour(tintBody(juce::Colour(0x1fc8dcff)));
  g.fillEllipse(cx - 1, cy - 1, 2, 2);
}

void drawMinimalKnob(juce::Graphics &g, int x, int y, int width,
                             int height, float sliderPos,
                             float rotaryStartAngle, float rotaryEndAngle,
                             juce::Slider &slider,
                             juce::Colour accent) {
  auto bounds =
      juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX();
  auto cy = bounds.getCentreY();
  auto angle =
      rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  bool isHovered = slider.isMouseOverOrDragging();
  bool isEnabled = slider.isEnabled();
  float alpha = isEnabled ? 1.0f : 0.35f;

  // Subtle drop shadow
  g.setColour(juce::Colour(0x33000000));
  g.fillEllipse(cx - radius + 0.5f, cy - radius + 1.5f, radius * 2.0f, radius * 2.0f);

  // Body: subtle radial gradient (not dead flat)
  {
    auto topColor = tintBody(isHovered ? juce::Colour(0xff303038) : juce::Colour(0xff282830));
    auto botColor = tintBody(isHovered ? juce::Colour(0xff252530) : juce::Colour(0xff1e1e26));
    if (!isEnabled) {
      topColor = topColor.withMultipliedBrightness(0.5f);
      botColor = botColor.withMultipliedBrightness(0.5f);
    }
    juce::ColourGradient bodyGrad(topColor, cx, cy - radius * 0.8f,
                                   botColor, cx, cy + radius, false);
    g.setGradientFill(bodyGrad);
    g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
  }

  // Faint edge ring
  g.setColour(tintBody(juce::Colour(0xff3a3a44)).withAlpha(0.5f * alpha));
  g.drawEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f, 0.75f);

  // Background arc track
  auto arcR = radius - 3.0f;
  {
    juce::Path bgArc;
    bgArc.addCentredArc(cx, cy, arcR, arcR, 0.0f, rotaryStartAngle,
                        rotaryEndAngle, true);
    g.setColour(tintBody(juce::Colour(0xff1a1a22)).withAlpha(0.6f * alpha));
    g.strokePath(bgArc, juce::PathStrokeType(2.0f));
  }

  // Value arc (subtle, thin)
  if (isEnabled && sliderPos > 0.001f) {
    juce::Path valueArc;
    valueArc.addCentredArc(cx, cy, arcR, arcR, 0.0f,
                           rotaryStartAngle, angle, true);
    g.setColour(accent.withAlpha(0.06f));
    g.strokePath(valueArc, juce::PathStrokeType(5.0f));
    g.setColour(accent.withAlpha(0.35f * alpha));
    g.strokePath(valueArc, juce::PathStrokeType(1.5f));
  }

  // Pointer line with tip dot
  {
    auto pa = angle - juce::MathConstants<float>::halfPi;
    auto inner = radius * 0.2f;
    auto outer = radius - 2.0f;
    // Shadow line
    g.setColour(juce::Colour(0xff000000).withAlpha(0.3f * alpha));
    g.drawLine(cx + inner * std::cos(pa) + 0.5f, cy + inner * std::sin(pa) + 0.5f,
               cx + outer * std::cos(pa) + 0.5f, cy + outer * std::sin(pa) + 0.5f, 2.0f);
    // Bright line
    g.setColour(accent.withAlpha(0.9f * alpha));
    g.drawLine(cx + inner * std::cos(pa), cy + inner * std::sin(pa),
               cx + outer * std::cos(pa), cy + outer * std::sin(pa), 1.5f);
    // Tip dot
    auto tipX = cx + outer * std::cos(pa);
    auto tipY = cy + outer * std::sin(pa);
    g.setColour(accent.withAlpha(alpha));
    g.fillEllipse(tipX - 1.5f, tipY - 1.5f, 3.0f, 3.0f);
  }
}

void drawOutlineKnob(juce::Graphics &g, int x, int y, int width,
                             int height, float sliderPos,
                             float rotaryStartAngle, float rotaryEndAngle,
                             juce::Slider &slider,
                             juce::Colour accent) {
  auto bounds =
      juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX();
  auto cy = bounds.getCentreY();
  auto angle =
      rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  bool isHovered = slider.isMouseOverOrDragging();
  bool isEnabled = slider.isEnabled();
  float alpha = isEnabled ? 1.0f : 0.3f;

  // Outer ring (thick, no fill)
  auto ringColor = tintBody(isHovered ? juce::Colour(0xff555566) : juce::Colour(0xff3a3a48));
  g.setColour(ringColor.withAlpha(alpha));
  g.drawEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f, 2.0f);

  // Inner highlight ring (slightly brighter, smaller)
  g.setColour(juce::Colours::white.withAlpha(0.03f * alpha));
  g.drawEllipse(cx - radius + 2.0f, cy - radius + 2.0f,
                (radius - 2.0f) * 2.0f, (radius - 2.0f) * 2.0f, 0.75f);

  // Background arc track
  auto arcR = radius - 1.0f;
  {
    juce::Path bgArc;
    bgArc.addCentredArc(cx, cy, arcR, arcR, 0.0f, rotaryStartAngle,
                        rotaryEndAngle, true);
    g.setColour(tintBody(juce::Colour(0xff222230)).withAlpha(0.4f * alpha));
    g.strokePath(bgArc, juce::PathStrokeType(2.5f));
  }

  // Value arc
  if (isEnabled && sliderPos > 0.001f) {
    juce::Path valueArc;
    valueArc.addCentredArc(cx, cy, arcR, arcR, 0.0f,
                           rotaryStartAngle, angle, true);
    g.setColour(accent.withAlpha(0.08f));
    g.strokePath(valueArc, juce::PathStrokeType(6.0f));
    g.setColour(accent.withAlpha(0.5f * alpha));
    g.strokePath(valueArc, juce::PathStrokeType(2.0f));
  }

  // Tick mark (extends from inner circle to outer)
  {
    auto pa = angle - juce::MathConstants<float>::halfPi;
    auto inner = radius - 10.0f;
    auto outer = radius + 2.0f;
    // Shadow
    g.setColour(juce::Colour(0xff000000).withAlpha(0.2f * alpha));
    g.drawLine(cx + inner * std::cos(pa) + 0.5f, cy + inner * std::sin(pa) + 0.5f,
               cx + outer * std::cos(pa) + 0.5f, cy + outer * std::sin(pa) + 0.5f, 2.5f);
    // Bright tick
    g.setColour(accent.withAlpha(0.85f * alpha));
    g.drawLine(cx + inner * std::cos(pa), cy + inner * std::sin(pa),
               cx + outer * std::cos(pa), cy + outer * std::sin(pa), 2.0f);
    // Tip dot
    auto tipX = cx + outer * std::cos(pa);
    auto tipY = cy + outer * std::sin(pa);
    g.setColour(accent.withAlpha(alpha));
    g.fillEllipse(tipX - 1.5f, tipY - 1.5f, 3.0f, 3.0f);
  }

  // Center dot (slightly larger, better contrast)
  auto dotR = 2.5f;
  g.setColour(tintBody(juce::Colour(0xff555560)).withAlpha(alpha));
  g.fillEllipse(cx - dotR, cy - dotR, dotR * 2.0f, dotR * 2.0f);
}

void drawNeonArcKnob(juce::Graphics &g, int x, int y, int width,
                              int height, float sliderPos,
                              float rotaryStartAngle, float rotaryEndAngle,
                              juce::Slider &slider,
                              juce::Colour accentStart,
                              juce::Colour accentEnd) {
  auto bounds =
      juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX();
  auto cy = bounds.getCentreY();
  auto angle =
      rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  bool isEnabled = slider.isEnabled();
  float alpha = isEnabled ? 1.0f : 0.35f;

  // Dark body
  g.setColour(tintBody(juce::Colour(0xff1a1a22)).withAlpha(alpha));
  g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

  // Background arc
  auto arcRadius = radius - 4.0f;
  {
    juce::Path bgArc;
    bgArc.addCentredArc(cx, cy, arcRadius, arcRadius, 0.0f, rotaryStartAngle,
                        rotaryEndAngle, true);
    g.setColour(tintBody(juce::Colour(0xff2a2a32)).withAlpha(alpha));
    g.strokePath(bgArc, juce::PathStrokeType(4.0f));
  }

  // Gradient value arc (draw as many small segments to simulate gradient)
  if (isEnabled && sliderPos > 0.001f) {
    int numSegs = juce::jmax(2, (int)(sliderPos * 40.0f));
    float arcSpan = angle - rotaryStartAngle;

    for (int i = 0; i < numSegs; ++i) {
      float t0 = (float)i / (float)numSegs;
      float t1 = (float)(i + 1) / (float)numSegs;
      float a0 = rotaryStartAngle + t0 * arcSpan;
      float a1 = rotaryStartAngle + t1 * arcSpan;

      // Interpolate color
      float tMid = (t0 + t1) * 0.5f;
      auto segColor = accentStart.interpolatedWith(accentEnd, tMid);

      juce::Path seg;
      seg.addCentredArc(cx, cy, arcRadius, arcRadius, 0.0f, a0, a1, true);

      // Glow
      g.setColour(segColor.withAlpha(0.12f));
      g.strokePath(seg, juce::PathStrokeType(8.0f));
      // Core
      g.setColour(segColor);
      g.strokePath(seg, juce::PathStrokeType(3.5f));
    }
  }

  // White pointer dot
  {
    auto dotR = juce::jmax(2.5f, radius * 0.08f);
    auto dotDist = radius * 0.6f;
    auto pa = angle - juce::MathConstants<float>::halfPi;
    auto dotX = cx + dotDist * std::cos(pa);
    auto dotY = cy + dotDist * std::sin(pa);
    g.setColour(juce::Colour(0xffd0d0e0).withAlpha(alpha));
    g.fillEllipse(dotX - dotR, dotY - dotR, dotR * 2.0f, dotR * 2.0f);
  }

  // Center dot
  g.setColour(tintBody(juce::Colour(0xff12121e)).withAlpha(alpha));
  g.fillEllipse(cx - 2, cy - 2, 4, 4);

  // Arc glow bleed onto dark body
  if (sliderPos > 0.001f) {
    auto pa = angle - juce::MathConstants<float>::halfPi;
    auto gx = cx + (radius - 3.0f) * std::cos(pa);
    auto gy = cy + (radius - 3.0f) * std::sin(pa);
    float tMid = sliderPos;
    auto glowCol = accentStart.interpolatedWith(accentEnd, tMid);
    juce::ColourGradient rg(glowCol.withAlpha(0.06f * alpha), gx, gy,
        juce::Colours::transparentBlack, cx, cy, true);
    g.setGradientFill(rg);
    g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
  }
}

void drawKnurledKnob(juce::Graphics &g, int x, int y, int width,
                              int height, float sliderPos,
                              float rotaryStartAngle, float rotaryEndAngle,
                              juce::Slider &slider,
                              juce::Colour accent) {
  auto bounds =
      juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX();
  auto cy = bounds.getCentreY();
  auto angle =
      rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  bool isHovered = slider.isMouseOverOrDragging();
  bool isEnabled = slider.isEnabled();
  float alpha = isEnabled ? 1.0f : 0.35f;

  // Shadow
  g.setColour(juce::Colour(0x55000000));
  g.fillEllipse(cx - radius + 1.0f, cy - radius + 2.0f, radius * 2.0f,
                radius * 2.0f);

  // Body
  {
    auto bodyColor = tintBody(isHovered ? juce::Colour(0xff353540) : juce::Colour(0xff2a2a34));
    if (!isEnabled) bodyColor = bodyColor.withMultipliedBrightness(0.5f);
    juce::ColourGradient bodyGrad(bodyColor.brighter(0.08f), cx, cy - radius,
                                  bodyColor, cx, cy + radius, false);
    g.setGradientFill(bodyGrad);
    g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
  }

  // Knurl tick marks around edge
  int numTicks = 32;
  for (int i = 0; i < numTicks; ++i) {
    float tickAngle = (float)i / (float)numTicks * juce::MathConstants<float>::twoPi;
    float inner = radius - 4.0f;
    float outer = radius - 1.0f;
    float tx1 = cx + inner * std::cos(tickAngle);
    float ty1 = cy + inner * std::sin(tickAngle);
    float tx2 = cx + outer * std::cos(tickAngle);
    float ty2 = cy + outer * std::sin(tickAngle);
    g.setColour(tintBody(juce::Colour(0xff555560)).withAlpha(0.5f * alpha));
    g.drawLine(tx1, ty1, tx2, ty2, 1.0f);
  }

  // Value arc
  auto arcRadius = radius - 6.0f;
  if (isEnabled && sliderPos > 0.001f) {
    juce::Path valueArc;
    valueArc.addCentredArc(cx, cy, arcRadius, arcRadius, 0.0f,
                           rotaryStartAngle, angle, true);
    g.setColour(accent.withAlpha(0.15f));
    g.strokePath(valueArc, juce::PathStrokeType(6.0f));
    g.setColour(accent);
    g.strokePath(valueArc, juce::PathStrokeType(2.5f));
  }

  // White dot marker
  {
    auto dotR = juce::jmax(3.0f, radius * 0.1f);
    auto dotDist = radius * 0.55f;
    auto pa = angle - juce::MathConstants<float>::halfPi;
    auto dotX = cx + dotDist * std::cos(pa);
    auto dotY = cy + dotDist * std::sin(pa);
    // Glow halo
    g.setColour(juce::Colours::white.withAlpha(0.12f * alpha));
    g.fillEllipse(dotX - dotR * 2.0f, dotY - dotR * 2.0f, dotR * 4.0f, dotR * 4.0f);
    g.setColour(juce::Colours::white.withAlpha(alpha));
    g.fillEllipse(dotX - dotR, dotY - dotR, dotR * 2.0f, dotR * 2.0f);
  }

  // Center cap
  g.setColour(tintBody(juce::Colour(0xff222230)).withAlpha(alpha));
  g.fillEllipse(cx - 3, cy - 3, 6, 6);
  g.setColour(tintBody(juce::Colour(0xff3a3a48)).withAlpha(alpha));
  g.fillEllipse(cx - 1.5f, cy - 1.5f, 3, 3);

  // Polish: bevel + screenHL + topHighlight
  drawBevel(g, cx, cy, radius, 0.04f, 0.08f);
  drawScreenHighlight(g, cx, cy, radius, -radius * 0.2f, -radius * 0.25f, 0.04f);
  drawTopHighlight(g, cx, cy, radius, 0.04f);
}

void drawIvoryKnob(juce::Graphics &g, int x, int y, int width,
                           int height, float sliderPos,
                           float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider &slider,
                           juce::Colour accent) {
  auto bounds =
      juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX();
  auto cy = bounds.getCentreY();
  auto angle =
      rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  bool isHovered = slider.isMouseOverOrDragging();
  bool isEnabled = slider.isEnabled();
  float alpha = isEnabled ? 1.0f : 0.35f;

  // Shadow
  g.setColour(juce::Colour(0x44000000));
  g.fillEllipse(cx - radius + 1.0f, cy - radius + 2.0f, radius * 2.0f,
                radius * 2.0f);

  // Ivory body
  {
    auto topColor = tintBody(isHovered ? juce::Colour(0xffddd8d0) : juce::Colour(0xffd0ccc4));
    auto botColor = tintBody(isHovered ? juce::Colour(0xffb8b4ac) : juce::Colour(0xffaaa69e));
    if (!isEnabled) {
      topColor = topColor.withMultipliedBrightness(0.5f);
      botColor = botColor.withMultipliedBrightness(0.5f);
    }
    juce::ColourGradient bodyGrad(topColor, cx, cy - radius, botColor, cx,
                                  cy + radius, false);
    g.setGradientFill(bodyGrad);
    g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
  }

  // Copper ring
  g.setColour(accent.withAlpha(0.6f * alpha));
  g.drawEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f, 1.5f);

  // Background arc
  auto arcRadius = radius - 5.0f;
  {
    juce::Path bgArc;
    bgArc.addCentredArc(cx, cy, arcRadius, arcRadius, 0.0f, rotaryStartAngle,
                        rotaryEndAngle, true);
    g.setColour(juce::Colour(0xff8a8680).withAlpha(0.5f * alpha));
    g.strokePath(bgArc, juce::PathStrokeType(3.0f));
  }

  // Copper value arc
  if (isEnabled && sliderPos > 0.001f) {
    juce::Path valueArc;
    valueArc.addCentredArc(cx, cy, arcRadius, arcRadius, 0.0f,
                           rotaryStartAngle, angle, true);
    g.setColour(accent.withAlpha(0.2f));
    g.strokePath(valueArc, juce::PathStrokeType(7.0f));
    g.setColour(accent);
    g.strokePath(valueArc, juce::PathStrokeType(3.0f,
        juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
  }

  // Dark pointer line (copper tinted)
  {
    auto pa = angle - juce::MathConstants<float>::halfPi;
    auto inner = radius * 0.25f;
    auto outer = radius - 6.0f;
    g.setColour(juce::Colour(0xff503220).withAlpha(0.65f * alpha));
    g.drawLine(cx + inner * std::cos(pa), cy + inner * std::sin(pa),
               cx + outer * std::cos(pa), cy + outer * std::sin(pa), 2.0f);
  }

  // Center cap (porcelain)
  g.setColour(tintBody(juce::Colour(0xffc8c3ba)).withAlpha(alpha));
  g.fillEllipse(cx - 3, cy - 3, 6, 6);
  g.setColour(accent.withAlpha(0.25f * alpha));
  g.drawEllipse(cx - 3, cy - 3, 6, 6, 0.5f);

  // Polish: envBand + topHighlight
  drawEnvBand(g, cx, cy, radius, 0.05f);
  drawTopHighlight(g, cx, cy, radius, 0.06f);
}

void drawConcentricKnob(juce::Graphics &g, int x, int y, int width,
                                int height, float sliderPos,
                                float rotaryStartAngle, float rotaryEndAngle,
                                juce::Slider &slider,
                                juce::Colour accent) {
  auto bounds =
      juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
  auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
  auto cx = bounds.getCentreX();
  auto cy = bounds.getCentreY();
  auto angle =
      rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  bool isEnabled = slider.isEnabled();
  bool isHovered = slider.isMouseOverOrDragging();
  float alpha = isEnabled ? 1.0f : 0.35f;

  // Concentric rings (3 rings at different radii)
  float ringRadii[] = {radius - 2.0f, radius * 0.65f, radius * 0.35f};
  float ringAlphas[] = {0.3f, 0.2f, 0.12f};
  for (int i = 0; i < 3; ++i) {
    auto r = ringRadii[i];
    auto a = ringAlphas[i] * alpha;
    auto color = tintBody(isHovered && i == 0 ? juce::Colour(0xff555566) : juce::Colour(0xff3a3a48));
    g.setColour(color.withAlpha(a));
    g.drawEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f, 1.0f);
  }

  // Value arc on outer ring
  auto arcR = ringRadii[0];
  if (isEnabled && sliderPos > 0.001f) {
    juce::Path valueArc;
    valueArc.addCentredArc(cx, cy, arcR, arcR, 0.0f, rotaryStartAngle,
                           angle, true);
    g.setColour(accent.withAlpha(0.08f));
    g.strokePath(valueArc, juce::PathStrokeType(6.0f));
    g.setColour(accent.withAlpha(0.5f * alpha));
    g.strokePath(valueArc, juce::PathStrokeType(1.5f));
  }

  // Orbiting dot on outer ring
  {
    auto dotR = juce::jmax(3.0f, radius * 0.1f);
    auto pa = angle - juce::MathConstants<float>::halfPi;
    auto dotX = cx + arcR * std::cos(pa);
    auto dotY = cy + arcR * std::sin(pa);

    // Glow
    g.setColour(accent.withAlpha(0.2f * alpha));
    g.fillEllipse(dotX - dotR * 2.0f, dotY - dotR * 2.0f,
                  dotR * 4.0f, dotR * 4.0f);
    // Core
    g.setColour(accent.withAlpha(alpha));
    g.fillEllipse(dotX - dotR, dotY - dotR, dotR * 2.0f, dotR * 2.0f);
    // White center
    auto innerR = dotR * 0.45f;
    g.setColour(juce::Colour(0xffffffff).withAlpha(0.7f * alpha));
    g.fillEllipse(dotX - innerR, dotY - innerR, innerR * 2.0f,
                  innerR * 2.0f);
  }

  // Inner connecting line (from center dot to outer dot)
  {
    auto pa = angle - juce::MathConstants<float>::halfPi;
    auto innerDist = ringRadii[2];
    g.setColour(accent.withAlpha(0.15f * alpha));
    g.drawLine(cx + innerDist * std::cos(pa), cy + innerDist * std::sin(pa),
               cx + arcR * std::cos(pa), cy + arcR * std::sin(pa), 1.0f);
  }
}

void drawBrushedSteelKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  // Shadow
  g.setColour(juce::Colour(0x66000000));
  g.fillEllipse(cx-r+1,cy-r+2,r*2,r*2);
  // Body
  juce::ColourGradient bg(tintBody(juce::Colour(0xff3a3a42)),cx,cy-r,tintBody(juce::Colour(0xff2a2a30)),cx,cy+r,false);
  g.setGradientFill(bg); g.fillEllipse(cx-r,cy-r,r*2,r*2);
  // Brush strokes (horizontal lines across the body)
  g.saveState();
  juce::Path clip; clip.addEllipse(cx-r,cy-r,r*2,r*2);
  g.reduceClipRegion(clip);
  for (float ly=cy-r; ly<cy+r; ly+=2.5f) {
    float lineAlpha = 0.03f + 0.02f*std::sin(ly*0.8f);
    g.setColour(juce::Colours::white.withAlpha(lineAlpha*al));
    g.drawHorizontalLine((int)ly, cx-r, cx+r);
  }
  g.restoreState();
  // Ring
  g.setColour(tintBody(juce::Colour(0xff505058)).withAlpha(al));
  g.drawEllipse(cx-r,cy-r,r*2,r*2,1.5f);
  drawHighlightCrescent(g, cx, cy, r, al);
  // Arc
  auto ar = r-4.0f;
  drawBgArc(g, cx, cy, ar, startA, endA, al);
  if (sliderPos>0.001f) {
    juce::Path va; va.addCentredArc(cx,cy,ar,ar,0,startA,a,true);
    g.setColour(accent.withAlpha(0.12f)); g.strokePath(va,juce::PathStrokeType(7));
    g.setColour(accent); g.strokePath(va,juce::PathStrokeType(3));
  }
  // Groove pointer (recessed line)
  auto pa = a-juce::MathConstants<float>::halfPi;
  g.setColour(tintBody(juce::Colour(0xff181820)).withAlpha(al));
  g.drawLine(cx+r*0.25f*std::cos(pa),cy+r*0.25f*std::sin(pa),
             cx+(r-5)*std::cos(pa),cy+(r-5)*std::sin(pa),2.5f);
  g.setColour(juce::Colours::white.withAlpha(0.15f*al));
  g.drawLine(cx+r*0.25f*std::cos(pa)+0.5f,cy+r*0.25f*std::sin(pa)+0.5f,
             cx+(r-5)*std::cos(pa)+0.5f,cy+(r-5)*std::sin(pa)+0.5f,1.0f);
  // Polish: grainNoise + bevel + envBand + screenHL
  drawGrainNoise(g, cx, cy, r, juce::Colours::white.withAlpha(0.015f), 2.0f, 42);
  drawGrainNoise(g, cx, cy, r, juce::Colours::black.withAlpha(0.012f), 2.0f, 137);
  drawBevel(g, cx, cy, r, 0.04f, 0.08f);
  drawEnvBand(g, cx, cy, r, 0.03f);
  drawScreenHighlight(g, cx, cy, r, -r*0.2f, -r*0.25f, 0.04f);
}

void drawCarbonFiberKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  g.setColour(juce::Colour(0x55000000));
  g.fillEllipse(cx-r+1,cy-r+2,r*2,r*2);
  // Dark body
  g.setColour(tintBody(juce::Colour(0xff1e1e22)).withAlpha(al));
  g.fillEllipse(cx-r,cy-r,r*2,r*2);
  // Crosshatch pattern
  g.saveState();
  juce::Path clip; clip.addEllipse(cx-r+2,cy-r+2,r*2-4,r*2-4);
  g.reduceClipRegion(clip);
  g.setColour(tintBody(juce::Colour(0xff282830)).withAlpha(al));
  for (float d=-r*2; d<r*2; d+=4.0f) {
    g.drawLine(cx-r+d,cy-r,cx-r+d+r*2,cy+r,0.5f);
    g.drawLine(cx+r-d,cy-r,cx+r-d-r*2,cy+r,0.5f);
  }
  g.restoreState();
  g.setColour(tintBody(juce::Colour(0xff3a3a40)).withAlpha(al));
  g.drawEllipse(cx-r,cy-r,r*2,r*2,1.5f);
  drawHighlightCrescent(g, cx, cy, r, al);
  // Arc
  auto ar = r-4.0f;
  juce::Path bgA; bgA.addCentredArc(cx,cy,ar,ar,0,startA,endA,true);
  g.setColour(tintBody(juce::Colour(0xff151518)).withAlpha(al));
  g.strokePath(bgA,juce::PathStrokeType(3));
  if (sliderPos>0.001f) {
    juce::Path va; va.addCentredArc(cx,cy,ar,ar,0,startA,a,true);
    g.setColour(accent.withAlpha(0.15f)); g.strokePath(va,juce::PathStrokeType(7));
    g.setColour(accent); g.strokePath(va,juce::PathStrokeType(3));
  }
  // Pointer dot
  auto pa = a-juce::MathConstants<float>::halfPi;
  auto dd = r*0.6f, dR = r*0.08f;
  g.setColour(accent.withAlpha(al));
  g.fillEllipse(cx+dd*std::cos(pa)-dR,cy+dd*std::sin(pa)-dR,dR*2,dR*2);
  // Center cap
  g.setColour(tintBody(juce::Colour(0xff0e0e12)).withAlpha(al));
  g.fillEllipse(cx-2.5f,cy-2.5f,5,5);
  g.setColour(tintBody(juce::Colour(0xff2a2a30)).withAlpha(al));
  g.drawEllipse(cx-2.5f,cy-2.5f,5,5,0.5f);
  // Polish: bevel + topHighlight
  drawBevel(g, cx, cy, r, 0.03f, 0.06f);
  drawTopHighlight(g, cx, cy, r, 0.03f);
}

void drawChromeCapKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  // Deep ring shadow
  g.setColour(tintBody(juce::Colour(0xff0a0a10)).withAlpha(al));
  g.fillEllipse(cx-r,cy-r,r*2,r*2);
  // Recessed track
  g.setColour(tintBody(juce::Colour(0xff161620)).withAlpha(al));
  g.drawEllipse(cx-r+3,cy-r+3,r*2-6,r*2-6,1.0f);
  // Value arc in recess
  auto ar = r-3.0f;
  drawBgArc(g, cx, cy, ar, startA, endA, al, juce::Colour(0xff0a0a10));
  if (sliderPos>0.001f) {
    juce::Path va; va.addCentredArc(cx,cy,ar,ar,0,startA,a,true);
    g.setColour(accent.withAlpha(0.06f)); g.strokePath(va,juce::PathStrokeType(7));
    g.setColour(accent.withAlpha(0.12f)); g.strokePath(va,juce::PathStrokeType(4));
    g.setColour(accent.withAlpha(0.6f*al)); g.strokePath(va,juce::PathStrokeType(2));
  }
  // Chrome cap (smaller raised circle)
  auto capR = r*0.6f;
  juce::ColourGradient cg(tintBody(juce::Colour(0xff606068)),cx-capR*0.3f,cy-capR*0.5f,
                           tintBody(juce::Colour(0xff2a2a30)),cx+capR*0.3f,cy+capR*0.5f,true);
  g.setGradientFill(cg); g.fillEllipse(cx-capR,cy-capR,capR*2,capR*2);
  g.setColour(tintBody(juce::Colour(0xff707078)).withAlpha(al));
  g.drawEllipse(cx-capR,cy-capR,capR*2,capR*2,1.0f);
  // Inner highlight on cap
  g.setColour(juce::Colours::white.withAlpha(0.03f*al));
  g.drawEllipse(cx-capR+1.5f,cy-capR+1.5f,(capR-1.5f)*2,(capR-1.5f)*2,0.5f);
  // Specular highlight on cap
  g.setColour(juce::Colours::white.withAlpha(0.1f*al));
  g.fillEllipse(cx-capR*0.35f,cy-capR*0.55f,capR*0.5f,capR*0.3f);
  // Pointer notch with glow
  auto pa = a-juce::MathConstants<float>::halfPi;
  g.setColour(juce::Colours::white.withAlpha(0.12f*al));
  g.fillEllipse(cx+capR*0.7f*std::cos(pa)-3.5f,cy+capR*0.7f*std::sin(pa)-3.5f,7,7);
  g.setColour(juce::Colour(0xffe0e0e0).withAlpha(al));
  g.fillEllipse(cx+capR*0.7f*std::cos(pa)-2,cy+capR*0.7f*std::sin(pa)-2,4,4);
  // Center screw
  g.setColour(tintBody(juce::Colour(0xff3a3a40)).withAlpha(al));
  g.fillEllipse(cx-2,cy-2,4,4);
  g.setColour(tintBody(juce::Colour(0xff5a5a64)).withAlpha(al));
  g.drawEllipse(cx-2,cy-2,4,4,0.5f);
  // Polish: radBrush + grainNoise on chrome cap, then screenHL + envBand + bevel + topHighlight
  {
    g.saveState();
    juce::Path clip; clip.addEllipse(cx-capR,cy-capR,capR*2,capR*2);
    g.reduceClipRegion(clip);
    drawRadialBrush(g, cx, cy, capR, 40, juce::Colours::white.withAlpha(0.06f), 0.75f);
    drawRadialBrush(g, cx, cy, capR, 20, juce::Colours::black.withAlpha(0.08f), 0.75f);
    drawGrainNoise(g, cx, cy, capR, juce::Colours::white.withAlpha(0.012f), 2.0f, 55);
    g.restoreState();
  }
  drawScreenHighlight(g, cx, cy, capR, -capR*0.2f, -capR*0.25f, 0.06f);
  drawEnvBand(g, cx, cy, capR, 0.04f);
  drawBevel(g, cx, cy, capR, 0.05f, 0.08f);
  drawTopHighlight(g, cx, cy, capR, 0.07f);
}

void drawGunmetalKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  g.setColour(juce::Colour(0x55000000));
  g.fillEllipse(cx-r+1,cy-r+2,r*2,r*2);
  // Gunmetal body
  juce::ColourGradient bg(tintBody(juce::Colour(0xff383840)),cx,cy-r,tintBody(juce::Colour(0xff222228)),cx,cy+r,false);
  g.setGradientFill(bg); g.fillEllipse(cx-r,cy-r,r*2,r*2);
  g.setColour(tintBody(juce::Colour(0xff4a4a50)).withAlpha(al));
  g.drawEllipse(cx-r,cy-r,r*2,r*2,1.0f);
  drawHighlightCrescent(g, cx, cy, r, al);
  // Scale marks (11 evenly spaced along the arc)
  float totalA = endA-startA;
  for (int i=0; i<=10; i++) {
    float sa = startA + (float)i/10.0f*totalA;
    float pa = sa - juce::MathConstants<float>::halfPi;
    float len = (i%5==0) ? 5.0f : 3.0f;
    g.setColour(tintBody(juce::Colour(0xff666670)).withAlpha(0.6f*al));
    g.drawLine(cx+(r+2)*std::cos(pa),cy+(r+2)*std::sin(pa),
               cx+(r+2+len)*std::cos(pa),cy+(r+2+len)*std::sin(pa),1.0f);
  }
  // Value arc
  auto ar = r-5.0f;
  drawBgArc(g, cx, cy, ar, startA, endA, al, juce::Colour(0xff151518));
  if (sliderPos>0.001f) {
    juce::Path va; va.addCentredArc(cx,cy,ar,ar,0,startA,a,true);
    g.setColour(accent.withAlpha(0.08f)); g.strokePath(va,juce::PathStrokeType(8));
    g.setColour(accent.withAlpha(0.15f)); g.strokePath(va,juce::PathStrokeType(5));
    g.setColour(accent); g.strokePath(va,juce::PathStrokeType(2.5f));
  }
  // Pointer with shadow + tip
  auto pa = a-juce::MathConstants<float>::halfPi;
  g.setColour(juce::Colour(0xff000000).withAlpha(0.25f*al));
  g.drawLine(cx+r*0.25f*std::cos(pa)+0.5f,cy+r*0.25f*std::sin(pa)+0.5f,
             cx+(r-6)*std::cos(pa)+0.5f,cy+(r-6)*std::sin(pa)+0.5f,2.0f);
  g.setColour(juce::Colour(0xffd0d0d0).withAlpha(al));
  g.drawLine(cx+r*0.25f*std::cos(pa),cy+r*0.25f*std::sin(pa),
             cx+(r-6)*std::cos(pa),cy+(r-6)*std::sin(pa),1.5f);
  auto tipX = cx+(r-6)*std::cos(pa), tipY = cy+(r-6)*std::sin(pa);
  g.fillEllipse(tipX-1.5f,tipY-1.5f,3.0f,3.0f);
  // Center cap
  g.setColour(tintBody(juce::Colour(0xff2a2a30)).withAlpha(al));
  g.fillEllipse(cx-3,cy-3,6,6);
  g.setColour(tintBody(juce::Colour(0xff444450)).withAlpha(al));
  g.fillEllipse(cx-1.5f,cy-1.5f,3,3);
  // Polish: grainNoise + bevel + screenHL + topHighlight
  drawGrainNoise(g, cx, cy, r, juce::Colours::white.withAlpha(0.012f), 2.0f, 99);
  drawBevel(g, cx, cy, r, 0.04f, 0.06f);
  drawScreenHighlight(g, cx, cy, r, -r*0.2f, -r*0.25f, 0.03f);
  drawTopHighlight(g, cx, cy, r, 0.04f);
}

void drawRoseGoldKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  g.setColour(juce::Colour(0x55000000));
  g.fillEllipse(cx-r+1,cy-r+2,r*2,r*2);
  // Rose gold body
  juce::ColourGradient bg(tintBody(juce::Colour(0xff5a3838)),cx-r*0.3f,cy-r*0.5f,
                           tintBody(juce::Colour(0xff3a2222)),cx+r*0.3f,cy+r*0.5f,true);
  g.setGradientFill(bg); g.fillEllipse(cx-r,cy-r,r*2,r*2);
  g.setColour(tintBody(juce::Colour(0xff7a5555)).withAlpha(al));
  g.drawEllipse(cx-r,cy-r,r*2,r*2,1.5f);
  drawHighlightCrescent(g, cx, cy, r, al);
  // Specular
  g.setColour(juce::Colours::white.withAlpha(0.06f*al));
  g.fillEllipse(cx-r*0.3f,cy-r*0.7f,r*0.5f,r*0.3f);
  // Arc
  auto ar = r-5.0f;
  juce::Path bgA; bgA.addCentredArc(cx,cy,ar,ar,0,startA,endA,true);
  g.setColour(tintBody(juce::Colour(0xff2a1818)).withAlpha(al));
  g.strokePath(bgA,juce::PathStrokeType(3));
  // Gradient arc: warm pink -> copper (per-segment, matching HTML d15)
  if (sliderPos>0.001f) {
    int n = juce::jmax(2, (int)(sliderPos * 25));
    float span = a - startA;
    for (int i = 0; i < n; ++i) {
      float t = ((float)i + 0.5f) / n;
      float a0 = startA + (float)i / n * span;
      float a1 = startA + (float)(i + 1) / n * span;
      auto col = tintBody(juce::Colour::fromRGB(
          (uint8_t)(0xcc + (int)(t * 0x33)),
          (uint8_t)(0x66 + (int)(t * 0x44)),
          (uint8_t)(0x88 - (int)(t * 0x30))));
      juce::Path seg;
      seg.addCentredArc(cx, cy, ar, ar, 0.0f, a0, a1, true);
      g.setColour(col.withAlpha(0.2f * al));
      g.strokePath(seg, juce::PathStrokeType(5.0f));
      g.setColour(col.withAlpha(0.85f * al));
      g.strokePath(seg, juce::PathStrokeType(1.5f));
    }
  }
  // Hairline pointer with shadow + tip
  auto pa = a-juce::MathConstants<float>::halfPi;
  g.setColour(juce::Colour(0xff000000).withAlpha(0.25f*al));
  g.drawLine(cx+r*0.3f*std::cos(pa)+0.5f,cy+r*0.3f*std::sin(pa)+0.5f,
             cx+(r-6)*std::cos(pa)+0.5f,cy+(r-6)*std::sin(pa)+0.5f,1.5f);
  g.setColour(juce::Colour(0xffeecccc).withAlpha(al));
  g.drawLine(cx+r*0.3f*std::cos(pa),cy+r*0.3f*std::sin(pa),
             cx+(r-6)*std::cos(pa),cy+(r-6)*std::sin(pa),1.0f);
  auto tipX = cx+(r-6)*std::cos(pa), tipY = cy+(r-6)*std::sin(pa);
  g.fillEllipse(tipX-1.0f,tipY-1.0f,2.0f,2.0f);
  // Polish: radBrush + grainNoise + bevel + envBand + overlapping gradient + topHighlight
  {
    g.saveState();
    juce::Path clip; clip.addEllipse(cx-r,cy-r,r*2,r*2);
    g.reduceClipRegion(clip);
    drawRadialBrush(g, cx, cy, r, 50, juce::Colour(0xffffb4a0).withAlpha(0.07f), 0.75f);
    drawRadialBrush(g, cx, cy, r, 25, juce::Colours::black.withAlpha(0.08f), 0.75f);
    drawGrainNoise(g, cx, cy, r, juce::Colour(0xffffc8b4).withAlpha(0.01f), 2.0f, 88);
    g.restoreState();
  }
  drawBevel(g, cx, cy, r, 0.03f, 0.07f);
  drawEnvBand(g, cx, cy, r, 0.03f);
  {
    juce::ColourGradient wg(juce::Colour(0xff3c1414).withAlpha(0.12f),
        cx+r*0.2f, cy+r*0.3f, juce::Colours::transparentBlack, cx, cy, true);
    g.setGradientFill(wg);
    g.fillEllipse(cx-r,cy-r,r*2,r*2);
  }
  drawTopHighlight(g, cx, cy, r, 0.04f);
}

void drawWalnutKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  g.setColour(juce::Colour(0x55000000));
  g.fillEllipse(cx-r+1,cy-r+2,r*2,r*2);
  // Wood body
  juce::ColourGradient bg(tintBody(juce::Colour(0xff3a2a1a)),cx,cy-r,tintBody(juce::Colour(0xff221810)),cx,cy+r,false);
  g.setGradientFill(bg); g.fillEllipse(cx-r,cy-r,r*2,r*2);
  // Grain lines (horizontal, subtle)
  g.saveState();
  juce::Path clip; clip.addEllipse(cx-r,cy-r,r*2,r*2);
  g.reduceClipRegion(clip);
  for (float ly=cy-r; ly<cy+r; ly+=3.0f) {
    float ga = 0.04f + 0.03f*std::sin(ly*1.2f + 0.5f);
    g.setColour(tintBody(juce::Colour(0xff4a3a2a)).withAlpha(ga*al));
    g.drawHorizontalLine((int)ly, cx-r, cx+r);
  }
  g.restoreState();
  g.setColour(tintBody(juce::Colour(0xff4a3828)).withAlpha(al));
  g.drawEllipse(cx-r,cy-r,r*2,r*2,1.5f);
  drawHighlightCrescent(g, cx, cy, r, al);
  // Arc
  auto ar = r-5.0f;
  drawBgArc(g, cx, cy, ar, startA, endA, al, juce::Colour(0xff1a1408));
  if (sliderPos>0.001f) {
    juce::Path va; va.addCentredArc(cx,cy,ar,ar,0,startA,a,true);
    g.setColour(accent.withAlpha(0.08f)); g.strokePath(va,juce::PathStrokeType(8));
    g.setColour(accent.withAlpha(0.15f)); g.strokePath(va,juce::PathStrokeType(5));
    g.setColour(accent); g.strokePath(va,juce::PathStrokeType(2.5f));
  }
  // Pointer with shadow
  auto pa = a-juce::MathConstants<float>::halfPi;
  g.setColour(juce::Colour(0xff000000).withAlpha(0.3f*al));
  g.drawLine(cx+r*0.3f*std::cos(pa)+0.5f,cy+r*0.3f*std::sin(pa)+0.5f,
             cx+(r-6)*std::cos(pa)+0.5f,cy+(r-6)*std::sin(pa)+0.5f,2.0f);
  g.setColour(juce::Colour(0xffddc088).withAlpha(al));
  g.drawLine(cx+r*0.3f*std::cos(pa),cy+r*0.3f*std::sin(pa),
             cx+(r-6)*std::cos(pa),cy+(r-6)*std::sin(pa),1.5f);
  // Tip dot
  auto tipX = cx+(r-6)*std::cos(pa), tipY = cy+(r-6)*std::sin(pa);
  g.fillEllipse(tipX-1.5f,tipY-1.5f,3.0f,3.0f);
  // Brass center dot
  auto dr = r*0.12f;
  g.setColour(tintBody(juce::Colour(0xffccaa66)).withAlpha(al));
  g.fillEllipse(cx-dr,cy-dr,dr*2,dr*2);
  // Polish: bevel + envBand + topHighlight
  drawBevel(g, cx, cy, r, 0.03f, 0.06f);
  drawEnvBand(g, cx, cy, r, 0.03f);
  drawTopHighlight(g, cx, cy, r, 0.04f);
}

void drawCeramicKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  g.setColour(juce::Colour(0x44000000));
  g.fillEllipse(cx-r+1,cy-r+2,r*2,r*2);
  // White ceramic body
  juce::ColourGradient bg(tintBody(juce::Colour(0xffeeeae4)),cx,cy-r,tintBody(juce::Colour(0xffc8c4be)),cx,cy+r,false);
  g.setGradientFill(bg); g.fillEllipse(cx-r,cy-r,r*2,r*2);
  g.setColour(tintBody(juce::Colour(0xffaaa8a2)).withAlpha(al));
  g.drawEllipse(cx-r,cy-r,r*2,r*2,1.0f);
  // Specular
  g.setColour(juce::Colours::white.withAlpha(0.2f*al));
  g.fillEllipse(cx-r*0.25f,cy-r*0.65f,r*0.4f,r*0.25f);
  // Arc
  auto ar = r-5.0f;
  juce::Path bgA; bgA.addCentredArc(cx,cy,ar,ar,0,startA,endA,true);
  g.setColour(tintBody(juce::Colour(0xff999690)).withAlpha(0.4f*al));
  g.strokePath(bgA,juce::PathStrokeType(2.5f));
  if (sliderPos>0.001f) {
    juce::Path va; va.addCentredArc(cx,cy,ar,ar,0,startA,a,true);
    g.setColour(accent.withAlpha(0.08f)); g.strokePath(va,juce::PathStrokeType(8));
    g.setColour(accent.withAlpha(0.15f)); g.strokePath(va,juce::PathStrokeType(5));
    g.setColour(accent); g.strokePath(va,juce::PathStrokeType(2.5f));
  }
  // Gold pointer with shadow
  auto pa = a-juce::MathConstants<float>::halfPi;
  g.setColour(juce::Colour(0xff000000).withAlpha(0.25f*al));
  g.drawLine(cx+r*0.25f*std::cos(pa)+0.5f,cy+r*0.25f*std::sin(pa)+0.5f,
             cx+(r-6)*std::cos(pa)+0.5f,cy+(r-6)*std::sin(pa)+0.5f,2.5f);
  g.setColour(accent.darker(0.2f).withAlpha(al));
  g.drawLine(cx+r*0.25f*std::cos(pa),cy+r*0.25f*std::sin(pa),
             cx+(r-6)*std::cos(pa),cy+(r-6)*std::sin(pa),2.0f);
  // Tip dot
  auto tipX = cx+(r-6)*std::cos(pa), tipY = cy+(r-6)*std::sin(pa);
  g.fillEllipse(tipX-1.5f,tipY-1.5f,3.0f,3.0f);
  // Center cap (porcelain + gold ring)
  g.setColour(tintBody(juce::Colour(0xffddd8ce)).withAlpha(al));
  g.fillEllipse(cx-3,cy-3,6,6);
  g.setColour(accent.withAlpha(0.3f*al));
  g.drawEllipse(cx-3,cy-3,6,6,0.5f);
  // Polish: envBand + overlapping warm gradient + topHighlight
  drawEnvBand(g, cx, cy, r, 0.04f);
  {
    juce::ColourGradient pg(juce::Colours::white.withAlpha(0.04f),
        cx-r*0.2f, cy-r*0.15f, juce::Colours::transparentBlack, cx, cy, true);
    g.setGradientFill(pg);
    g.fillEllipse(cx-r,cy-r,r*2,r*2);
  }
  drawTopHighlight(g, cx, cy, r, 0.05f);
}

void drawSapphireKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  g.setColour(juce::Colour(0x55000000));
  g.fillEllipse(cx-r+1,cy-r+2,r*2,r*2);
  juce::ColourGradient bg(tintBody(juce::Colour(0xff1a2244)),cx-r*0.3f,cy-r*0.5f,
                           tintBody(juce::Colour(0xff0e1228)),cx+r*0.3f,cy+r*0.5f,true);
  g.setGradientFill(bg); g.fillEllipse(cx-r,cy-r,r*2,r*2);
  g.setColour(tintBody(juce::Colour(0xff334466)).withAlpha(al));
  g.drawEllipse(cx-r,cy-r,r*2,r*2,1.5f);
  // Facet lines (8 radial cuts for jewel-cut feel — matching HTML)
  {
    g.saveState();
    juce::Path clip; clip.addEllipse(cx-r,cy-r,r*2,r*2);
    g.reduceClipRegion(clip);
    for (int i = 0; i < 8; ++i) {
      float fa = (float)i / 8.0f * juce::MathConstants<float>::twoPi;
      g.setColour(tintBody(juce::Colour(0xff324678)).withAlpha(0.1f * al));
      g.drawLine(cx, cy, cx + r * std::cos(fa), cy + r * std::sin(fa), 0.75f);
    }
    g.restoreState();
  }
  auto ar = r-5.0f;
  drawBgArc(g, cx, cy, ar, startA, endA, al, juce::Colour(0xff0a1020));
  if (sliderPos>0.001f) {
    juce::Path va; va.addCentredArc(cx,cy,ar,ar,0,startA,a,true);
    g.setColour(accent.withAlpha(0.08f)); g.strokePath(va,juce::PathStrokeType(8));
    g.setColour(accent.withAlpha(0.15f)); g.strokePath(va,juce::PathStrokeType(5));
    g.setColour(accent); g.strokePath(va,juce::PathStrokeType(2.5f));
  }
  // Pointer with shadow + tip
  auto pa = a-juce::MathConstants<float>::halfPi;
  g.setColour(juce::Colour(0xff000000).withAlpha(0.3f*al));
  g.drawLine(cx+r*0.25f*std::cos(pa)+0.5f,cy+r*0.25f*std::sin(pa)+0.5f,
             cx+(r-6)*std::cos(pa)+0.5f,cy+(r-6)*std::sin(pa)+0.5f,2.0f);
  g.setColour(juce::Colour(0xffccccdd).withAlpha(al));
  g.drawLine(cx+r*0.25f*std::cos(pa),cy+r*0.25f*std::sin(pa),
             cx+(r-6)*std::cos(pa),cy+(r-6)*std::sin(pa),1.5f);
  auto tipX = cx+(r-6)*std::cos(pa), tipY = cy+(r-6)*std::sin(pa);
  g.fillEllipse(tipX-1.5f,tipY-1.5f,3.0f,3.0f);
  // Polish: bevel + screenHL + envBand + overlapping blue gradient + topHighlight
  drawBevel(g, cx, cy, r, 0.03f, 0.07f);
  drawScreenHighlight(g, cx, cy, r, -r*0.2f, -r*0.25f, 0.04f);
  drawEnvBand(g, cx, cy, r, 0.03f);
  {
    juce::ColourGradient sg(juce::Colour(0xff0a1028).withAlpha(0.15f),
        cx+r*0.15f, cy+r*0.3f, juce::Colours::transparentBlack, cx, cy, true);
    g.setGradientFill(sg);
    g.fillEllipse(cx-r,cy-r,r*2,r*2);
  }
  drawTopHighlight(g, cx, cy, r, 0.04f);
}

void drawWireframeKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  // Outer wireframe ring
  g.setColour(tintBody(juce::Colour(0xff2a2a3a)).withAlpha(al));
  g.drawEllipse(cx-r,cy-r,r*2,r*2,1.0f);
  // Inner wireframe ring
  auto ir = r*0.6f;
  g.setColour(tintBody(juce::Colour(0xff222230)).withAlpha(al));
  g.drawEllipse(cx-ir,cy-ir,ir*2,ir*2,1.0f);
  // Cross lines (structural wireframe)
  g.setColour(tintBody(juce::Colour(0xff1e1e2a)).withAlpha(0.5f*al));
  g.drawLine(cx-r*0.7f,cy,cx+r*0.7f,cy,0.5f);
  g.drawLine(cx,cy-r*0.7f,cx,cy+r*0.7f,0.5f);
  // Value arc
  if (sliderPos>0.001f) {
    juce::Path va; va.addCentredArc(cx,cy,r-2,r-2,0,startA,a,true);
    g.setColour(accent.withAlpha(0.1f)); g.strokePath(va,juce::PathStrokeType(5));
    g.setColour(accent.withAlpha(0.6f*al)); g.strokePath(va,juce::PathStrokeType(1.5f));
  }
  // Electric pointer
  auto pa = a-juce::MathConstants<float>::halfPi;
  g.setColour(accent.withAlpha(al));
  g.drawLine(cx,cy,cx+(r-2)*std::cos(pa),cy+(r-2)*std::sin(pa),1.5f);
  auto dR = 2.5f;
  g.fillEllipse(cx+(r-2)*std::cos(pa)-dR,cy+(r-2)*std::sin(pa)-dR,dR*2,dR*2);
}

void drawGradientFillKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour fillStart,
    juce::Colour fillEnd) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  // Dark base body
  g.setColour(tintBody(juce::Colour(0xff18181e)).withAlpha(al));
  g.fillEllipse(cx-r,cy-r,r*2,r*2);
  // Fill sweep (arc clip from startA to current angle, matching HTML)
  if (sliderPos>0.001f) {
    g.saveState();
    // Create pie slice using arc + lineTo center (matching HTML approach)
    auto pa0 = startA - juce::MathConstants<float>::halfPi;
    auto pa1 = a - juce::MathConstants<float>::halfPi;
    juce::Path pie;
    pie.addCentredArc(cx, cy, r-2, r-2, 0.0f, startA, a, true);
    pie.lineTo(cx, cy);
    pie.closeSubPath();
    g.reduceClipRegion(pie);
    // Gradient fill (purple to teal based on value)
    juce::ColourGradient fg(fillStart, cx, cy - r,
                            fillEnd, cx, cy + r, false);
    fg.addColour(sliderPos, fillStart.interpolatedWith(fillEnd, sliderPos));
    g.setGradientFill(fg);
    g.setOpacity(0.35f * al);
    g.fillEllipse(cx-r+2,cy-r+2,r*2-4,r*2-4);
    g.restoreState();
  }
  // Ring
  g.setColour(tintBody(juce::Colour(0xff3a3a44)).withAlpha(al));
  g.drawEllipse(cx-r,cy-r,r*2,r*2,1.5f);
  // Pointer
  auto pa = a-juce::MathConstants<float>::halfPi;
  g.setColour(juce::Colour(0xffe0e0e0).withAlpha(al));
  g.drawLine(cx+r*0.2f*std::cos(pa),cy+r*0.2f*std::sin(pa),
             cx+(r-3)*std::cos(pa),cy+(r-3)*std::sin(pa),2.0f);
}

void drawHexKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  constexpr float pi = juce::MathConstants<float>::pi;
  constexpr float hp = juce::MathConstants<float>::halfPi;
  constexpr float tp = juce::MathConstants<float>::twoPi;

  // Helper: draw hexagon
  auto drawHex = [&](float hr, juce::Colour fill, juce::Colour stroke, float sw) {
    juce::Path hex;
    for (int i = 0; i < 6; i++) {
      float ha = (float)i / 6.0f * tp - hp;
      float hx = cx + hr * std::cos(ha), hy = cy + hr * std::sin(ha);
      if (i == 0) hex.startNewSubPath(hx, hy); else hex.lineTo(hx, hy);
    }
    hex.closeSubPath();
    if (fill.getAlpha() > 0) { g.setColour(fill); g.fillPath(hex); }
    if (stroke.getAlpha() > 0) { g.setColour(stroke); g.strokePath(hex, juce::PathStrokeType(sw)); }
  };

  // Outer hex + inner hex
  drawHex(r, tintBody(juce::Colour(0xff1c1c24)).withAlpha(al), accent.withAlpha(0.2f * al), 1.5f);
  drawHex(r * 0.5f, juce::Colours::transparentBlack, accent.withAlpha(0.1f * al), 1.0f);

  // Connecting lines at vertices
  for (int i = 0; i < 6; i++) {
    float ha = (float)i / 6.0f * tp - hp;
    g.setColour(accent.withAlpha(0.08f * al));
    g.drawLine(cx + r * 0.5f * std::cos(ha), cy + r * 0.5f * std::sin(ha),
               cx + r * std::cos(ha), cy + r * std::sin(ha), 0.75f);
  }

  // Hex-perimeter-following value arc (ray-hex distance formula)
  if (sliderPos > 0.001f) {
    float hr = r - 4.0f;
    float cos30 = std::cos(pi / 6.0f);
    float pa0 = startA - hp;
    float pa1 = a - hp;
    float sweep = pa1 - pa0;
    int steps = 48;
    juce::Path hexArc;
    for (int i = 0; i <= steps; i++) {
      float ang = pa0 + (float)i / steps * sweep;
      // Distance from center to hex edge at this angle
      float rel = std::fmod(std::fmod(ang + hp, pi / 3.0f) + pi / 3.0f, pi / 3.0f) - pi / 6.0f;
      float d = hr * cos30 / std::cos(rel);
      float px = cx + d * std::cos(ang), py = cy + d * std::sin(ang);
      if (i == 0) hexArc.startNewSubPath(px, py); else hexArc.lineTo(px, py);
    }
    g.setColour(accent.withAlpha(0.6f * al));
    g.strokePath(hexArc, juce::PathStrokeType(1.5f, juce::PathStrokeType::curved,
                                               juce::PathStrokeType::rounded));
    // Glow bleed at tip
    float gpa = a - hp;
    float relg = std::fmod(std::fmod(gpa + hp, pi / 3.0f) + pi / 3.0f, pi / 3.0f) - pi / 6.0f;
    float dg = (r - 4.0f) * cos30 / std::cos(relg);
    float glx = cx + dg * std::cos(gpa), gly = cy + dg * std::sin(gpa);
    juce::ColourGradient rg(accent.withAlpha(0.06f * al), glx, gly,
        juce::Colours::transparentBlack, cx, cy, true);
    g.setGradientFill(rg);
    g.fillEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);
  }

  // Pointer snapped to hex edge
  {
    float pa = a - hp;
    float cos30 = std::cos(pi / 6.0f);
    float rel = std::fmod(std::fmod(pa + hp, pi / 3.0f) + pi / 3.0f, pi / 3.0f) - pi / 6.0f;
    float d = (r - 4.0f) * cos30 / std::cos(rel);
    g.setColour(accent.withAlpha(0.8f * al));
    g.drawLine(cx + d * 0.3f * std::cos(pa), cy + d * 0.3f * std::sin(pa),
               cx + d * std::cos(pa), cy + d * std::sin(pa), 1.5f);
  }
}

void drawMercuryKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  g.setColour(juce::Colour(0x55000000));
  g.fillEllipse(cx-r+1,cy-r+2,r*2,r*2);
  // Mercury dome (offset radial gradient for convex dome — toned down to match HTML)
  {
    juce::ColourGradient bg(tintBody(juce::Colour(0xff888898)), cx-r*0.3f, cy-r*0.35f,
                             tintBody(juce::Colour(0xff2a2a30)), cx, cy, true);
    bg.addColour(0.4, tintBody(juce::Colour(0xff666674)));
    bg.addColour(0.7, tintBody(juce::Colour(0xff404048)));
    g.setGradientFill(bg); g.fillEllipse(cx-r,cy-r,r*2,r*2);
  }
  g.setColour(tintBody(juce::Colour(0xff606068)).withAlpha(al));
  g.drawEllipse(cx-r,cy-r,r*2,r*2,1.0f);
  // Value arc
  auto ar = r-4.0f;
  drawBgArc(g, cx, cy, ar, startA, endA, al, juce::Colour(0xff1a1a22));
  if (sliderPos>0.001f) {
    juce::Path va; va.addCentredArc(cx,cy,ar,ar,0,startA,a,true);
    g.setColour(accent.withAlpha(0.08f)); g.strokePath(va,juce::PathStrokeType(8));
    g.setColour(accent.withAlpha(0.15f)); g.strokePath(va,juce::PathStrokeType(5));
    g.setColour(accent); g.strokePath(va,juce::PathStrokeType(2.5f));
  }
  auto pa = a-juce::MathConstants<float>::halfPi;
  auto dR = 2.5f;
  g.setColour(juce::Colour(0xffffffff).withAlpha(al));
  g.fillEllipse(cx+r*0.55f*std::cos(pa)-dR,cy+r*0.55f*std::sin(pa)-dR,dR*2,dR*2);
  // Polish: radBrush + grainNoise + bevel + screenHL + envBand + topHighlight
  {
    g.saveState();
    juce::Path clip; clip.addEllipse(cx-r,cy-r,r*2,r*2);
    g.reduceClipRegion(clip);
    drawRadialBrush(g, cx, cy, r, 50, juce::Colours::white.withAlpha(0.06f), 0.75f);
    drawGrainNoise(g, cx, cy, r, juce::Colours::white.withAlpha(0.012f), 2.0f, 66);
    g.restoreState();
  }
  // Center reflection
  g.setColour(juce::Colours::white.withAlpha(0.04f*al));
  g.fillEllipse(cx-2.5f,cy-2.5f,5,5);
  g.setColour(juce::Colours::white.withAlpha(0.08f*al));
  g.fillEllipse(cx-1,cy-1,2,2);
  drawBevel(g, cx, cy, r, 0.05f, 0.08f);
  drawScreenHighlight(g, cx, cy, r, -r*0.25f, -r*0.3f, 0.05f);
  drawEnvBand(g, cx, cy, r, 0.03f);
  drawTopHighlight(g, cx, cy, r, 0.05f);
}

void drawCircuitKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  // PCB green-tinted body
  g.setColour(tintBody(juce::Colour(0xff1a2218)).withAlpha(al));
  g.fillEllipse(cx-r,cy-r,r*2,r*2);
  // Trace lines — cardinal (4) + diagonal (4) matching HTML
  g.setColour(tintBody(juce::Colour(0xff2a3328)).withAlpha(0.5f*al));
  for (int i=0; i<4; i++) {
    float ta = (float)i/4.0f*juce::MathConstants<float>::twoPi;
    g.drawLine(cx,cy,cx+r*0.9f*std::cos(ta),cy+r*0.9f*std::sin(ta),0.75f);
  }
  // Diagonal traces (shorter, thinner)
  for (int i=0; i<4; i++) {
    float ta = (float)i/4.0f*juce::MathConstants<float>::twoPi + juce::MathConstants<float>::pi*0.25f;
    g.setColour(tintBody(juce::Colour(0xff2a3328)).withAlpha(0.25f*al));
    g.drawLine(cx+r*0.3f*std::cos(ta),cy+r*0.3f*std::sin(ta),
               cx+r*0.7f*std::cos(ta),cy+r*0.7f*std::sin(ta),0.5f);
  }
  // Via holes at r*0.5 and r*0.75, 4 cardinal dots each
  g.setColour(tintBody(juce::Colour(0xff2a3328)).withAlpha(0.4f*al));
  g.drawEllipse(cx-r*0.5f,cy-r*0.5f,r,r,0.5f);
  g.drawEllipse(cx-r*0.75f,cy-r*0.75f,r*1.5f,r*1.5f,0.5f);
  for (float rr : {0.5f, 0.75f}) {
    for (int i=0; i<4; i++) {
      float ta = (float)i/4.0f*juce::MathConstants<float>::twoPi;
      float dx = cx + r*rr*std::cos(ta), dy = cy + r*rr*std::sin(ta);
      g.setColour(tintBody(juce::Colour(0xff2a3328)).withAlpha(0.3f*al));
      g.fillEllipse(dx-1.5f,dy-1.5f,3.0f,3.0f);
      g.setColour(accent.withAlpha(0.08f*al));
      g.drawEllipse(dx-1.5f,dy-1.5f,3.0f,3.0f,0.3f);
    }
  }
  g.setColour(tintBody(juce::Colour(0xff334430)).withAlpha(al));
  g.drawEllipse(cx-r,cy-r,r*2,r*2,1.0f);
  // Value arc
  auto ar = r-3.0f;
  drawBgArc(g, cx, cy, ar, startA, endA, al, juce::Colour(0xff0e1a10));
  if (sliderPos>0.001f) {
    juce::Path va; va.addCentredArc(cx,cy,ar,ar,0,startA,a,true);
    g.setColour(accent.withAlpha(0.08f)); g.strokePath(va,juce::PathStrokeType(7));
    g.setColour(accent.withAlpha(0.15f)); g.strokePath(va,juce::PathStrokeType(4));
    g.setColour(accent); g.strokePath(va,juce::PathStrokeType(2));
  }
  // Solder dot pointer with glow
  auto pa = a-juce::MathConstants<float>::halfPi;
  auto dR = juce::jmax(2.5f,r*0.09f);
  g.setColour(accent.withAlpha(0.15f*al));
  g.fillEllipse(cx+r*0.6f*std::cos(pa)-dR*2,cy+r*0.6f*std::sin(pa)-dR*2,dR*4,dR*4);
  g.setColour(tintBody(juce::Colour(0xffccccaa)).withAlpha(al));
  g.fillEllipse(cx+r*0.6f*std::cos(pa)-dR,cy+r*0.6f*std::sin(pa)-dR,dR*2,dR*2);
}

void drawFloatingDiscKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  // Base ring (the "base")
  g.setColour(tintBody(juce::Colour(0xff1a1a22)).withAlpha(al));
  g.fillEllipse(cx-r,cy-r,r*2,r*2);
  g.setColour(tintBody(juce::Colour(0xff333340)).withAlpha(al));
  g.drawEllipse(cx-r,cy-r,r*2,r*2,1.5f);
  // Value arc on base
  auto ar=r-2.0f;
  if (sliderPos>0.001f) {
    juce::Path va; va.addCentredArc(cx,cy,ar,ar,0,startA,a,true);
    g.setColour(accent.withAlpha(0.08f)); g.strokePath(va,juce::PathStrokeType(5));
    g.setColour(accent.withAlpha(0.4f*al)); g.strokePath(va,juce::PathStrokeType(1.5f));
  }
  // Floating disc (smaller, "hovering" with shadow gap)
  auto discR = r*0.55f;
  g.setColour(juce::Colour(0x33000000));
  g.fillEllipse(cx-discR+1,cy-discR+3,discR*2,discR*2);
  juce::ColourGradient dg(tintBody(juce::Colour(0xff404050)),cx,cy-discR,
                           tintBody(juce::Colour(0xff28282e)),cx,cy+discR,false);
  g.setGradientFill(dg); g.fillEllipse(cx-discR,cy-discR,discR*2,discR*2);
  g.setColour(tintBody(juce::Colour(0xff555560)).withAlpha(al));
  g.drawEllipse(cx-discR,cy-discR,discR*2,discR*2,1.0f);
  // Pointer notch on disc
  auto pa = a-juce::MathConstants<float>::halfPi;
  g.setColour(accent.withAlpha(al));
  g.fillEllipse(cx+discR*0.65f*std::cos(pa)-2,cy+discR*0.65f*std::sin(pa)-2,4,4);
  // Polish: radBrush on disc + topHighlight on disc
  {
    g.saveState();
    juce::Path clip; clip.addEllipse(cx-discR,cy-discR,discR*2,discR*2);
    g.reduceClipRegion(clip);
    drawRadialBrush(g, cx, cy, discR, 30, juce::Colours::white.withAlpha(0.05f), 0.75f);
    g.restoreState();
  }
  // Top-edge highlight on disc
  {
    juce::Path hl;
    hl.addCentredArc(cx, cy, discR-1, discR-1, 0.0f,
        juce::MathConstants<float>::pi*1.1f, juce::MathConstants<float>::pi*1.9f, true);
    g.setColour(juce::Colours::white.withAlpha(0.06f));
    g.strokePath(hl, juce::PathStrokeType(0.5f));
  }
}

void drawHolographicKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour /*accent*/) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  // Shadow
  g.setColour(juce::Colour(0x44000000));
  g.fillEllipse(cx-r+1,cy-r+2,r*2,r*2);
  // Hue-shifted body (matching HTML: hsl(p*0.8*360, 30%, 22%))
  auto bodyColor = juce::Colour::fromHSV(sliderPos*0.8f, 0.3f, 0.22f, al);
  g.setColour(bodyColor);
  g.fillEllipse(cx-r,cy-r,r*2,r*2);
  // Holographic grain with shifting color (seeded blocks, matching HTML)
  {
    g.saveState();
    juce::Path clip; clip.addEllipse(cx-r,cy-r,r*2,r*2);
    g.reduceClipRegion(clip);
    uint32_t s2 = 73;
    auto rng2 = [&s2]() -> float {
      s2 = s2 * 1664525u + 1013904223u;
      return (float)(s2 >> 1) / (float)(0x7FFFFFFFu);
    };
    float step = 2.0f;
    for (float ny = cy - r; ny < cy + r; ny += step) {
      for (float nx = cx - r; nx < cx + r; nx += step) {
        float dx = nx + 1.0f - cx, dy = ny + 1.0f - cy;
        if (dx*dx + dy*dy > r*r) continue;
        if (rng2() > 0.4f) continue;
        float hue = std::fmod(((ny - cy + r) / (r * 2.0f)) * 0.5f + sliderPos * 0.8f, 1.0f);
        g.setColour(juce::Colour::fromHSV(hue, 0.5f, 0.55f, 0.04f));
        g.fillRect(nx, ny, step, step);
      }
    }
    g.restoreState();
  }
  g.setColour(tintBody(juce::Colour(0xff555560)).withAlpha(al));
  g.drawEllipse(cx-r,cy-r,r*2,r*2,1.0f);
  // Arc (white)
  auto ar = r-4.0f;
  if (sliderPos>0.001f) {
    juce::Path va; va.addCentredArc(cx,cy,ar,ar,0,startA,a,true);
    g.setColour(juce::Colours::white.withAlpha(0.5f*al));
    g.strokePath(va,juce::PathStrokeType(2.5f));
  }
  auto pa = a-juce::MathConstants<float>::halfPi;
  g.setColour(juce::Colours::white.withAlpha(0.8f*al));
  g.drawLine(cx+r*0.25f*std::cos(pa),cy+r*0.25f*std::sin(pa),
             cx+(r-5)*std::cos(pa),cy+(r-5)*std::sin(pa),1.5f);
}

void drawOledBandKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  // Dark body
  g.setColour(tintBody(juce::Colour(0xff141418)).withAlpha(al));
  g.fillEllipse(cx-r,cy-r,r*2,r*2);
  // OLED band (thick arc track)
  auto bandR = r-2.0f;
  juce::Path bgBand; bgBand.addCentredArc(cx,cy,bandR,bandR,0,startA,endA,true);
  g.setColour(tintBody(juce::Colour(0xff0a0a0e)).withAlpha(al));
  g.strokePath(bgBand,juce::PathStrokeType(8.0f));
  // Lit band
  if (sliderPos>0.001f) {
    juce::Path litBand; litBand.addCentredArc(cx,cy,bandR,bandR,0,startA,a,true);
    g.setColour(accent.withAlpha(0.15f)); g.strokePath(litBand,juce::PathStrokeType(12));
    g.setColour(accent.withAlpha(al)); g.strokePath(litBand,juce::PathStrokeType(8));
  }
  // Inner disc
  auto ir = r-9.0f;
  g.setColour(tintBody(juce::Colour(0xff1a1a20)).withAlpha(al));
  g.fillEllipse(cx-ir,cy-ir,ir*2,ir*2);
  // OLED glow bleed onto inner disc (matching HTML)
  if (sliderPos>0.001f) {
    juce::ColourGradient rg(juce::Colours::transparentBlack, cx, cy,
        accent.withAlpha(sliderPos * 0.04f), cx, cy, true);
    rg.addColour(0.0, juce::Colours::transparentBlack);
    rg.addColour(1.0, accent.withAlpha(sliderPos * 0.04f));
    g.setGradientFill(juce::ColourGradient(
        juce::Colours::transparentBlack, cx, cy,
        accent.withAlpha(sliderPos * 0.04f), cx + ir, cy, true));
    g.fillEllipse(cx-ir,cy-ir,ir*2,ir*2);
  }
  // Center detail (matching HTML)
  g.setColour(tintBody(juce::Colour(0xff111118)).withAlpha(al));
  g.fillEllipse(cx-2,cy-2,4,4);
  g.setColour(tintBody(juce::Colour(0xff2a2a34)).withAlpha(al));
  g.drawEllipse(cx-2,cy-2,4,4,0.5f);
}

void drawPinstripeKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  g.setColour(tintBody(juce::Colour(0xff222228)).withAlpha(al));
  g.fillEllipse(cx-r,cy-r,r*2,r*2);
  // Radial pinstripes
  g.saveState();
  juce::Path clip; clip.addEllipse(cx-r,cy-r,r*2,r*2);
  g.reduceClipRegion(clip);
  for (int i=0; i<24; i++) {
    float la = (float)i/24.0f*juce::MathConstants<float>::twoPi;
    g.setColour(tintBody(juce::Colour(0xff2e2e36)).withAlpha(0.6f*al));
    g.drawLine(cx,cy,cx+r*std::cos(la),cy+r*std::sin(la),0.5f);
  }
  g.restoreState();
  g.setColour(tintBody(juce::Colour(0xff3a3a42)).withAlpha(al));
  g.drawEllipse(cx-r,cy-r,r*2,r*2,1.0f);
  // Value arc
  auto ar = r-4.0f;
  drawBgArc(g, cx, cy, ar, startA, endA, al, juce::Colour(0xff151518));
  if (sliderPos>0.001f) {
    juce::Path va; va.addCentredArc(cx,cy,ar,ar,0,startA,a,true);
    g.setColour(accent.withAlpha(0.08f)); g.strokePath(va,juce::PathStrokeType(7));
    g.setColour(accent.withAlpha(0.15f)); g.strokePath(va,juce::PathStrokeType(4));
    g.setColour(accent); g.strokePath(va,juce::PathStrokeType(2));
  }
  // Colored dot with glow
  auto pa = a-juce::MathConstants<float>::halfPi;
  auto dR = juce::jmax(3.0f,r*0.1f);
  g.setColour(accent.withAlpha(0.15f*al));
  g.fillEllipse(cx+r*0.6f*std::cos(pa)-dR*2,cy+r*0.6f*std::sin(pa)-dR*2,dR*4,dR*4);
  g.setColour(accent.withAlpha(al));
  g.fillEllipse(cx+r*0.6f*std::cos(pa)-dR,cy+r*0.6f*std::sin(pa)-dR,dR*2,dR*2);
  // Center cap
  g.setColour(tintBody(juce::Colour(0xff1a1a22)).withAlpha(al));
  g.fillEllipse(cx-2.5f,cy-2.5f,5,5);
  g.setColour(tintBody(juce::Colour(0xff333340)).withAlpha(al));
  g.fillEllipse(cx-1,cy-1,2,2);
}

void drawSunkenKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  // Raised rim (lighter outer ring)
  g.setColour(tintBody(juce::Colour(0xff3a3a42)).withAlpha(al));
  g.fillEllipse(cx-r,cy-r,r*2,r*2);
  // Sunken interior (concave radial gradient — dark center, lighter rim, matching HTML)
  auto ir = r-4.0f;
  {
    juce::ColourGradient bg(tintBody(juce::Colour(0xff141420)), cx, cy,
                             tintBody(juce::Colour(0xff282834)), cx, cy + ir, true);
    bg.addColour(0.6, tintBody(juce::Colour(0xff1a1a26)));
    g.setGradientFill(bg); g.fillEllipse(cx-ir,cy-ir,ir*2,ir*2);
  }
  // Inner shadow ring
  g.setColour(tintBody(juce::Colour(0xff111118)).withAlpha(0.5f*al));
  g.drawEllipse(cx-ir,cy-ir,ir*2,ir*2,1.0f);
  // Value arc
  auto ar = r-6.0f;
  drawBgArc(g, cx, cy, ar, startA, endA, al, juce::Colour(0xff0a0a10));
  if (sliderPos>0.001f) {
    juce::Path va; va.addCentredArc(cx,cy,ar,ar,0,startA,a,true);
    g.setColour(accent.withAlpha(0.7f*al));
    g.strokePath(va,juce::PathStrokeType(2.0f));
  }
  // Raised ridge pointer (dark + bright edge, matching HTML)
  auto pa = a-juce::MathConstants<float>::halfPi;
  g.setColour(tintBody(juce::Colour(0xff555560)).withAlpha(al));
  g.drawLine(cx+r*0.2f*std::cos(pa),cy+r*0.2f*std::sin(pa),
             cx+(r-5)*std::cos(pa),cy+(r-5)*std::sin(pa),2.0f);
  g.setColour(juce::Colours::white.withAlpha(0.1f*al));
  g.drawLine(cx+r*0.2f*std::cos(pa)+0.5f,cy+r*0.2f*std::sin(pa)+0.5f,
             cx+(r-5)*std::cos(pa)+0.5f,cy+(r-5)*std::sin(pa)+0.5f,0.75f);
  // Center cap (matching HTML: two dots)
  g.setColour(tintBody(juce::Colour(0xff2e2e36)).withAlpha(al));
  g.fillEllipse(cx-3,cy-3,6,6);
  g.setColour(tintBody(juce::Colour(0xff4a4a54)).withAlpha(al));
  g.fillEllipse(cx-1.5f,cy-1.5f,3,3);
}

void drawDualArcKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour inner,
    juce::Colour outer) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  // Dark body
  g.setColour(tintBody(juce::Colour(0xff1a1a22)).withAlpha(al));
  g.fillEllipse(cx-r,cy-r,r*2,r*2);
  // Outer arc (full range, accent color)
  auto or1 = r-2.0f;
  juce::Path bgO; bgO.addCentredArc(cx,cy,or1,or1,0,startA,endA,true);
  g.setColour(tintBody(juce::Colour(0xff2a2a32)).withAlpha(al));
  g.strokePath(bgO,juce::PathStrokeType(2.5f));
  if (sliderPos>0.001f) {
    juce::Path vo; vo.addCentredArc(cx,cy,or1,or1,0,startA,a,true);
    g.setColour(tintAccent(outer).withAlpha(0.08f)); g.strokePath(vo,juce::PathStrokeType(6));
    g.setColour(tintAccent(outer).withAlpha(0.5f*al)); g.strokePath(vo,juce::PathStrokeType(2.5f));
  }
  // Inner arc (value, different color)
  auto ir1 = r-8.0f;
  juce::Path bgI; bgI.addCentredArc(cx,cy,ir1,ir1,0,startA,endA,true);
  g.setColour(tintBody(juce::Colour(0xff222230)).withAlpha(al));
  g.strokePath(bgI,juce::PathStrokeType(3.0f));
  if (sliderPos>0.001f) {
    juce::Path vi; vi.addCentredArc(cx,cy,ir1,ir1,0,startA,a,true);
    g.setColour(tintAccent(inner).withAlpha(0.12f)); g.strokePath(vi,juce::PathStrokeType(7));
    g.setColour(tintAccent(inner).withAlpha(al)); g.strokePath(vi,juce::PathStrokeType(3));
  }
  // Center dot
  auto dR = 2.0f;
  g.setColour(tintBody(juce::Colour(0xff444450)).withAlpha(al));
  g.fillEllipse(cx-dR,cy-dR,dR*2,dR*2);
}

void drawTargetKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  float al = slider.isEnabled() ? 1.0f : 0.35f;
  // Bullseye rings
  float rings[] = {r, r*0.7f, r*0.4f, r*0.15f};
  for (int i=0; i<4; i++) {
    g.setColour(tintBody(juce::Colour(i%2==0 ? 0xff1e1e28 : 0xff282832)).withAlpha(al));
    g.fillEllipse(cx-rings[i],cy-rings[i],rings[i]*2,rings[i]*2);
  }
  g.setColour(tintBody(juce::Colour(0xff333340)).withAlpha(al));
  g.drawEllipse(cx-r,cy-r,r*2,r*2,1.0f);
  // Value arc
  auto ar = r-3.0f;
  drawBgArc(g, cx, cy, ar, startA, endA, al, juce::Colour(0xff121218));
  if (sliderPos>0.001f) {
    juce::Path va; va.addCentredArc(cx,cy,ar,ar,0,startA,a,true);
    g.setColour(accent.withAlpha(0.08f)); g.strokePath(va,juce::PathStrokeType(7));
    g.setColour(accent.withAlpha(0.15f)); g.strokePath(va,juce::PathStrokeType(4));
    g.setColour(accent); g.strokePath(va,juce::PathStrokeType(2));
  }
  // Crosshair pointer with glow
  auto pa = a-juce::MathConstants<float>::halfPi;
  auto px = cx+r*0.6f*std::cos(pa), py = cy+r*0.6f*std::sin(pa);
  auto ch = 4.0f;
  g.setColour(accent.withAlpha(0.15f*al));
  g.fillEllipse(px-ch*1.5f,py-ch*1.5f,ch*3,ch*3);
  g.setColour(accent.withAlpha(al));
  g.drawLine(px-ch,py,px+ch,py,1.0f);
  g.drawLine(px,py-ch,px,py+ch,1.0f);
}

void drawRadarKnob(juce::Graphics &g, int x, int y, int w,
    int h, float sliderPos, float startA, float endA, juce::Slider &slider,
    juce::Colour accent) {
  auto b = juce::Rectangle<int>(x,y,w,h).toFloat().reduced(4.0f);
  auto r = juce::jmin(b.getWidth(),b.getHeight())*0.5f;
  auto cx = b.getCentreX(), cy = b.getCentreY();
  auto a = startA + sliderPos*(endA-startA);
  bool isEnabled = slider.isEnabled();
  float al = isEnabled ? 1.0f : 0.35f;

  // Drop shadow
  g.setColour(juce::Colour(0x55000000));
  g.fillEllipse(cx-r+1, cy-r+2, r*2, r*2);

  // Dark scope background (very dark green-black)
  juce::ColourGradient scopeBg(
      tintBody(juce::Colour(0xff0a1a0e)), cx, cy,
      tintBody(juce::Colour(0xff040c06)), cx, cy+r, true);
  g.setGradientFill(scopeBg);
  g.fillEllipse(cx-r, cy-r, r*2, r*2);

  // Scope bezel ring
  g.setColour(tintBody(juce::Colour(0xff2a3a2e)).withAlpha(al));
  g.drawEllipse(cx-r, cy-r, r*2, r*2, 2.0f);

  // Concentric range rings (3 rings for depth)
  for (int i = 1; i <= 3; ++i) {
    float ringR = r * (float)i / 4.0f;
    g.setColour(accent.withAlpha(0.06f * al));
    g.drawEllipse(cx-ringR, cy-ringR, ringR*2, ringR*2, 0.5f);
  }

  // Crosshair lines (horizontal + vertical through center)
  g.setColour(accent.withAlpha(0.05f * al));
  g.drawLine(cx-r+4, cy, cx+r-4, cy, 0.5f);
  g.drawLine(cx, cy-r+4, cx, cy+r-4, 0.5f);

  // Sweep beam (the main pointer — a radial line with glow trailing)
  {
    auto pa = a - juce::MathConstants<float>::halfPi;
    auto beamLen = r - 4.0f;

    // Trailing glow (faded pie sweep behind the beam)
    if (isEnabled) {
      float trailAngle = 0.4f; // radians of trail
      juce::Path trail;
      trail.addPieSegment(cx-beamLen, cy-beamLen, beamLen*2, beamLen*2,
          a - trailAngle - juce::MathConstants<float>::halfPi,
          pa, 0.0f);
      juce::ColourGradient trailGrad(
          accent.withAlpha(0.0f), cx, cy,
          accent.withAlpha(0.08f), cx + beamLen*std::cos(pa),
          cy + beamLen*std::sin(pa), true);
      g.setGradientFill(trailGrad);
      g.fillPath(trail);
    }

    // Beam line (bright)
    g.setColour(accent.withAlpha(0.6f * al));
    g.drawLine(cx, cy, cx+beamLen*std::cos(pa), cy+beamLen*std::sin(pa), 1.5f);

    // Beam tip glow
    auto tipX = cx + beamLen * std::cos(pa);
    auto tipY = cy + beamLen * std::sin(pa);
    g.setColour(accent.withAlpha(0.3f * al));
    g.fillEllipse(tipX-3, tipY-3, 6, 6);
    g.setColour(accent.withAlpha(al));
    g.fillEllipse(tipX-1.5f, tipY-1.5f, 3, 3);
  }

  // Blip markers — 3 fixed "contacts" on the scope at different radii
  if (isEnabled) {
    float blipAngles[] = {0.3f, -0.8f, 1.5f};
    float blipDist[] = {r*0.4f, r*0.7f, r*0.55f};
    float blipSize[] = {2.0f, 1.5f, 2.5f};
    for (int i = 0; i < 3; ++i) {
      float bx = cx + blipDist[i] * std::cos(blipAngles[i]);
      float by = cy + blipDist[i] * std::sin(blipAngles[i]);
      g.setColour(accent.withAlpha(0.3f));
      g.fillEllipse(bx-blipSize[i], by-blipSize[i], blipSize[i]*2, blipSize[i]*2);
    }
  }

  // Center dot (origin point)
  g.setColour(accent.withAlpha(0.5f * al));
  g.fillEllipse(cx-1.5f, cy-1.5f, 3, 3);

  // Value arc (subtle outer ring showing position)
  if (sliderPos > 0.001f) {
    auto arcR = r - 2.0f;
    juce::Path va;
    va.addCentredArc(cx, cy, arcR, arcR, 0.0f, startA, a, true);
    g.setColour(accent.withAlpha(0.12f));
    g.strokePath(va, juce::PathStrokeType(4.0f));
    g.setColour(accent.withAlpha(0.4f * al));
    g.strokePath(va, juce::PathStrokeType(1.5f));
  }
}

} // namespace knobs
} // namespace gm
