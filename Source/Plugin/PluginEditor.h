#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "DarkMetallicTheme.h"
#include "ComboBoxStyles.h"
#include "LEDMeter.h"
#include "LEDMeterH.h"
#include "Knob.h"
#include "KnobStyles.h"
#include "HSlider.h"
#include "SliderStyles.h"
#include "GmTextButton.h"
#include "ThemeManager.h"
#include "ComboSelector.h"
#include "EffectsDrawer.h"
#include "SynthDrawer.h"
#include "AxisDrawer.h"
#include "PresetManager.h"
#include "ToastOverlay.h"
#include "ButtonStyles.h"
#include "AnimatedPanel.h"
#include <melatonin_blur/melatonin_blur.h>

#include "CollapsibleComboBox.h"

// Joychord theme: DarkMetallic + ghostmoon Neon combo boxes
// Popup rendering handled by DarkMetallicTheme (fillAll + exact alignment)
struct JoychordTheme : public gm::DarkMetallicTheme {
    void drawComboBox (juce::Graphics& g, int w, int h, bool down,
                       int bx, int by, int bw, int bh, juce::ComboBox& cb) override {
        // Clip to rounded rect so neon bloom doesn't leak as square corners
        float cr = juce::jmin (4.0f, (float) h * 0.2f);
        juce::Path clip;
        clip.addRoundedRectangle (0.0f, 0.0f, (float) w, (float) h, cr);
        g.saveState();
        g.reduceClipRegion (clip);
        gm::combos::drawNeonComboBox (g, w, h, down, bx, by, bw, bh, cb);
        g.restoreState();
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
    void paintOverChildren (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void toggleDrawer();
    void toggleSynthDrawer();
    void toggleAxisDrawer();
    void animateToWidth (int targetW);

    JoychordProcessor& processor;

    // APVTS-attached controls (gm::ComboSelector — self-rendering)
    gm::ComboSelector keySel, scaleSel, voicingSel, synthModeSel;

    // Non-APVTS combos (manual onChange)
    gm::combos::CollapsibleComboBox octaveBox, presetBox, gamepadIndexBox;

    std::unique_ptr<gm::HSlider> strumSlider;

    gm::GmTextButton loadSfzBtn;

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
    bool l3 = false, r3 = false;
    float ltVal = 0.f, rtVal = 0.f;
    float lsX = 0.f, lsY = 0.f, rsX = 0.f, rsY = 0.f;
    bool connected = false;

    // Ghostmoon UI
    JoychordTheme darkTheme;
    gm::LEDMeterH meterL, meterR;

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
    static constexpr int mainWidth = 585;
    static constexpr int sidebarWidth = 200;
    static constexpr int drawerWidth = 180;

    // Synth drawer
    gm::GmTextButton synthDrawerBtn;
    std::unique_ptr<SynthDrawer> synthDrawer;
    bool synthDrawerOpen = false;

    // Axis assignment drawer
    gm::GmTextButton axisDrawerBtn;
    std::unique_ptr<AxisDrawer> axisDrawer;
    bool axisDrawerOpen = false;

    // Drawer animation (JUCE built-in ComponentAnimator via Desktop::getAnimator)
    void slideDrawerIn (juce::Component* drawer);
    void slideDrawerOut (juce::Component* drawer);

    /// Returns whichever drawer is currently active (effects, synth, or axis)
    juce::Component* getActiveDrawer() const {
        if (drawerOpen && effectsDrawer) return effectsDrawer.get();
        if (synthDrawerOpen && synthDrawer) return synthDrawer.get();
        if (axisDrawerOpen && axisDrawer) return axisDrawer.get();
        return nullptr;
    }

    // Canvas background tile (crosshatch texture, loaded from BinaryData)
    juce::Image canvasTile;

    // Preset system (gm::PresetManager — JSON, A/B, dirty detection)
    std::unique_ptr<gm::PresetManager> presetMgr;
    gm::combos::CollapsibleComboBox paramPresetBox;
    gm::GmTextButton presetSaveBtn;
    gm::GmTextButton presetDeleteBtn;
    gm::GmTextButton presetPrevBtn;
    gm::GmTextButton presetNextBtn;
    gm::ToastOverlay toastOverlay;
    void refreshPresetList();
    void savePresetWithDialog();
    void deleteCurrentPreset();
    // DPI scale
    gm::combos::CollapsibleComboBox dpiScaleBox;
    juce::Label dpiLabel;
    void applyDpiScale (float scale);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JoychordEditor)
};
