#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "DarkMetallicTheme.h"
#include "LEDMeter.h"
#include "Knob.h"
#include "KnobStyles.h"

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
    gm::DarkMetallicTheme darkTheme;
    gm::LEDMeter meterL, meterR;

    // LED Ladder knob LookAndFeel for master volume
    struct LedLadderLnF : public gm::DarkMetallicTheme {
        using gm::DarkMetallicTheme::DarkMetallicTheme;
        void drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h,
                               float pos, float startA, float endA,
                               juce::Slider& s) override {
            gm::knobs::drawLedLadderKnob (g, x, y, w, h, pos, startA, endA, s,
                                           findColour (accentColourId));
        }
    } ledLadderLnF;

    std::unique_ptr<gm::Knob> masterVolumeKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JoychordEditor)
};
