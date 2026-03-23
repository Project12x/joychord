#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "DarkMetallicTheme.h"
#include "ComboBoxStyles.h"
#include "LEDMeter.h"
#include "Knob.h"
#include "KnobStyles.h"

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

    JoychordProcessor& processor;

    // APVTS-attached controls
    juce::ComboBox keyBox, scaleBox, voicingBox, octaveBox, synthModeBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> keyAttach, scaleAttach, voicingAttach, synthModeAttach;

    juce::Slider strumSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> strumAttach;

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
    juce::Label keyLabel, scaleLabel, voicingLabel, octaveLabel, presetLabel, gamepadLabel, synthModeLabel, strumLabel;

    // Button state
    bool btnA = false, btnB = false, btnX = false, btnY = false;
    bool dUp = false, dDown = false, dLeft = false, dRight = false;
    bool lb = false, rb = false;
    bool connected = false;

    // Ghostmoon UI
    JoychordTheme darkTheme;
    gm::LEDMeter meterL, meterR;

    // LED Ladder knob LookAndFeel for master volume
    struct LedLadderLnF : public juce::LookAndFeel_V4 {
        void drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h,
                               float pos, float startA, float endA,
                               juce::Slider& s) override {
            gm::knobs::drawLedLadderKnob (g, x, y, w, h, pos, startA, endA, s,
                                           juce::Colour (0xff00ccff));  // neon cyan
        }
    } ledLadderLnF;

    std::unique_ptr<gm::Knob> masterVolumeKnob;

    // Effects knobs
    std::unique_ptr<gm::Knob> reverbDecayKnob, reverbDampKnob, reverbMixKnob;
    std::unique_ptr<gm::Knob> filterCutoffKnob, filterResKnob;
    std::unique_ptr<gm::Knob> chorusRateKnob, chorusMixKnob;

    // Section label for effects
    juce::Label effectsLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JoychordEditor)
};
