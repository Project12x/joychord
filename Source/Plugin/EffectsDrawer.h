#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "Knob.h"
#include "KnobStyles.h"

// Sliding effects drawer containing all effect controls + toggles.
// Lives as a child of JoychordEditor, positioned at the right edge.
class EffectsDrawer : public juce::Component {
public:
    EffectsDrawer (juce::AudioProcessorValueTreeState& apvts)
    {
        // Toggle buttons
        auto makeToggle = [&](juce::ToggleButton& btn, const juce::String& label,
                              const juce::String& paramId) {
            btn.setButtonText (label);
            btn.setColour (juce::ToggleButton::textColourId, juce::Colour (0xffaaaaaa));
            btn.setColour (juce::ToggleButton::tickColourId, juce::Colour (0xff00cccc));
            addAndMakeVisible (btn);
            toggleAttachments.push_back (
                std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
                    apvts, paramId, btn));
        };
        makeToggle (filterToggle,  "Filter",  "filterEnabled");
        makeToggle (chorusToggle,  "Chorus",  "chorusEnabled");
        makeToggle (reverbToggle,  "Reverb",  "reverbEnabled");

        // Knobs
        auto makeKnob = [&](std::unique_ptr<gm::Knob>& knob,
                            const juce::String& paramId, const juce::String& label) {
            knob = std::make_unique<gm::Knob> (apvts, paramId, label);
            knob->setStyle (gm::KnobStyle::NeonArc);
            addAndMakeVisible (*knob);
        };

        // Filter section
        makeKnob (filterCutoffKnob, "filterCutoff", "Cutoff");
        makeKnob (filterResKnob,    "filterRes",    "Reso");

        // Chorus section
        makeKnob (chorusRateKnob, "chorusRate", "Rate");
        makeKnob (chorusMixKnob,  "chorusMix",  "Chorus");

        // Reverb section
        makeKnob (reverbDecayKnob, "reverbDecay", "Decay");
        makeKnob (reverbDampKnob,  "reverbDamp",  "Damp");
        makeKnob (reverbMixKnob,   "reverbMix",   "Reverb");

        // Section labels
        auto makeLabel = [&](juce::Label& lbl, const juce::String& text) {
            lbl.setText (text, juce::dontSendNotification);
            lbl.setColour (juce::Label::textColourId, juce::Colour (0xff00cccc));
            lbl.setFont (juce::Font (juce::FontOptions (11.0f)));
            lbl.setJustificationType (juce::Justification::centredLeft);
            addAndMakeVisible (lbl);
        };
        makeLabel (filterLabel,  "FILTER");
        makeLabel (chorusLabel,  "CHORUS");
        makeLabel (reverbLabel,  "REVERB");
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colour (0xff111118));

        // Left edge separator line
        g.setColour (juce::Colour (0xff00cccc).withAlpha (0.3f));
        g.drawLine (0.0f, 0.0f, 0.0f, static_cast<float> (getHeight()), 1.0f);

        // Section dividers
        g.setColour (juce::Colour (0xff333340));
        int divY1 = filterLabel.getBottom() + 78;
        int divY2 = chorusLabel.getBottom() + 78;
        g.drawLine (8.0f, (float)divY1, (float)(getWidth() - 8), (float)divY1, 1.0f);
        g.drawLine (8.0f, (float)divY2, (float)(getWidth() - 8), (float)divY2, 1.0f);
    }

    void resized() override
    {
        auto b = getLocalBounds().reduced (8, 6);
        int knobSize = 70;
        int toggleH = 22;
        int sectionLabelH = 14;
        int sectionGap = 6;

        // -- FILTER --
        filterToggle.setBounds (b.removeFromTop (toggleH));
        filterLabel.setBounds (b.removeFromTop (sectionLabelH));
        b.removeFromTop (2);
        {
            auto row = b.removeFromTop (knobSize);
            filterCutoffKnob->setBounds (row.removeFromLeft (knobSize));
            row.removeFromLeft (4);
            filterResKnob->setBounds (row.removeFromLeft (knobSize));
        }
        b.removeFromTop (sectionGap);

        // -- CHORUS --
        chorusToggle.setBounds (b.removeFromTop (toggleH));
        chorusLabel.setBounds (b.removeFromTop (sectionLabelH));
        b.removeFromTop (2);
        {
            auto row = b.removeFromTop (knobSize);
            chorusRateKnob->setBounds (row.removeFromLeft (knobSize));
            row.removeFromLeft (4);
            chorusMixKnob->setBounds (row.removeFromLeft (knobSize));
        }
        b.removeFromTop (sectionGap);

        // -- REVERB --
        reverbToggle.setBounds (b.removeFromTop (toggleH));
        reverbLabel.setBounds (b.removeFromTop (sectionLabelH));
        b.removeFromTop (2);
        {
            auto row = b.removeFromTop (knobSize);
            reverbDecayKnob->setBounds (row.removeFromLeft (knobSize));
            row.removeFromLeft (4);
            reverbDampKnob->setBounds (row.removeFromLeft (knobSize));
        }
        b.removeFromTop (2);
        {
            auto row = b.removeFromTop (knobSize);
            reverbMixKnob->setBounds (row.removeFromLeft (knobSize));
        }
    }

private:
    // Toggles
    juce::ToggleButton filterToggle, chorusToggle, reverbToggle;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> toggleAttachments;

    // Section labels
    juce::Label filterLabel, chorusLabel, reverbLabel;

    // Knobs
    std::unique_ptr<gm::Knob> filterCutoffKnob, filterResKnob;
    std::unique_ptr<gm::Knob> chorusRateKnob, chorusMixKnob;
    std::unique_ptr<gm::Knob> reverbDecayKnob, reverbDampKnob, reverbMixKnob;
};
