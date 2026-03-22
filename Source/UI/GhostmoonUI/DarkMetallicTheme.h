#pragma once
// ghostmoon UI Catalog: DarkMetallicTheme
// Origin: bondsp BoNLookAndFeel.h
// Full dark metallic color palette + 3D rotary knob + vertical fader rendering.
// Requires BinaryData with Inter + JetBrains Mono TTF fonts.
//
// Colors are read via findColour() using custom ColourIds. Defaults match the
// original dark metallic palette. ThemeManager can override any color by calling
// setColour() on the LookAndFeel instance.

#include <BinaryData.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace gm {

// Default color constants — used as ColourId defaults, not directly in rendering.
namespace Colors {
inline constexpr juce::uint32 bg = 0xff141418;
inline constexpr juce::uint32 bgGradTop = 0xff1c1c22;
inline constexpr juce::uint32 bgGradBottom = 0xff0e0e12;
inline constexpr juce::uint32 bgGrid = 0xff1e1e24;
inline constexpr juce::uint32 panelBg = 0xff1e1e24;
inline constexpr juce::uint32 panelGradTop = 0xff303038;
inline constexpr juce::uint32 panelHighlight = 0xff454550;
inline constexpr juce::uint32 panelBorder = 0xff383840;
inline constexpr juce::uint32 accent = 0xff00cccc;
inline constexpr juce::uint32 accentDim = 0xff008888;
inline constexpr juce::uint32 textPrimary = 0xffcccccc;
inline constexpr juce::uint32 textSecondary = 0xff888888;
inline constexpr juce::uint32 knobBody = 0xff2c2c32;
inline constexpr juce::uint32 knobBodyLight = 0xff3a3a42;
inline constexpr juce::uint32 knobBodyDark = 0xff202026;
inline constexpr juce::uint32 knobOutline = 0xff444448;
inline constexpr juce::uint32 knobShadow = 0xff0a0a0e;
inline constexpr juce::uint32 knobIndicator = 0xffe0e0e0;
inline constexpr juce::uint32 meterGreen = 0xff44cc44;
inline constexpr juce::uint32 meterYellow = 0xffcccc44;
inline constexpr juce::uint32 meterRed = 0xffcc4444;
inline constexpr juce::uint32 faderTrack = 0xff1a1a1e;
inline constexpr juce::uint32 faderThumb = 0xff555560;
} // namespace Colors

class DarkMetallicTheme : public juce::LookAndFeel_V4 {
public:
  // ColourIds for all theme-controllable colors in this LookAndFeel.
  // ThemeManager (or any caller) can override via setColour().
  enum ColourIds {
    bgColourId              = 0x3200000,
    panelColourId           = 0x3200001,
    panelHighlightColourId  = 0x3200002,
    panelBorderColourId     = 0x3200003,
    accentColourId          = 0x3200004,
    accentDimColourId       = 0x3200005,
    textPrimaryColourId     = 0x3200006,
    textSecondaryColourId   = 0x3200007,
    knobBodyLightColourId   = 0x3200008,
    knobBodyDarkColourId    = 0x3200009,
    knobOutlineColourId     = 0x320000A,
    knobShadowColourId      = 0x320000B,
    knobIndicatorColourId   = 0x320000C,
    faderTrackColourId      = 0x320000D,
    faderThumbColourId      = 0x320000E
  };

  DarkMetallicTheme() {
    interRegular = juce::Typeface::createSystemTypefaceFor(
        BinaryData::InterRegular_ttf, BinaryData::InterRegular_ttfSize);
    interBold = juce::Typeface::createSystemTypefaceFor(
        BinaryData::InterBold_ttf, BinaryData::InterBold_ttfSize);
    mono = juce::Typeface::createSystemTypefaceFor(
        BinaryData::JetBrainsMonoRegular_ttf,
        BinaryData::JetBrainsMonoRegular_ttfSize);

    // Register all defaults
    setColour(bgColourId,             juce::Colour(Colors::bg));
    setColour(panelColourId,          juce::Colour(Colors::panelBg));
    setColour(panelHighlightColourId, juce::Colour(Colors::panelGradTop));
    setColour(panelBorderColourId,    juce::Colour(Colors::panelBorder));
    setColour(accentColourId,         juce::Colour(Colors::accent));
    setColour(accentDimColourId,      juce::Colour(Colors::accentDim));
    setColour(textPrimaryColourId,    juce::Colour(Colors::textPrimary));
    setColour(textSecondaryColourId,  juce::Colour(Colors::textSecondary));
    setColour(knobBodyLightColourId,  juce::Colour(Colors::knobBodyLight));
    setColour(knobBodyDarkColourId,   juce::Colour(Colors::knobBodyDark));
    setColour(knobOutlineColourId,    juce::Colour(Colors::knobOutline));
    setColour(knobShadowColourId,     juce::Colour(Colors::knobShadow));
    setColour(knobIndicatorColourId,  juce::Colour(Colors::knobIndicator));
    setColour(faderTrackColourId,     juce::Colour(Colors::faderTrack));
    setColour(faderThumbColourId,     juce::Colour(Colors::faderThumb));

    // Standard JUCE ColourIds
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(Colors::bg));
    setColour(juce::Label::textColourId, juce::Colour(Colors::textPrimary));
    setColour(juce::Slider::textBoxTextColourId, juce::Colour(Colors::textPrimary));
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
  }

  /// Apply colors from ThemeManager to this LookAndFeel's ColourIds.
  /// Call this when the theme changes.
  /// Apply colors from ThemeManager to this LookAndFeel's ColourIds.
  /// Accent/text/panel colors are fully overridden.
  /// Body/surface colors (knob body, fader thumb, indicator) are TINTED —
  /// blended toward the theme tone while preserving the style's character.
  void applyTheme(const std::map<juce::String, juce::Colour>& colours,
                  juce::Colour tintColour = juce::Colour(0xff141418),
                  float tintAmount = 0.0f) {
    // Full override: UI chrome colors
    auto apply = [&](ColourIds id, const char* key) {
      auto it = colours.find(key);
      if (it != colours.end()) setColour(id, it->second);
    };
    apply(bgColourId,             "background");
    apply(panelColourId,          "panel");
    apply(panelHighlightColourId, "panelHighlight");
    apply(panelBorderColourId,    "panelBorder");
    apply(accentColourId,         "accent");
    apply(accentDimColourId,      "accentDim");
    apply(textPrimaryColourId,    "textPrimary");
    apply(textSecondaryColourId,  "textSecondary");
    apply(faderTrackColourId,     "faderTrack");

    // Tinted: body/surface colors — blend defaults toward theme tone
    auto tint = [&](ColourIds id, juce::uint32 defaultVal) {
      auto base = juce::Colour(defaultVal);
      setColour(id, (tintAmount > 0.0f)
          ? base.interpolatedWith(tintColour, tintAmount)
          : base);
    };
    tint(knobBodyLightColourId,  Colors::knobBodyLight);
    tint(knobBodyDarkColourId,   Colors::knobBodyDark);
    tint(knobOutlineColourId,    Colors::knobOutline);
    tint(knobShadowColourId,     Colors::knobShadow);
    tint(knobIndicatorColourId,  Colors::knobIndicator);
    tint(faderThumbColourId,     Colors::faderThumb);

    // Standard JUCE ColourIds
    auto it = colours.find("background");
    if (it != colours.end())
      setColour(juce::ResizableWindow::backgroundColourId, it->second);
    it = colours.find("textPrimary");
    if (it != colours.end()) {
      setColour(juce::Label::textColourId, it->second);
      setColour(juce::Slider::textBoxTextColourId, it->second);
    }
  }

  juce::Font getInterFont(float height, bool bold = false) const {
    auto tf = bold ? interBold : interRegular;
    if (!tf) return juce::Font(height);
    return juce::Font(juce::FontOptions(tf).withHeight(height));
  }

  juce::Font getMonoFont(float height) const {
    if (!mono) return juce::Font(height);
    return juce::Font(juce::FontOptions(mono).withHeight(height));
  }

  juce::Font getLabelFont(juce::Label &) override {
    return getInterFont(12.0f);
  }

  void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                        float sliderPos, float rotaryStartAngle,
                        float rotaryEndAngle, juce::Slider &) override {
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto cx = bounds.getCentreX();
    auto cy = bounds.getCentreY();

    // Drop shadow
    g.setColour(findColour(knobShadowColourId).withAlpha(0.5f));
    g.fillEllipse(cx - radius + 1.0f, cy - radius + 2.0f, radius * 2.0f, radius * 2.0f);

    // Knob body (radial gradient, metallic 3D)
    {
      juce::ColourGradient bodyGrad(
          findColour(knobBodyLightColourId), cx, cy - radius,
          findColour(knobBodyDarkColourId), cx, cy + radius, false);
      g.setGradientFill(bodyGrad);
      g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
    }

    // Beveled ring
    g.setColour(findColour(knobOutlineColourId));
    g.drawEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f, 1.5f);

    // Inner highlight arc
    {
      juce::Path highlightArc;
      highlightArc.addCentredArc(cx, cy, radius - 1.5f, radius - 1.5f, 0.0f,
                                 -juce::MathConstants<float>::pi * 0.8f,
                                 juce::MathConstants<float>::pi * 0.8f, true);
      g.setColour(juce::Colours::white.withAlpha(0.04f));
      g.strokePath(highlightArc, juce::PathStrokeType(1.0f));
    }

    // Arc track (background)
    auto arcRadius = radius - 4.0f;
    juce::Path bgArc;
    bgArc.addCentredArc(cx, cy, arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(findColour(panelColourId));
    g.strokePath(bgArc, juce::PathStrokeType(3.5f));

    // Value arc with glow
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    juce::Path valueArc;
    valueArc.addCentredArc(cx, cy, arcRadius, arcRadius, 0.0f, rotaryStartAngle, angle, true);
    auto acc = findColour(accentColourId);
    g.setColour(acc.withAlpha(0.08f));
    g.strokePath(valueArc, juce::PathStrokeType(9.0f));
    g.setColour(acc.withAlpha(0.18f));
    g.strokePath(valueArc, juce::PathStrokeType(6.0f));
    g.setColour(acc);
    g.strokePath(valueArc, juce::PathStrokeType(3.5f));

    // Pointer line
    auto pointerLen = radius - 6.0f;
    auto pointerAngle = angle - juce::MathConstants<float>::halfPi;
    auto innerDist = radius * 0.3f;
    auto px1 = cx + innerDist * std::cos(pointerAngle);
    auto py1 = cy + innerDist * std::sin(pointerAngle);
    auto px2 = cx + pointerLen * std::cos(pointerAngle);
    auto py2 = cy + pointerLen * std::sin(pointerAngle);
    g.setColour(findColour(knobIndicatorColourId));
    g.drawLine(px1, py1, px2, py2, 2.0f);
    g.fillEllipse(px2 - 1.5f, py2 - 1.5f, 3.0f, 3.0f);
  }

  // ============================================================
  // Styled Toggle/Text Buttons (pill shape, glow when ON)
  // ============================================================

  void drawButtonBackground(juce::Graphics &g, juce::Button &button,
                            const juce::Colour &, bool isHighlighted,
                            bool isDown) override {
    auto bounds = button.getLocalBounds().toFloat();
    auto cornerRadius = juce::jmin(bounds.getHeight() * 0.4f, 8.0f);
    bool isToggle = button.getClickingTogglesState();
    bool isOn = button.getToggleState();

    if (isToggle) {
      juce::Colour bgColor;
      if (isOn) {
        bgColor = button.findColour(juce::TextButton::buttonOnColourId);
        if (bgColor == juce::LookAndFeel::getDefaultLookAndFeel()
                           .findColour(juce::TextButton::buttonOnColourId))
          bgColor = findColour(accentColourId);
        if (isDown) bgColor = bgColor.darker(0.2f);
      } else {
        bgColor = findColour(panelColourId);
        if (isHighlighted) bgColor = findColour(panelHighlightColourId);
        if (isDown) bgColor = bgColor.brighter(0.1f);
      }

      if (isOn) {
        g.setColour(bgColor.withAlpha(0.3f));
        g.fillRoundedRectangle(bounds.expanded(1.5f), cornerRadius + 1.5f);
      }

      g.setColour(bgColor);
      g.fillRoundedRectangle(bounds, cornerRadius);

      if (isOn) {
        g.setColour(juce::Colours::white.withAlpha(0.12f));
        g.drawRoundedRectangle(bounds.reduced(0.5f), cornerRadius, 1.0f);
      } else {
        g.setColour(findColour(panelBorderColourId).withAlpha(0.5f));
        g.drawRoundedRectangle(bounds, cornerRadius, 1.0f);
      }
    } else {
      auto bgColor = button.findColour(juce::TextButton::buttonColourId);
      if (bgColor == juce::LookAndFeel::getDefaultLookAndFeel()
                         .findColour(juce::TextButton::buttonColourId))
        bgColor = findColour(panelColourId);
      if (isDown)
        bgColor = bgColor.darker(0.2f);
      else if (isHighlighted)
        bgColor = bgColor.brighter(0.15f);

      g.setColour(bgColor);
      g.fillRoundedRectangle(bounds, cornerRadius);
      g.setColour(findColour(panelBorderColourId));
      g.drawRoundedRectangle(bounds, cornerRadius, 1.0f);
    }
  }

  void drawButtonText(juce::Graphics &g, juce::TextButton &button,
                      bool, bool) override {
    auto font = getInterFont(juce::jmax(10.0f, button.getHeight() * 0.45f));
    g.setFont(font);

    bool isOn = button.getToggleState() && button.getClickingTogglesState();
    auto textColour = isOn ? button.findColour(juce::TextButton::textColourOnId)
                           : button.findColour(juce::TextButton::textColourOffId);

    if (!button.isEnabled())
      textColour = textColour.withAlpha(0.4f);

    g.setColour(textColour);
    g.drawText(button.getButtonText(), button.getLocalBounds(),
               juce::Justification::centred, false);
  }

  // ============================================================
  // Premium ComboBox
  // ============================================================

  void drawComboBox(juce::Graphics &g, int width, int height,
                    bool, int buttonX, int buttonY, int buttonW, int buttonH,
                    juce::ComboBox &box) override {
    auto cornerSize = 5.0f;
    auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat().reduced(0.5f);

    bool isHovered = box.isMouseOver();
    auto bgColor = isHovered ? findColour(panelHighlightColourId) : findColour(panelColourId);
    g.setColour(bgColor);
    g.fillRoundedRectangle(bounds, cornerSize);

    auto borderColor = box.hasKeyboardFocus(true) ? findColour(accentColourId) : findColour(panelBorderColourId);
    g.setColour(borderColor);
    g.drawRoundedRectangle(bounds, cornerSize, 1.0f);

    // Chevron arrow
    auto arrowZone = juce::Rectangle<int>(buttonX, buttonY, buttonW, buttonH).toFloat();
    auto cx = arrowZone.getCentreX();
    auto cy = arrowZone.getCentreY();
    juce::Path chevron;
    chevron.startNewSubPath(cx - 5.0f, cy - 3.0f);
    chevron.lineTo(cx, cy + 3.0f);
    chevron.lineTo(cx + 5.0f, cy - 3.0f);
    g.setColour(findColour(accentColourId));
    g.strokePath(chevron, juce::PathStrokeType(1.5f));
  }

  // ============================================================
  // PopupMenu item
  // ============================================================

  void drawPopupMenuItem(juce::Graphics &g, const juce::Rectangle<int> &area,
                         bool isSeparator, bool isActive, bool isHighlighted,
                         bool isTicked, bool,
                         const juce::String &text, const juce::String &,
                         const juce::Drawable *, const juce::Colour *) override {
    if (isSeparator) {
      auto sepArea = area.reduced(8, 0);
      g.setColour(findColour(panelBorderColourId).withAlpha(0.5f));
      g.fillRect(sepArea.getX(), sepArea.getCentreY(), sepArea.getWidth(), 1);
      return;
    }

    auto textArea = area.reduced(8, 0);

    if (isHighlighted && isActive) {
      juce::ColourGradient highlightGrad(
          findColour(accentColourId), (float)area.getX(), (float)area.getY(),
          findColour(accentDimColourId), (float)area.getRight(), (float)area.getY(), false);
      g.setGradientFill(highlightGrad);
      g.fillRoundedRectangle(area.toFloat().reduced(2, 1), 3.0f);
      g.setColour(findColour(bgColourId));
    } else {
      g.setColour(isActive ? findColour(textPrimaryColourId) : findColour(textSecondaryColourId));
    }

    g.setFont(getInterFont(12.0f));

    if (isTicked) {
      auto tickArea = textArea.removeFromLeft(16);
      g.setColour(findColour(accentColourId));
      auto tick = tickArea.toFloat().reduced(3);
      g.drawLine(tick.getX() + 2, tick.getCentreY(), tick.getCentreX(), tick.getBottom() - 2, 1.5f);
      g.drawLine(tick.getCentreX(), tick.getBottom() - 2, tick.getRight() - 2, tick.getY() + 2, 1.5f);
    }

    g.drawText(text, textArea, juce::Justification::centredLeft, true);
  }

  // ============================================================
  // Vertical Fader
  // ============================================================

  void drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height,
                        float sliderPos, float minSliderPos, float maxSliderPos,
                        juce::Slider::SliderStyle style, juce::Slider &slider) override {
    if (style != juce::Slider::LinearVertical) {
      juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos,
                                             minSliderPos, maxSliderPos, style, slider);
      return;
    }

    auto trackWidth = 6.0f;
    auto trackX = (float)x + (float)width * 0.5f - trackWidth * 0.5f;

    g.setColour(findColour(faderTrackColourId));
    g.fillRoundedRectangle(trackX, (float)y, trackWidth, (float)height, 3.0f);

    auto fillHeight = (float)(y + height) - sliderPos;
    if (fillHeight > 0.0f) {
      g.setColour(findColour(accentColourId));
      g.fillRoundedRectangle(trackX, sliderPos, trackWidth, fillHeight, 3.0f);
    }

    auto thumbW = 16.0f;
    auto thumbH = 8.0f;
    g.setColour(findColour(faderThumbColourId));
    g.fillRoundedRectangle((float)x + (float)width * 0.5f - thumbW * 0.5f,
                           sliderPos - thumbH * 0.5f, thumbW, thumbH, 2.0f);
    g.setColour(findColour(accentColourId).withAlpha(0.6f));
    g.drawRoundedRectangle((float)x + (float)width * 0.5f - thumbW * 0.5f,
                           sliderPos - thumbH * 0.5f, thumbW, thumbH, 2.0f, 1.0f);
  }

  // ============================================================
  // Tooltip
  // ============================================================

  void drawTooltip(juce::Graphics& g, const juce::String& text,
                   int width, int height) override {
    auto bounds = juce::Rectangle<float>(0, 0, (float)width, (float)height);

    g.setColour(findColour(panelHighlightColourId));
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(findColour(accentColourId).withAlpha(0.5f));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 4.0f, 1.0f);

    g.setColour(findColour(textPrimaryColourId));
    g.setFont(getInterFont(11.0f));
    g.drawText(text, 0, 0, width, height, juce::Justification::centred);
  }

  // ============================================================
  // Scrollbar
  // ============================================================

  void drawScrollbar(juce::Graphics& g, juce::ScrollBar&,
                     int x, int y, int width, int height,
                     bool isScrollbarVertical, int thumbStartPosition,
                     int thumbSize, bool isMouseOver, bool isMouseDown) override {
    const float thumbInset = 1.0f;
    const float cornerRadius = 3.0f;

    juce::Colour thumbCol;
    if (isMouseDown)
      thumbCol = findColour(accentColourId).withAlpha(0.7f);
    else if (isMouseOver)
      thumbCol = findColour(textPrimaryColourId).withAlpha(0.35f);
    else
      thumbCol = findColour(textPrimaryColourId).withAlpha(0.18f);

    if (thumbSize > 0) {
      juce::Rectangle<float> thumbBounds;
      if (isScrollbarVertical)
        thumbBounds = { (float)x + thumbInset, (float)thumbStartPosition,
                        (float)width - thumbInset * 2.0f, (float)thumbSize };
      else
        thumbBounds = { (float)thumbStartPosition, (float)y + thumbInset,
                        (float)thumbSize, (float)height - thumbInset * 2.0f };

      g.setColour(thumbCol);
      g.fillRoundedRectangle(thumbBounds, cornerRadius);
    }
  }

  // ============================================================
  // ProgressBar
  // ============================================================

  void drawProgressBar(juce::Graphics& g, juce::ProgressBar&,
                       int width, int height, double progress,
                       const juce::String& textToShow) override {
    float cornerRadius = 4.0f;
    juce::Rectangle<float> bounds(0.0f, 0.0f, (float)width, (float)height);

    auto bgCol = findColour(bgColourId);
    juce::ColourGradient trackGrad(bgCol.darker(0.3f), 0.0f, 0.0f,
                                    bgCol.darker(0.15f), 0.0f, (float)height, false);
    g.setGradientFill(trackGrad);
    g.fillRoundedRectangle(bounds, cornerRadius);

    g.setColour(juce::Colour(0x25000000));
    g.drawHorizontalLine(1, 2.0f, (float)width - 2.0f);

    g.setColour(juce::Colour(0x40000000));
    g.drawRoundedRectangle(bounds.reduced(0.5f), cornerRadius, 1.0f);

    if (progress > 0.0) {
      float fillWidth = juce::jmax(cornerRadius * 2.0f, (float)(width - 2) * (float)progress);
      juce::Rectangle<float> fillBounds(1.0f, 1.0f, fillWidth, (float)height - 2.0f);

      auto meterCol = findColour(accentColourId);

      juce::ColourGradient fillGrad(meterCol.brighter(0.2f), 0.0f, fillBounds.getY(),
                                     meterCol.darker(0.1f), 0.0f, fillBounds.getBottom(), false);
      g.setGradientFill(fillGrad);
      g.fillRoundedRectangle(fillBounds, cornerRadius - 1.0f);

      juce::ColourGradient sheen(juce::Colours::white.withAlpha(0.2f), 0.0f, fillBounds.getY(),
                                  juce::Colours::white.withAlpha(0.0f), 0.0f, fillBounds.getCentreY(), false);
      g.setGradientFill(sheen);
      g.fillRoundedRectangle(fillBounds.reduced(1.0f), cornerRadius - 2.0f);

      g.setColour(meterCol.brighter(0.5f).withAlpha(0.6f));
      g.drawVerticalLine((int)fillBounds.getRight() - 1, fillBounds.getY() + 2.0f, fillBounds.getBottom() - 2.0f);
    }

    if (textToShow.isNotEmpty()) {
      g.setColour(findColour(textPrimaryColourId));
      g.setFont(getInterFont(11.0f));
      g.drawText(textToShow, bounds.toNearestInt(), juce::Justification::centred, true);
    }
  }

private:
  juce::Typeface::Ptr interRegular, interBold, mono;
};

} // namespace gm
