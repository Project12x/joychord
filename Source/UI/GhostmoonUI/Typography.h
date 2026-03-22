#pragma once
// ghostmoon UI Catalog: Typography (Joychord subset)
// Singleton font system providing semantic font accessors.
// Origin: UDS Typography.h (adapted for ghostmoon font kit).
//
// Core fonts: Inter-Regular, Inter-Bold, JetBrainsMono-Regular
// Requires BinaryData with these TTFs.

#include <BinaryData.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace gm {

class Typography {
public:
    static Typography& getInstance() {
        static Typography instance;
        return instance;
    }

    juce::Typeface::Ptr getInterRegular() const { return interRegular_; }
    juce::Typeface::Ptr getInterBold() const { return interBold_; }
    juce::Typeface::Ptr getJetBrainsMono() const { return jetBrainsMono_; }

    juce::Font getTitleFont(float height = 18.0f) const { return makeFont(interBold_, height); }
    juce::Font getHeaderFont(float height = 14.0f) const { return makeFont(interBold_, height); }
    juce::Font getLabelFont(float height = 12.0f) const { return makeFont(interRegular_, height); }
    juce::Font getButtonFont(float height = 11.0f) const { return makeFont(interBold_, height); }
    juce::Font getValueFont(float height = 11.0f) const { return makeFont(jetBrainsMono_, height); }
    juce::Font getCaptionFont(float height = 10.0f) const { return makeFont(interRegular_, height); }
    juce::Font getBrandFont(float height = 16.0f) const { return makeFont(interBold_, height); }
    juce::Font getRetroFont(float height = 8.0f) const { return makeFont(jetBrainsMono_, height); }

private:
    Typography() {
        interRegular_  = loadFont(BinaryData::InterRegular_ttf, BinaryData::InterRegular_ttfSize);
        interBold_     = loadFont(BinaryData::InterBold_ttf, BinaryData::InterBold_ttfSize);
        jetBrainsMono_ = loadFont(BinaryData::JetBrainsMonoRegular_ttf, BinaryData::JetBrainsMonoRegular_ttfSize);
    }

    static juce::Typeface::Ptr loadFont(const void* data, int size) {
        if (data != nullptr && size > 0)
            return juce::Typeface::createSystemTypefaceFor(data, static_cast<size_t>(size));
        return nullptr;
    }

    static juce::Font makeFont(juce::Typeface::Ptr tf, float height) {
        if (tf) return juce::Font(juce::FontOptions(tf).withHeight(height));
        return juce::Font(juce::FontOptions().withHeight(height));
    }

    juce::Typeface::Ptr interRegular_, interBold_, jetBrainsMono_;

    Typography(const Typography&) = delete;
    Typography& operator=(const Typography&) = delete;
};

} // namespace gm
