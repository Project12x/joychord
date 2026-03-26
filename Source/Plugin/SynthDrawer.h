#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "CollapsibleComboBox.h"
#include "Knob.h"
#include "KnobStyles.h"
#include "ButtonStyles.h"
#include "BinaryData.h"

// Sliding synth settings drawer - same format as EffectsDrawer.
// Sections: Oscillator -> Unison -> Sub Oscillator -> Envelope (AHDSR) -> Portamento -> Drift
class SynthDrawer : public juce::Component {
public:
    SynthDrawer (juce::AudioProcessorValueTreeState& apvts)
    {
        content = std::make_unique<Content> (apvts);
        viewport.setViewedComponent (content.get(), false);
        viewport.setScrollBarsShown (true, false);
        viewport.setScrollBarThickness (6);
        addAndMakeVisible (viewport);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colour (0xff111118));

        if (! drawerTile.isValid())
            drawerTile = juce::ImageCache::getFromMemory (
                BinaryData::tile_brushed_metal_png, BinaryData::tile_brushed_metal_pngSize);
        if (drawerTile.isValid())
        {
            int tw = drawerTile.getWidth();
            int th = drawerTile.getHeight();
            g.saveState();
            g.reduceClipRegion (getLocalBounds());
            g.setOpacity (0.15f);
            for (int ty = 0; ty < getHeight(); ty += th)
                for (int tx = 0; tx < getWidth(); tx += tw)
                    g.drawImageAt (drawerTile, tx, ty);
            g.restoreState();
        }

        // Right edge separator line
        g.setColour (juce::Colour (0xff00cccc).withAlpha (0.3f));
        g.drawLine (static_cast<float> (getWidth()), 0.0f,
                    static_cast<float> (getWidth()), static_cast<float> (getHeight()), 1.0f);
    }

    void resized() override
    {
        viewport.setBounds (getLocalBounds());
        content->setSize (getWidth() - 8, content->getTotalHeight());
        content->resized();
        content->setSize (getWidth() - 8, content->getTotalHeight());
    }

private:
    class Content : public juce::Component {
    public:
        Content (juce::AudioProcessorValueTreeState& apvts)
        {
            auto makeKnob = [&](std::unique_ptr<gm::Knob>& knob,
                                const juce::String& paramId, const juce::String& label) {
                knob = std::make_unique<gm::Knob> (apvts, paramId, label);
                knob->setStyle (gm::KnobStyle::NeonArc);
                addAndMakeVisible (*knob);
            };

            auto makeLabel = [&](juce::Label& lbl, const juce::String& text) {
                lbl.setText (text, juce::dontSendNotification);
                lbl.setColour (juce::Label::textColourId, juce::Colour (0xff00cccc));
                lbl.setFont (juce::Font (juce::FontOptions (11.0f)));
                lbl.setJustificationType (juce::Justification::centredLeft);
                addAndMakeVisible (lbl);
            };

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

            // Oscillator section - waveshape combo box
            makeLabel (oscLabel, "OSCILLATOR");
            waveshapeCombo.addItemList (
                { "Sine", "Triangle", "Saw", "Square", "Pulse",
                  "SineOct", "FifthStack", "Pad", "Bell" }, 1);
            addAndMakeVisible (waveshapeCombo);
            waveshapeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
                apvts, "synthWaveshape", waveshapeCombo);

            // Unison
            makeLabel (unisonLabel, "UNISON");
            makeKnob (unisonCountKnob,  "unisonCount",  "Voices");
            makeKnob (unisonDetuneKnob, "unisonDetune", "Detune");

            // Sub Oscillator
            makeToggle (subToggle, "Sub Osc", "subOscEnabled");
            makeLabel (subLabel, "SUB OSCILLATOR");
            makeKnob (subLevelKnob, "subOscLevel", "Level");

            // Envelope (AHDSR)
            makeLabel (envLabel, "ENVELOPE");
            makeKnob (envAttackKnob,  "envAttack",  "Attack");
            makeKnob (envHoldKnob,    "envHold",    "Hold");
            makeKnob (envDecayKnob,   "envDecay",   "Decay");
            makeKnob (envSustainKnob, "envSustain", "Sustain");
            makeKnob (envReleaseKnob, "envRelease", "Release");

            // Portamento
            makeToggle (portaToggle, "Portamento", "portaEnabled");
            makeLabel (portaLabel, "PORTAMENTO");
            makeKnob (portaTimeKnob, "portaTime", "Time");

            // Drift
            makeLabel (driftLabel, "DRIFT");
            makeKnob (driftAmtKnob, "synthDrift", "Amount");
        }

        int getTotalHeight() const { return totalHeight; }

        void paint (juce::Graphics& g) override
        {
            g.setColour (juce::Colour (0xff333340));
            for (int y : dividerYs)
                g.drawLine (8.0f, (float)y, (float)(getWidth() - 8), (float)y, 1.0f);
        }

        void resized() override
        {
            auto b = getLocalBounds().reduced (8, 6);
            int knobSize = 70;
            int toggleH = 22;
            int labelH = 14;
            int comboH = 24;
            int sectionGap = 6;

            dividerYs.clear();

            // Oscillator: label + combo
            oscLabel.setBounds (b.removeFromTop (labelH));
            b.removeFromTop (4);
            waveshapeCombo.setBounds (b.removeFromTop (comboH).reduced (2, 0));
            b.removeFromTop (sectionGap);
            dividerYs.push_back (b.getY() - sectionGap / 2);

            // Unison: label + 2 knobs
            unisonLabel.setBounds (b.removeFromTop (labelH));
            b.removeFromTop (2);
            {
                auto row = b.removeFromTop (knobSize);
                unisonCountKnob->setBounds (row.removeFromLeft (knobSize));
                row.removeFromLeft (4);
                unisonDetuneKnob->setBounds (row.removeFromLeft (knobSize));
            }
            b.removeFromTop (sectionGap);
            dividerYs.push_back (b.getY() - sectionGap / 2);

            // Sub Oscillator: toggle + label + 1 knob
            subToggle.setBounds (b.removeFromTop (toggleH));
            subLabel.setBounds (b.removeFromTop (labelH));
            b.removeFromTop (2);
            {
                auto row = b.removeFromTop (knobSize);
                subLevelKnob->setBounds (row.removeFromLeft (knobSize));
            }
            b.removeFromTop (sectionGap);
            dividerYs.push_back (b.getY() - sectionGap / 2);

            // Envelope: label + 5 knobs (2+2+1 rows)
            envLabel.setBounds (b.removeFromTop (labelH));
            b.removeFromTop (2);
            {
                auto row = b.removeFromTop (knobSize);
                envAttackKnob->setBounds (row.removeFromLeft (knobSize));
                row.removeFromLeft (4);
                envHoldKnob->setBounds (row.removeFromLeft (knobSize));
            }
            b.removeFromTop (2);
            {
                auto row = b.removeFromTop (knobSize);
                envDecayKnob->setBounds (row.removeFromLeft (knobSize));
                row.removeFromLeft (4);
                envSustainKnob->setBounds (row.removeFromLeft (knobSize));
            }
            b.removeFromTop (2);
            {
                auto row = b.removeFromTop (knobSize);
                envReleaseKnob->setBounds (row.removeFromLeft (knobSize));
            }
            b.removeFromTop (sectionGap);
            dividerYs.push_back (b.getY() - sectionGap / 2);

            // Portamento: toggle + label + 1 knob
            portaToggle.setBounds (b.removeFromTop (toggleH));
            portaLabel.setBounds (b.removeFromTop (labelH));
            b.removeFromTop (2);
            {
                auto row = b.removeFromTop (knobSize);
                portaTimeKnob->setBounds (row.removeFromLeft (knobSize));
            }
            b.removeFromTop (sectionGap);
            dividerYs.push_back (b.getY() - sectionGap / 2);

            // Drift: label + 1 knob
            driftLabel.setBounds (b.removeFromTop (labelH));
            b.removeFromTop (2);
            {
                auto row = b.removeFromTop (knobSize);
                driftAmtKnob->setBounds (row.removeFromLeft (knobSize));
            }

            totalHeight = b.getY() + 10;
        }

    private:
        int totalHeight = 900;
        std::vector<int> dividerYs;

        // Toggles
        juce::ToggleButton subToggle, portaToggle;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> toggleAttachments;

        // Labels
        juce::Label oscLabel, unisonLabel, subLabel, envLabel, portaLabel, driftLabel;

        // Combo
        gm::combos::CollapsibleComboBox waveshapeCombo;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveshapeAttach;

        // Knobs
        std::unique_ptr<gm::Knob> unisonCountKnob, unisonDetuneKnob;
        std::unique_ptr<gm::Knob> subLevelKnob;
        std::unique_ptr<gm::Knob> envAttackKnob, envHoldKnob, envDecayKnob, envSustainKnob, envReleaseKnob;
        std::unique_ptr<gm::Knob> portaTimeKnob;
        std::unique_ptr<gm::Knob> driftAmtKnob;
    };

    juce::Viewport viewport;
    std::unique_ptr<Content> content;
    mutable juce::Image drawerTile;
};
