#pragma once
// ghostmoon UI Catalog: Typography
// Singleton font system providing semantic font accessors.
// Origin: UDS Typography.h (adapted for ghostmoon font kit).
//
// Font hierarchy:
//   Inter Bold:           Titles, headers
//   Inter Regular:        Labels, body text, captions
//   JetBrains Mono:       Numerical values, parameter readouts
//   Lato Bold:            Alternative headers
//   SpaceGrotesk Bold:    Branding, plugin titles
//   PressStart2P:         Retro/pixel art displays
//
// Initialization:
//   If the full ghostmoon font kit is in BinaryData, fonts auto-load.
//   Consumer projects embedding a subset should define GM_TYPOGRAPHY_MANUAL_INIT
//   and call the setXxx() methods at startup instead.

#if !defined(GM_TYPOGRAPHY_MANUAL_INIT) && __has_include(<BinaryData.h>)
  #include <BinaryData.h>
  #define GM_TYPOGRAPHY_AUTO_LOAD_ 1
#endif

#include <juce_gui_basics/juce_gui_basics.h>

namespace gm {

class Typography {
public:
    static Typography& getInstance() {
        static Typography instance;
        return instance;
    }

    // ==================== Typeface Setters ====================
    // Use these when the consumer embeds a font subset, or when
    // GM_TYPOGRAPHY_MANUAL_INIT is defined.

    void setInterRegular     (juce::Typeface::Ptr tf) { interRegular_      = tf; }
    void setInterBold        (juce::Typeface::Ptr tf) { interBold_         = tf; }
    void setJetBrainsMono    (juce::Typeface::Ptr tf) { jetBrainsMono_     = tf; }
    void setJetBrainsMonoBold(juce::Typeface::Ptr tf) { jetBrainsMonoBold_ = tf; }
    void setLatoRegular      (juce::Typeface::Ptr tf) { latoRegular_       = tf; }
    void setLatoBold         (juce::Typeface::Ptr tf) { latoBold_          = tf; }
    void setSpaceGroteskBold (juce::Typeface::Ptr tf) { spaceGroteskBold_  = tf; }
    void setPressStart2P     (juce::Typeface::Ptr tf) { pressStart2P_      = tf; }

    // ==================== Raw Typeface Access ====================

    juce::Typeface::Ptr getInterRegular() const { return interRegular_; }
    juce::Typeface::Ptr getInterBold() const { return interBold_; }
    juce::Typeface::Ptr getJetBrainsMono() const { return jetBrainsMono_; }
    juce::Typeface::Ptr getJetBrainsMonoBold() const { return jetBrainsMonoBold_; }
    juce::Typeface::Ptr getLatoRegular() const { return latoRegular_; }
    juce::Typeface::Ptr getLatoBold() const { return latoBold_; }
    juce::Typeface::Ptr getSpaceGroteskBold() const { return spaceGroteskBold_; }
    juce::Typeface::Ptr getPressStart2P() const { return pressStart2P_; }

    // ==================== Semantic Font Accessors ====================

    /// Plugin title, large headers — Inter Bold
    juce::Font getTitleFont(float height = 18.0f) const {
        return makeFont(interBold_, height);
    }

    /// Section headers, sub-headers — Inter Bold
    juce::Font getHeaderFont(float height = 14.0f) const {
        return makeFont(interBold_, height);
    }

    /// Labels, body text — Inter Regular
    juce::Font getLabelFont(float height = 12.0f) const {
        return makeFont(interRegular_, height);
    }

    /// Button text — Inter Bold, slightly smaller
    juce::Font getButtonFont(float height = 11.0f) const {
        return makeFont(interBold_, height);
    }

    /// Numerical values, parameter readouts — JetBrains Mono
    juce::Font getValueFont(float height = 11.0f) const {
        return makeFont(jetBrainsMono_, height);
    }

    /// Small captions, footnotes — Inter Regular
    juce::Font getCaptionFont(float height = 10.0f) const {
        return makeFont(interRegular_, height);
    }

    /// Branding, plugin name — Space Grotesk Bold
    juce::Font getBrandFont(float height = 16.0f) const {
        return makeFont(spaceGroteskBold_, height);
    }

    /// Retro/pixel art displays — Press Start 2P
    juce::Font getRetroFont(float height = 8.0f) const {
        return makeFont(pressStart2P_, height);
    }

private:
    Typography() {
#ifdef GM_TYPOGRAPHY_AUTO_LOAD_
        interRegular_     = loadFont(BinaryData::InterRegular_ttf, BinaryData::InterRegular_ttfSize);
        interBold_        = loadFont(BinaryData::InterBold_ttf, BinaryData::InterBold_ttfSize);
        jetBrainsMono_    = loadFont(BinaryData::JetBrainsMonoRegular_ttf, BinaryData::JetBrainsMonoRegular_ttfSize);
        jetBrainsMonoBold_= loadFont(BinaryData::JetBrainsMonoBold_ttf, BinaryData::JetBrainsMonoBold_ttfSize);
        latoRegular_      = loadFont(BinaryData::LatoRegular_ttf, BinaryData::LatoRegular_ttfSize);
        latoBold_         = loadFont(BinaryData::LatoBold_ttf, BinaryData::LatoBold_ttfSize);
        spaceGroteskBold_ = loadFont(BinaryData::SpaceGroteskBold_ttf, BinaryData::SpaceGroteskBold_ttfSize);
        pressStart2P_     = loadFont(BinaryData::PressStart2PRegular_ttf, BinaryData::PressStart2PRegular_ttfSize);
#endif
    }

    static juce::Typeface::Ptr loadFont(const void* data, int size) {
        if (data != nullptr && size > 0)
            return juce::Typeface::createSystemTypefaceFor(data, static_cast<size_t>(size));
        return nullptr;
    }

    static juce::Font makeFont(juce::Typeface::Ptr tf, float height) {
        if (tf)
            return juce::Font(juce::FontOptions(tf).withHeight(height));
        return juce::Font(juce::FontOptions().withHeight(height));
    }

    juce::Typeface::Ptr interRegular_, interBold_;
    juce::Typeface::Ptr jetBrainsMono_, jetBrainsMonoBold_;
    juce::Typeface::Ptr latoRegular_, latoBold_;
    juce::Typeface::Ptr spaceGroteskBold_;
    juce::Typeface::Ptr pressStart2P_;

    Typography(const Typography&) = delete;
    Typography& operator=(const Typography&) = delete;
};

} // namespace gm
