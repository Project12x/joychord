#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

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
    juce::ComboBox keyBox, scaleBox, voicingBox, octaveBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> keyAttach, scaleAttach, voicingAttach;

    // Chord display
    juce::Label chordLabel;

    // Connection status
    juce::Label statusLabel;

    // Section labels
    juce::Label keyLabel, scaleLabel, voicingLabel, octaveLabel, gamepadLabel;

    // Button state (painted directly)
    bool btnA = false, btnB = false, btnX = false, btnY = false;
    bool connected = false;

    // Gamepad setup
    juce::ComboBox gamepadIndexBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JoychordEditor)
};
