#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "Knob.h"
#include "KnobStyles.h"
#include "ButtonStyles.h"
#include "BinaryData.h"

// Sliding effects drawer with scrollable content.
// Sections ordered by signal chain: Filter -> Compressor -> Chorus -> Flanger -> Phaser -> Delay -> Reverb -> Shimmer -> Dither
class EffectsDrawer : public juce::Component {
public:
    EffectsDrawer (juce::AudioProcessorValueTreeState& apvts)
    {
        // Inner content component (lives inside Viewport)
        content = std::make_unique<Content> (apvts);
        viewport.setViewedComponent (content.get(), false);
        viewport.setScrollBarsShown (true, false);
        viewport.setScrollBarThickness (6);
        addAndMakeVisible (viewport);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colour (0xff111118));

        // Tiled brushed metal texture from BinaryData
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

        // Left edge separator line
        g.setColour (juce::Colour (0xff00cccc).withAlpha (0.3f));
        g.drawLine (0.0f, 0.0f, 0.0f, static_cast<float> (getHeight()), 1.0f);
    }

    void resized() override
    {
        viewport.setBounds (getLocalBounds());
        // Trigger content layout first, then update viewport with actual height
        content->setSize (getWidth() - 8, content->getTotalHeight());
        content->resized();
        content->setSize (getWidth() - 8, content->getTotalHeight());
    }

private:
    // Inner scrollable content with all effect sections
    class Content : public juce::Component {
    public:
        Content (juce::AudioProcessorValueTreeState& apvts)
        {
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

            // -- Sections in signal chain order --
            makeToggle (filterToggle,   "Filter",     "filterEnabled");
            makeLabel  (filterLabel,    "FILTER");
            makeKnob   (filterCutoffKnob, "filterCutoff", "Cutoff");
            makeKnob   (filterResKnob,    "filterRes",    "Reso");

            makeToggle (compToggle,     "Compressor", "compEnabled");
            makeLabel  (compLabel,      "COMPRESSOR");
            makeKnob   (compThreshKnob, "compThreshold", "Thresh");
            makeKnob   (compRatioKnob,  "compRatio",     "Ratio");
            makeKnob   (compAttackKnob, "compAttack",    "Attack");
            makeKnob   (compReleaseKnob,"compRelease",   "Release");

            makeToggle (chorusToggle,   "Chorus",     "chorusEnabled");
            makeLabel  (chorusLabel,    "CHORUS");
            makeKnob   (chorusRateKnob, "chorusRate", "Rate");
            makeKnob   (chorusMixKnob,  "chorusMix",  "Chorus");

            makeToggle (flangerToggle,  "Flanger",    "flangerEnabled");
            makeLabel  (flangerLabel,   "FLANGER");
            makeKnob   (flangerRateKnob,  "flangerRate",     "Rate");
            makeKnob   (flangerDepthKnob, "flangerDepth",    "Depth");
            makeKnob   (flangerFbKnob,    "flangerFeedback", "Fdbk");
            makeKnob   (flangerMixKnob,   "flangerMix",      "Mix");

            makeToggle (phaserToggle,   "Phaser",     "phaserEnabled");
            makeLabel  (phaserLabel,    "PHASER");
            makeKnob   (phaserRateKnob,  "phaserRate",     "Rate");
            makeKnob   (phaserDepthKnob, "phaserDepth",    "Depth");
            makeKnob   (phaserFbKnob,    "phaserFeedback", "Fdbk");
            makeKnob   (phaserMixKnob,   "phaserMix",      "Mix");

            makeToggle (delayToggle,    "Delay",      "delayEnabled");
            makeLabel  (delayLabel,     "PING PONG DELAY");
            makeKnob   (delayTimeKnob,  "delayTime",     "Time");
            makeKnob   (delayFbKnob,    "delayFeedback", "Fdbk");
            makeKnob   (delayMixKnob,   "delayMix",      "Mix");

            makeToggle (reverbToggle,   "Reverb",     "reverbEnabled");
            makeLabel  (reverbLabel,    "REVERB");
            makeKnob   (reverbDecayKnob, "reverbDecay", "Decay");
            makeKnob   (reverbDampKnob,  "reverbDamp",  "Damp");
            makeKnob   (reverbMixKnob,   "reverbMix",   "Reverb");

            makeToggle (shimmerToggle,  "Shimmer",    "shimmerEnabled");
            makeLabel  (shimmerLabel,   "SHIMMER REVERB");
            makeKnob   (shimmerDecayKnob,  "shimmerDecay",  "Decay");
            makeKnob   (shimmerAmtKnob,    "shimmerAmount", "Shimmer");
            makeKnob   (shimmerMixKnob,    "shimmerMix",    "Mix");

            makeToggle (ditherToggle,   "Dither",     "ditherEnabled");
            makeLabel  (ditherLabel,    "DITHER");
            makeKnob   (ditherBitsKnob, "ditherBits", "Bits");
        }

        int getTotalHeight() const { return totalHeight; }

        void paint (juce::Graphics& g) override
        {
            // Section dividers
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
            int sectionGap = 6;

            dividerYs.clear();

            auto layoutSection2 = [&](juce::ToggleButton& toggle, juce::Label& label,
                                      gm::Knob* k1, gm::Knob* k2) {
                toggle.setBounds (b.removeFromTop (toggleH));
                label.setBounds (b.removeFromTop (labelH));
                b.removeFromTop (2);
                auto row = b.removeFromTop (knobSize);
                if (k1) k1->setBounds (row.removeFromLeft (knobSize));
                row.removeFromLeft (4);
                if (k2) k2->setBounds (row.removeFromLeft (knobSize));
                b.removeFromTop (sectionGap);
                dividerYs.push_back (b.getY() - sectionGap / 2);
            };

            auto layoutSection3 = [&](juce::ToggleButton& toggle, juce::Label& label,
                                      gm::Knob* k1, gm::Knob* k2, gm::Knob* k3) {
                toggle.setBounds (b.removeFromTop (toggleH));
                label.setBounds (b.removeFromTop (labelH));
                b.removeFromTop (2);
                {
                    auto row = b.removeFromTop (knobSize);
                    if (k1) k1->setBounds (row.removeFromLeft (knobSize));
                    row.removeFromLeft (4);
                    if (k2) k2->setBounds (row.removeFromLeft (knobSize));
                }
                b.removeFromTop (2);
                {
                    auto row = b.removeFromTop (knobSize);
                    if (k3) k3->setBounds (row.removeFromLeft (knobSize));
                }
                b.removeFromTop (sectionGap);
                dividerYs.push_back (b.getY() - sectionGap / 2);
            };

            auto layoutSection4 = [&](juce::ToggleButton& toggle, juce::Label& label,
                                      gm::Knob* k1, gm::Knob* k2, gm::Knob* k3, gm::Knob* k4) {
                toggle.setBounds (b.removeFromTop (toggleH));
                label.setBounds (b.removeFromTop (labelH));
                b.removeFromTop (2);
                {
                    auto row = b.removeFromTop (knobSize);
                    if (k1) k1->setBounds (row.removeFromLeft (knobSize));
                    row.removeFromLeft (4);
                    if (k2) k2->setBounds (row.removeFromLeft (knobSize));
                }
                b.removeFromTop (2);
                {
                    auto row = b.removeFromTop (knobSize);
                    if (k3) k3->setBounds (row.removeFromLeft (knobSize));
                    row.removeFromLeft (4);
                    if (k4) k4->setBounds (row.removeFromLeft (knobSize));
                }
                b.removeFromTop (sectionGap);
                dividerYs.push_back (b.getY() - sectionGap / 2);
            };

            auto layoutSection1 = [&](juce::ToggleButton& toggle, juce::Label& label, gm::Knob* k1) {
                toggle.setBounds (b.removeFromTop (toggleH));
                label.setBounds (b.removeFromTop (labelH));
                b.removeFromTop (2);
                auto row = b.removeFromTop (knobSize);
                if (k1) k1->setBounds (row.removeFromLeft (knobSize));
                b.removeFromTop (sectionGap);
                dividerYs.push_back (b.getY() - sectionGap / 2);
            };

            // Signal chain order
            layoutSection2 (filterToggle,  filterLabel,  filterCutoffKnob.get(), filterResKnob.get());
            layoutSection4 (compToggle,    compLabel,    compThreshKnob.get(), compRatioKnob.get(),
                                                         compAttackKnob.get(), compReleaseKnob.get());
            layoutSection2 (chorusToggle,  chorusLabel,  chorusRateKnob.get(), chorusMixKnob.get());
            layoutSection4 (flangerToggle, flangerLabel, flangerRateKnob.get(), flangerDepthKnob.get(),
                                                         flangerFbKnob.get(), flangerMixKnob.get());
            layoutSection4 (phaserToggle,  phaserLabel,  phaserRateKnob.get(), phaserDepthKnob.get(),
                                                         phaserFbKnob.get(), phaserMixKnob.get());
            layoutSection3 (delayToggle,   delayLabel,   delayTimeKnob.get(), delayFbKnob.get(),
                                                         delayMixKnob.get());
            layoutSection3 (reverbToggle,  reverbLabel,  reverbDecayKnob.get(), reverbDampKnob.get(),
                                                         reverbMixKnob.get());
            layoutSection3 (shimmerToggle, shimmerLabel, shimmerDecayKnob.get(), shimmerAmtKnob.get(),
                                                         shimmerMixKnob.get());
            layoutSection1 (ditherToggle,  ditherLabel,  ditherBitsKnob.get());

            totalHeight = b.getY() + 10;
        }

    private:
        int totalHeight = 1400;
        std::vector<int> dividerYs;

        // Toggles
        juce::ToggleButton filterToggle, compToggle, chorusToggle, flangerToggle;
        juce::ToggleButton phaserToggle, delayToggle, reverbToggle, shimmerToggle, ditherToggle;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> toggleAttachments;

        // Labels
        juce::Label filterLabel, compLabel, chorusLabel, flangerLabel;
        juce::Label phaserLabel, delayLabel, reverbLabel, shimmerLabel, ditherLabel;

        // Knobs -- Filter
        std::unique_ptr<gm::Knob> filterCutoffKnob, filterResKnob;
        // Knobs -- Compressor
        std::unique_ptr<gm::Knob> compThreshKnob, compRatioKnob, compAttackKnob, compReleaseKnob;
        // Knobs -- Chorus
        std::unique_ptr<gm::Knob> chorusRateKnob, chorusMixKnob;
        // Knobs -- Flanger
        std::unique_ptr<gm::Knob> flangerRateKnob, flangerDepthKnob, flangerFbKnob, flangerMixKnob;
        // Knobs -- Phaser
        std::unique_ptr<gm::Knob> phaserRateKnob, phaserDepthKnob, phaserFbKnob, phaserMixKnob;
        // Knobs -- Delay
        std::unique_ptr<gm::Knob> delayTimeKnob, delayFbKnob, delayMixKnob;
        // Knobs -- Reverb
        std::unique_ptr<gm::Knob> reverbDecayKnob, reverbDampKnob, reverbMixKnob;
        // Knobs -- Shimmer
        std::unique_ptr<gm::Knob> shimmerDecayKnob, shimmerAmtKnob, shimmerMixKnob;
        // Knobs -- Dither
        std::unique_ptr<gm::Knob> ditherBitsKnob;
    };

    juce::Viewport viewport;
    std::unique_ptr<Content> content;
    mutable juce::Image drawerTile;
};
