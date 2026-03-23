#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "DarkMetallicTheme.h"
#include "ComboBoxStyles.h"
#include "LEDMeter.h"
#include "Knob.h"
#include "KnobStyles.h"
#include "HSlider.h"
#include "SliderStyles.h"
#include "EffectsDrawer.h"

// Joychord theme: DarkMetallic + ghostmoon Neon combo boxes + themed popups
struct JoychordTheme : public gm::DarkMetallicTheme {
    void drawComboBox (juce::Graphics& g, int w, int h, bool down,
                       int bx, int by, int bw, int bh, juce::ComboBox& cb) override {
        gm::combos::drawNeonComboBox (g, w, h, down, bx, by, bw, bh, cb);
    }
    void drawPopupMenuBackground (juce::Graphics& g, int w, int h) override {
        gm::combos::drawDarkPopupBackground (g, w, h);
    }
    void drawPopupMenuItem (juce::Graphics& g, const juce::Rectangle<int>& area,
                            bool isSep, bool isActive, bool isHL, bool isTicked,
                            bool hasSub, const juce::String& text, const juce::String& shortcut,
                            const juce::Drawable* icon, const juce::Colour* textCol) override {
        gm::combos::drawThemedPopupMenuItem (g, area, isSep, isActive, isHL, isTicked,
                                              hasSub, text, shortcut, icon, textCol);
    }
    void drawLinearSlider (juce::Graphics& g, int x, int y, int w, int h,
                           float sliderPos, float minPos, float maxPos,
                           juce::Slider::SliderStyle style, juce::Slider& slider) override {
        gm::sliders::drawPillTrackSlider (g, x, y, w, h, sliderPos, minPos, maxPos,
                                          style, slider, juce::Colour (0xff00ccff));
    }
};

class JoychordEditor : public juce::AudioProcessorEditor,
                       private juce::Timer
{
public:
    explicit JoychordEditor (JoychordProcessor&);
    ~JoychordEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void toggleDrawer();

    JoychordProcessor& processor;

    // APVTS-attached controls
    juce::ComboBox keyBox, scaleBox, voicingBox, octaveBox, synthModeBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> keyAttach, scaleAttach, voicingAttach, synthModeAttach;

    std::unique_ptr<gm::HSlider> strumSlider;

    juce::TextButton loadSfzBtn { "Load SFZ..." };

    // Preset selector
    juce::ComboBox presetBox;

    // Gamepad setup
    juce::ComboBox gamepadIndexBox;

    // Chord display
    juce::Label chordLabel;

    // Connection status
    juce::Label statusLabel;

    // Section labels
    juce::Label keyLabel, scaleLabel, voicingLabel, octaveLabel, presetLabel, gamepadLabel, synthModeLabel;

    // Button state
    bool btnA = false, btnB = false, btnX = false, btnY = false;
    bool dUp = false, dDown = false, dLeft = false, dRight = false;
    bool lb = false, rb = false;
    bool connected = false;

    // Ghostmoon UI
    JoychordTheme darkTheme;
    gm::LEDMeter meterL, meterR;

    // Main window macro knobs
    std::unique_ptr<gm::Knob> masterVolumeKnob;
    std::unique_ptr<gm::Knob> reverbMixKnob;
    std::unique_ptr<gm::Knob> filterCutoffKnob;

    // Section label for effects
    juce::Label effectsLabel;

    // Effects drawer
    juce::TextButton fxDrawerBtn { "FX" };
    std::unique_ptr<EffectsDrawer> effectsDrawer;
    bool drawerOpen = false;
    static constexpr int mainWidth = 520;
    static constexpr int drawerWidth = 180;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JoychordEditor)
};
