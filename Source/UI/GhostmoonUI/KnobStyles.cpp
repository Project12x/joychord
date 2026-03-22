// ghostmoon UI Catalog - KnobStyles.h implementations
// Auto-extracted from header-only to compiled library

#include "KnobStyles.h"

namespace gm {
namespace knobs {

juce::Colour tintBody(juce::Colour base) {
    if (!tintEnabledFlag()) return base;
#ifdef GM_KNOB_HAS_THEME
    return gm::ThemeManager::getInstance().applyTint(base);
#else
    return base;
#endif
}

juce::Colour tintAccent(juce::Colour base) {
    if (!tintEnabledFlag()) return base;
#ifdef GM_KNOB_HAS_THEME
    auto& tm = gm::ThemeManager::getInstance();
    float t = tm.getTintAmount();
    if (t <= 0.0f) return base;
    return base.interpolatedWith(tm.getAccent(), t);
#else
    return base;
#endif
}

void drawHighlightCrescent(juce::Graphics &g, float cx, float cy,
    float radius, float alpha) {
  juce::Path crescent;
  crescent.addCentredArc(cx, cy, radius * 0.85f, radius * 0.85f, 0.0f,
      -juce::MathConstants<float>::pi * 0.65f,
      -juce::MathConstants<float>::pi * 0.15f, true);
  g.setColour(juce::Colours::white.withAlpha(0.06f * alpha));
  g.strokePath(crescent, juce::PathStrokeType(2.0f));
}

void drawBgArc(juce::Graphics &g, float cx, float cy, float arcR,
    float startA, float endA, float alpha,
    juce::Colour col) {
  juce::Path bg;
  bg.addCentredArc(cx, cy, arcR, arcR, 0.0f, startA, endA, true);
  g.setColour(col.withAlpha(0.4f * alpha));
  g.strokePath(bg, juce::PathStrokeType(3.0f));
}

void drawBevel(juce::Graphics &g, float cx, float cy, float radius,
    float lightAlpha, float darkAlpha) {
  juce::Path top;
  top.addCentredArc(cx, cy, radius, radius, 0.0f,
      juce::MathConstants<float>::pi, juce::MathConstants<float>::twoPi, true);
  g.setColour(juce::Colours::white.withAlpha(lightAlpha));
  g.strokePath(top, juce::PathStrokeType(1.0f));
  juce::Path bot;
  bot.addCentredArc(cx, cy, radius, radius, 0.0f,
      0.0f, juce::MathConstants<float>::pi, true);
  g.setColour(juce::Colours::black.withAlpha(darkAlpha));
  g.strokePath(bot, juce::PathStrokeType(1.0f));
}

void drawEnvBand(juce::Graphics &g, float cx, float cy, float radius,
    float alpha) {
  g.saveState();
  juce::Path clip;
  clip.addEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
  g.reduceClipRegion(clip);
  float highlightR = radius * 0.8f;
  float highlightY = cy - radius * 0.35f;
  juce::ColourGradient env(juce::Colours::white.withAlpha(alpha * 2.0f),
      cx, highlightY, juce::Colours::transparentWhite, cx, highlightY + highlightR, true);
  env.addColour(0.5, juce::Colours::white.withAlpha(alpha));
  g.setGradientFill(env);
  g.fillEllipse(cx - highlightR, highlightY - highlightR * 0.4f,
      highlightR * 2.0f, highlightR * 0.8f);
  g.restoreState();
}

void drawTopHighlight(juce::Graphics &g, float cx, float cy,
    float radius, float alpha) {
  juce::Path hl;
  hl.addCentredArc(cx, cy, radius - 1.0f, radius - 1.0f, 0.0f,
      juce::MathConstants<float>::pi * 1.15f,
      juce::MathConstants<float>::pi * 1.85f, true);
  g.setColour(juce::Colours::white.withAlpha(alpha));
  g.strokePath(hl, juce::PathStrokeType(0.75f));
}

void drawScreenHighlight(juce::Graphics &g, float cx, float cy,
    float radius, float offX, float offY, float alpha) {
  juce::ColourGradient scr(juce::Colours::white.withAlpha(alpha),
      cx + offX, cy + offY,
      juce::Colours::transparentBlack, cx + radius * 0.8f, cy + radius * 0.8f, true);
  scr.addColour(0.5, juce::Colours::white.withAlpha(alpha * 0.3f));
  g.saveState();
  juce::Path clip;
  clip.addEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
  g.reduceClipRegion(clip);
  g.setGradientFill(scr);
  g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
  g.restoreState();
}

void drawRadialBrush(juce::Graphics &g, float cx, float cy,
    float radius, int numLines, juce::Colour col, float lineWidth) {
  for (int i = 0; i < numLines; ++i) {
    float a = (float)i / numLines * juce::MathConstants<float>::twoPi;
    g.setColour(col);
    g.drawLine(cx + radius * 0.15f * std::cos(a), cy + radius * 0.15f * std::sin(a),
               cx + (radius - 1.0f) * std::cos(a), cy + (radius - 1.0f) * std::sin(a),
               lineWidth);
  }
}

void drawGrainNoise(juce::Graphics &g, float cx, float cy,
    float radius, juce::Colour col, float step, uint32_t seed) {
  uint32_t s = seed;
  auto rng = [&s]() -> float {
    s = s * 1664525u + 1013904223u;
    return (float)(s >> 1) / (float)(0x7FFFFFFFu);
  };
  g.saveState();
  juce::Path clip;
  clip.addEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
  g.reduceClipRegion(clip);
  g.setColour(col);
  for (float ny = cy - radius; ny < cy + radius; ny += step) {
    for (float nx = cx - radius; nx < cx + radius; nx += step) {
      float dx = nx + step * 0.5f - cx, dy = ny + step * 0.5f - cy;
      if (dx * dx + dy * dy > radius * radius) continue;
      if (rng() > 0.5f)
        g.fillRect(nx, ny, step, step);
    }
  }
  g.restoreState();
}

void drawShadow(juce::Graphics& g, float cx, float cy, float r, float alpha) {
#ifdef GM_KNOB_HAS_BLUR
  static melatonin::DropShadow knobShadow;
  knobShadow.setColor(juce::Colours::black.withAlpha(alpha));
  knobShadow.setRadius(6);
  knobShadow.setOffset({1, 2});
  juce::Path shape;
  shape.addEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);
  knobShadow.render(g, shape);
#else
  g.setColour(juce::Colours::black.withAlpha(alpha * 0.3f));
  g.fillEllipse(cx - r * 1.2f + 2.0f, cy - r * 1.2f + 3.0f, r * 2.4f, r * 2.4f);
  g.setColour(juce::Colours::black.withAlpha(alpha));
  g.fillEllipse(cx - r + 1.0f, cy - r + 1.5f, r * 2.0f, r * 2.0f);
#endif
}

void drawKnobGlow(juce::Graphics& g, float cx, float cy, float r,
                           juce::Colour colour, float alpha) {
#ifdef GM_KNOB_HAS_BLUR
  static melatonin::DropShadow bodyGlow;
  bodyGlow.setColor(colour.withAlpha(alpha));
  bodyGlow.setRadius(8);
  bodyGlow.setOffset({0, 0});
  juce::Path shape;
  shape.addEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);
  bodyGlow.render(g, shape);
#else
  g.setColour(colour.withAlpha(alpha * 0.5f));
  g.fillEllipse(cx - r * 1.3f, cy - r * 1.3f, r * 2.6f, r * 2.6f);
  g.setColour(colour.withAlpha(alpha));
  g.fillEllipse(cx - r * 1.1f, cy - r * 1.1f, r * 2.2f, r * 2.2f);
#endif
}

void drawGlowArc(juce::Graphics &g, float cx, float cy, float arcR,
    float startA, float endA, juce::Colour col,
    float glowWidth, float coreWidth) {
  juce::Path arcPath;
  arcPath.addCentredArc(cx, cy, arcR, arcR, 0.0f, startA, endA, true);

#ifdef GM_KNOB_HAS_BLUR
  static melatonin::DropShadow arcGlow;
  arcGlow.setColor(col.withMultipliedAlpha(0.4f));
  arcGlow.setRadius((int)(glowWidth + 2.0f));
  arcGlow.setOffset({0, 0});
  arcGlow.render(g, arcPath);
#else
  g.setColour(col.withMultipliedAlpha(0.25f));
  g.strokePath(arcPath, juce::PathStrokeType(glowWidth + 4.0f));
  g.setColour(col.withMultipliedAlpha(0.5f));
  g.strokePath(arcPath, juce::PathStrokeType(glowWidth));
#endif

  // Core stroke always drawn (sharp line on top of glow)
  g.setColour(col);
  g.strokePath(arcPath, juce::PathStrokeType(coreWidth,
      juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

} // namespace knobs
} // namespace gm
