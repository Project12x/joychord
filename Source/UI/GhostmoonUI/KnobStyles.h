#pragma once
// ghostmoon UI Catalog: KnobStyles
// Umbrella header — includes all knob style categories.
// 43 standalone knob rendering functions for LookAndFeel drawRotarySlider overrides.
//
// Theme integration:
//   Body/surface colors use tintBody() to blend toward the theme's tint tone.
//   Arc/indicator colors use tintAccent() to blend toward the theme's accent.
//   Gradient arcs use getArcGradient() for theme preset gradients with tint fallback.
//   Use TintBypass to disable all tinting for specific draw calls.
//
// Usage:
//   void drawRotarySlider(...) override {
//       gm::knobs::drawBronzeKnob(g, x, y, w, h, sliderPos,
//                                  rotaryStartAngle, rotaryEndAngle, slider);
//   }
//
//   // Bypass tinting for one specific knob:
//   { gm::knobs::TintBypass bypass; gm::knobs::drawIvoryKnob(...); }
//
// Categories:
//   KnobStylesStandard.h  — #1-31 standard knobs
//   KnobStylesBipolar.h   — B1-B4 bipolar (center-zero) knobs
//   KnobStylesStepped.h   — S1-S4 stepped/detented knobs
//   KnobStylesEncoder.h   — E1-E4 endless encoder knobs

#include <cmath>
#include <juce_gui_basics/juce_gui_basics.h>

#if __has_include("ThemeManager.h")
#include "ThemeManager.h"
#define GM_KNOB_HAS_THEME 1
#elif __has_include(<ThemeManager.h>)
#include <ThemeManager.h>
#define GM_KNOB_HAS_THEME 1
#endif

#if __has_include(<melatonin_blur/melatonin_blur.h>)
#include <melatonin_blur/melatonin_blur.h>
#define GM_KNOB_HAS_BLUR 1
#endif

namespace gm {
namespace knobs {

// ============================================================
// Tint bypass system
// ============================================================

inline bool& tintEnabledFlag() {
    static bool enabled = true;
    return enabled;
}

/// Scoped guard — disables all tinting within scope.
struct TintBypass {
    TintBypass()  { tintEnabledFlag() = false; }
    ~TintBypass() { tintEnabledFlag() = true; }
};

/// Apply theme tint to a body/surface color.
/// Returns base unchanged if tinting is bypassed or ThemeManager unavailable.
juce::Colour tintBody(juce::Colour base);


/// Apply theme accent shift to an arc/indicator color.
/// Blends toward the theme's accent color. Bypassed by TintBypass.
juce::Colour tintAccent(juce::Colour base);


/// Get themed arc gradient colors for gradient-arc styles.
/// If the theme defines arcGradientStart/End, uses those (full creative control).
/// Otherwise, falls back to tintAccent() on the style's defaults.
/// Bypassed by TintBypass — returns defaults unchanged.
inline std::pair<juce::Colour, juce::Colour> getArcGradient(
        juce::Colour defaultStart, juce::Colour defaultEnd) {
    if (!tintEnabledFlag()) return { defaultStart, defaultEnd };
#ifdef GM_KNOB_HAS_THEME
    auto& tm = gm::ThemeManager::getInstance();
    auto& colours = tm.getColours();
    auto itStart = colours.find("arcGradientStart");
    auto itEnd   = colours.find("arcGradientEnd");
    if (itStart != colours.end() && itEnd != colours.end())
        return { itStart->second, itEnd->second };
    return { tintAccent(defaultStart), tintAccent(defaultEnd) };
#else
    return { defaultStart, defaultEnd };
#endif
}

// ============================================================
// Shared polish helpers (used by any knob style)
// ============================================================

void drawHighlightCrescent(juce::Graphics &g, float cx, float cy,
    float radius, float alpha);


void drawBgArc(juce::Graphics &g, float cx, float cy, float arcR,
    float startA, float endA, float alpha,
    juce::Colour col = juce::Colour(0xff222230));


void drawBevel(juce::Graphics &g, float cx, float cy, float radius,
    float lightAlpha = 0.06f, float darkAlpha = 0.1f);


void drawEnvBand(juce::Graphics &g, float cx, float cy, float radius,
    float alpha = 0.04f);


void drawTopHighlight(juce::Graphics &g, float cx, float cy,
    float radius, float alpha = 0.06f);


void drawScreenHighlight(juce::Graphics &g, float cx, float cy,
    float radius, float offX, float offY, float alpha = 0.08f);


void drawRadialBrush(juce::Graphics &g, float cx, float cy,
    float radius, int numLines, juce::Colour col, float lineWidth = 0.5f);


void drawGrainNoise(juce::Graphics &g, float cx, float cy,
    float radius, juce::Colour col, float step = 2.0f, uint32_t seed = 42);


void drawShadow(juce::Graphics& g, float cx, float cy, float r, float alpha = 0.35f);


/// Accent glow around knob body -- real blur when available.
void drawKnobGlow(juce::Graphics& g, float cx, float cy, float r,
                           juce::Colour colour, float alpha = 0.15f);


void drawGlowArc(juce::Graphics &g, float cx, float cy, float arcR,
    float startA, float endA, juce::Colour col,
    float glowWidth = 4.0f, float coreWidth = 2.0f);


} // namespace knobs
} // namespace gm

// Include all knob style categories
#include "KnobStylesStandard.h"
#include "KnobStylesBipolar.h"
#include "KnobStylesStepped.h"
#include "KnobStylesEncoder.h"