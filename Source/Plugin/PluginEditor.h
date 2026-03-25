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
#include "GmTextButton.h"
#include "ThemeManager.h"
#include "ComboSelector.h"
#include "EffectsDrawer.h"
#include "PresetManager.h"
#include "ToastOverlay.h"
#include "ButtonStyles.h"
#include <melatonin_blur/melatonin_blur.h>

// Joychord theme: DarkMetallic + ghostmoon Neon combo boxes
// Popup rendering handled by DarkMetallicTheme (fillAll + exact alignment)
struct JoychordTheme : public gm::DarkMetallicTheme {
    void drawComboBox (juce::Graphics& g, int w, int h, bool down,
                       int bx, int by, int bw, int bh, juce::ComboBox& cb) override {
        gm::combos::drawNeonComboBox (g, w, h, down, bx, by, bw, bh, cb);
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

    // APVTS-attached controls (gm::ComboSelector — self-rendering)
    gm::ComboSelector keySel, scaleSel, voicingSel, synthModeSel;

    // Non-APVTS combos (manual onChange)
    juce::ComboBox octaveBox, presetBox, gamepadIndexBox;

    std::unique_ptr<gm::HSlider> strumSlider;

    juce::TextButton loadSfzBtn { "Load SFZ..." };

    // Chord display
    juce::Label chordLabel;

    // Connection status
    juce::Label statusLabel;

    // Section labels for non-ComboSelector rows
    juce::Label octaveLabel, presetLabel, gamepadLabel;

    // Section label for effects area
    juce::Label effectsLabel;

    // Button state
    bool btnA = false, btnB = false, btnX = false, btnY = false;
    bool dUp = false, dDown = false, dLeft = false, dRight = false;
    bool lb = false, rb = false;
    bool connected = false;

    // Ghostmoon UI
    JoychordTheme darkTheme;
    gm::LEDMeter meterL, meterR;

    // Melatonin blur shadows (cached, GPU-accelerated)
    melatonin::DropShadow chordGlow { juce::Colour (0xff00ccff).withAlpha (0.35f), 24, { 0, 4 } };
    melatonin::DropShadow sidebarShadow { juce::Colours::black.withAlpha (0.5f), 12, { 4, 0 } };

    // Main window macro knobs
    std::unique_ptr<gm::Knob> masterVolumeKnob;
    std::unique_ptr<gm::Knob> reverbMixKnob;
    std::unique_ptr<gm::Knob> filterCutoffKnob;



    // Effects drawer
    gm::GmTextButton fxDrawerBtn;
    std::unique_ptr<EffectsDrawer> effectsDrawer;
    bool drawerOpen = false;
    static constexpr int mainWidth = 620;
    static constexpr int sidebarWidth = 200;
    static constexpr int drawerWidth = 180;

    // Canvas background tile (crosshatch texture, loaded from BinaryData)
    juce::Image canvasTile;

    // Preset system (gm::PresetManager — JSON, A/B, dirty detection)
    std::unique_ptr<gm::PresetManager> presetMgr;
    juce::ComboBox paramPresetBox;
    gm::GmTextButton presetSaveBtn;
    gm::GmTextButton presetDeleteBtn;
    gm::GmTextButton presetPrevBtn;
    gm::GmTextButton presetNextBtn;
    gm::ToastOverlay toastOverlay;
    void refreshPresetList();
    void savePresetWithDialog();
    void deleteCurrentPreset();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JoychordEditor)
};
