#include "PluginEditor.h"
#include <BinaryData.h>

static const juce::StringArray kNoteNames {"C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"};

JoychordEditor::JoychordEditor (JoychordProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Init Typography singleton with embedded fonts (must happen before setSize triggers paint)
    auto& typo = gm::Typography::getInstance();
    typo.setInterRegular  (juce::Typeface::createSystemTypefaceFor (BinaryData::InterRegular_ttf, BinaryData::InterRegular_ttfSize));
    typo.setInterBold     (juce::Typeface::createSystemTypefaceFor (BinaryData::InterBold_ttf, BinaryData::InterBold_ttfSize));
    typo.setJetBrainsMono (juce::Typeface::createSystemTypefaceFor (BinaryData::JetBrainsMonoRegular_ttf, BinaryData::JetBrainsMonoRegular_ttfSize));

    // Classical serif font for chord display
    typo.setDisplayFont (juce::Typeface::createSystemTypefaceFor (BinaryData::CormorantGaramondBold_ttf, BinaryData::CormorantGaramondBold_ttfSize));


    // Apply Joychord theme (DarkMetallic + Neon combos) with neon cyan accent
    setLookAndFeel (&darkTheme);
    darkTheme.setColour (gm::DarkMetallicTheme::accentColourId, juce::Colour (0xff00ccff));

    // Initialize ThemeManager with Dark Metallic palette
    auto& theme = gm::ThemeManager::getInstance();
    theme.loadPreset ("Dark Metallic");

    // Key selector (gm::ComboSelector)
    keySel.setup (processor.apvts, "key", "Key",
                  juce::StringArray {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"});
    addAndMakeVisible (keySel);

    // Scale selector
    scaleSel.setup (processor.apvts, "scale", "Scale",
                    juce::StringArray {"Major", "Minor", "Dorian", "Mixolydian", "Phrygian", "Lydian", "Whole Tone"});
    addAndMakeVisible (scaleSel);

    // Voicing selector
    voicingSel.setup (processor.apvts, "voicing", "Voicing",
                      juce::StringArray {"Close", "Drop-2"});
    addAndMakeVisible (voicingSel);

    // Octave dropdown
    octaveLabel.setText ("Octave", juce::dontSendNotification);
    octaveLabel.setJustificationType (juce::Justification::centredRight);
    octaveLabel.setFont (gm::Typography::getInstance().getLabelFont (10.0f));
    octaveLabel.setColour (juce::Label::textColourId, juce::Colour (0xffa0a0b0));
    addAndMakeVisible (octaveLabel);
    for (int i = 2; i <= 6; ++i)
        octaveBox.addItem (juce::String(i), i);
    octaveBox.setSelectedId (4, juce::dontSendNotification);
    octaveBox.onChange = [this]() {
        if (auto* param = processor.apvts.getParameter("octave"))
            param->setValueNotifyingHost (param->convertTo0to1 (static_cast<float>(octaveBox.getSelectedId())));
    };
    addAndMakeVisible (octaveBox);

    // Preset dropdown
    presetLabel.setText ("Preset", juce::dontSendNotification);
    presetLabel.setJustificationType (juce::Justification::centredRight);
    presetLabel.setFont (gm::Typography::getInstance().getLabelFont (10.0f));
    presetLabel.setColour (juce::Label::textColourId, juce::Colour (0xffa0a0b0));
    addAndMakeVisible (presetLabel);
    auto presets = ButtonRoleMap::getFactoryPresets();
    for (int i = 0; i < static_cast<int>(presets.size()); ++i)
        presetBox.addItem (juce::String (presets[i].displayName), i + 1);
    presetBox.setSelectedId (1, juce::dontSendNotification);
    presetBox.onChange = [this, presets]() {
        int idx = presetBox.getSelectedItemIndex();
        if (idx >= 0 && idx < static_cast<int>(presets.size()))
            processor.loadPreset (presets[idx].id);
    };
    presetBox.onChange(); // Force initial sync
    addAndMakeVisible (presetBox);

    // Gamepad controller index
    gamepadLabel.setText ("Gamepad", juce::dontSendNotification);
    gamepadLabel.setJustificationType (juce::Justification::centredRight);
    gamepadLabel.setFont (gm::Typography::getInstance().getLabelFont (10.0f));
    gamepadLabel.setColour (juce::Label::textColourId, juce::Colour (0xffa0a0b0));
    addAndMakeVisible (gamepadLabel);
    for (int i = 1; i <= 4; ++i)
        gamepadIndexBox.addItem ("Controller " + juce::String(i), i);
    gamepadIndexBox.setSelectedId (1, juce::dontSendNotification);
    gamepadIndexBox.onChange = [this]() {
        processor.setControllerIndex (gamepadIndexBox.getSelectedId() - 1);
    };
    addAndMakeVisible (gamepadIndexBox);

    // Synth Mode selector
    synthModeSel.setup (processor.apvts, "synthMode", "Synth",
                        juce::StringArray {"MIDI", "SYNTH", "PIANO", "SFZ"});
    addAndMakeVisible (synthModeSel);

    // Strum Speed slider (ghostmoon HSlider)
    strumSlider = std::make_unique<gm::HSlider> (
        processor.apvts, "strumSpeed", "Strum (ms)", "0", "200", "Strum delay between chord notes");
    addAndMakeVisible (*strumSlider);

    // Load SFZ Button (ghostmoon styled)
    loadSfzBtn.setup ("Load SFZ...");
    addAndMakeVisible (loadSfzBtn);
    loadSfzBtn.getButton().onClick = [this]() {
        static std::unique_ptr<juce::FileChooser> chooser;
        chooser = std::make_unique<juce::FileChooser> ("Load SFZ File", juce::File(), "*.sfz");
        auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
        chooser->launchAsync (flags, [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile())
                processor.loadCustomSfz (file.getFullPathName());
        });
    };


    // Chord display — Classical serif (Cormorant Garamond Bold)
    // Painted directly in paint() for full font control
    chordLabel.setVisible (false);

    // Status — Typography regular
    statusLabel.setText ("Controller: not connected", juce::dontSendNotification);
    statusLabel.setFont (gm::Typography::getInstance().getLabelFont (12.0f));
    statusLabel.setJustificationType (juce::Justification::centredLeft);
    statusLabel.setColour (juce::Label::textColourId, juce::Colour (0xff808080));
    addAndMakeVisible (statusLabel);


    // LED Meters
    addAndMakeVisible (meterL);
    addAndMakeVisible (meterR);
    meterL.startMetering();
    meterR.startMetering();


    // Master Volume knob
    masterVolumeKnob = std::make_unique<gm::Knob> (processor.apvts, "masterVolume", "Volume", "Master output volume");
    masterVolumeKnob->setStyle (gm::KnobStyle::LedLadder);
    masterVolumeKnob->getSlider().setDoubleClickReturnValue (true, 0.0);  // double-click resets to 0 dB
    addAndMakeVisible (*masterVolumeKnob);

    // Effects section label
    effectsLabel.setText ("EFFECTS", juce::dontSendNotification);
    effectsLabel.setFont (gm::Typography::getInstance().getLabelFont (11.0f));
    effectsLabel.setJustificationType (juce::Justification::centredLeft);
    effectsLabel.setColour (juce::Label::textColourId, juce::Colour (0xff00ccff));
    addAndMakeVisible (effectsLabel);

    // Macro knobs on main window (NeonArc style)
    auto makeMacroKnob = [&](const juce::String& paramId, const juce::String& label,
                              const juce::String& tooltip, double defaultVal) {
        auto knob = std::make_unique<gm::Knob> (processor.apvts, paramId, label, tooltip);
        knob->setStyle (gm::KnobStyle::NeonArc);
        knob->getSlider().setDoubleClickReturnValue (true, defaultVal);
        addAndMakeVisible (*knob);
        return knob;
    };

    reverbMixKnob    = makeMacroKnob ("reverbMix",    "Reverb", "Reverb wet/dry mix", 0.25);
    filterCutoffKnob = makeMacroKnob ("filterCutoff", "Cutoff", "Filter cutoff frequency", 8000.0);

    // FX drawer button (ghostmoon styled)
    fxDrawerBtn.setup ("FX");
    fxDrawerBtn.onClick = [this] { toggleDrawer(); };
    fxDrawerBtn.getButton().onClick = [this] { toggleDrawer(); };
    addAndMakeVisible (fxDrawerBtn);

    // Effects drawer (created but initially hidden)
    effectsDrawer = std::make_unique<EffectsDrawer> (processor.apvts);
    addAndMakeVisible (*effectsDrawer);
    effectsDrawer->setVisible (false);

    // Preset system (gm::PresetManager - JSON, A/B, dirty detection)
    presetMgr = std::make_unique<gm::PresetManager> (processor.apvts, "Wombletook", "Joychord");

    // Hook RoleMap into preset serialization
    presetMgr->onSerializeCustomState = [this]() -> nlohmann::json {
        auto tree = processor.getRoleMap().toValueTree();
        return {{ "roleMap", tree.toXmlString().toStdString() }};
    };
    presetMgr->onDeserializeCustomState = [this](const nlohmann::json& j) {
        if (j.contains ("roleMap"))
        {
            auto xml = juce::XmlDocument::parse (juce::String (j["roleMap"].get<std::string>()));
            if (xml)
                processor.getRoleMap().fromValueTree (juce::ValueTree::fromXml (*xml));
        }
    };
    presetMgr->scanPresets();

    paramPresetBox.setTextWhenNothingSelected ("-- Presets --");
    paramPresetBox.onChange = [this] {
        int idx = paramPresetBox.getSelectedItemIndex();
        if (idx >= 0 && presetMgr->loadPreset (idx))
            toastOverlay.showToast ("Loaded: " + presetMgr->getCurrentPresetName(), gm::ToastOverlay::Type::Info, 1500);
    };
    addAndMakeVisible (paramPresetBox);

    presetSaveBtn.setup ("S");
    presetSaveBtn.setTooltip ("Save preset");
    presetSaveBtn.onClick = [this] { savePresetWithDialog(); };
    presetSaveBtn.getButton().onClick = [this] { savePresetWithDialog(); };
    addAndMakeVisible (presetSaveBtn);

    presetDeleteBtn.setup ("X");
    presetDeleteBtn.setTooltip ("Delete preset");
    presetDeleteBtn.onClick = [this] { deleteCurrentPreset(); };
    presetDeleteBtn.getButton().onClick = [this] { deleteCurrentPreset(); };
    addAndMakeVisible (presetDeleteBtn);

    presetPrevBtn.setup ("<");
    presetPrevBtn.setTooltip ("Previous preset");
    presetPrevBtn.onClick = [this] { presetMgr->loadPreviousPreset(); refreshPresetList(); };
    presetPrevBtn.getButton().onClick = [this] { presetMgr->loadPreviousPreset(); refreshPresetList(); };
    addAndMakeVisible (presetPrevBtn);

    presetNextBtn.setup (">");
    presetNextBtn.setTooltip ("Next preset");
    presetNextBtn.onClick = [this] { presetMgr->loadNextPreset(); refreshPresetList(); };
    presetNextBtn.getButton().onClick = [this] { presetMgr->loadNextPreset(); refreshPresetList(); };
    addAndMakeVisible (presetNextBtn);
    addAndMakeVisible (toastOverlay);

    refreshPresetList();

    // Load canvas background tile from BinaryData
    canvasTile = juce::ImageCache::getFromMemory (
        BinaryData::tile_crosshatch_png, BinaryData::tile_crosshatch_pngSize);

    setSize (mainWidth, 480);
    startTimerHz (30);
}

JoychordEditor::~JoychordEditor()
{
    setLookAndFeel (nullptr);
    meterL.stopMetering();
    meterR.stopMetering();
    stopTimer();
}

void JoychordEditor::timerCallback()
{
    btnA = processor.btnAState.load();
    btnB = processor.btnBState.load();
    btnX = processor.btnXState.load();
    btnY = processor.btnYState.load();
    dUp = processor.dUpState.load();
    dDown = processor.dDownState.load();
    dLeft = processor.dLeftState.load();
    dRight = processor.dRightState.load();
    lb = processor.lbState.load();
    rb = processor.rbState.load();
    connected = processor.gamepadConnected.load();

    // Only show Load SFZ button if SFZ (index 4) is selected
    loadSfzBtn.setVisible (static_cast<int>(processor.apvts.getParameterAsValue("synthMode").getValue()) == 3);

    // Set chord label with glow effect when playing
    if (processor.lastChordName.isNotEmpty())
    {
        chordLabel.setText (processor.lastChordName, juce::dontSendNotification);
        chordLabel.setColour (juce::Label::textColourId, juce::Colour (0xffffffff));
    }
    else
    {
        chordLabel.setText ("---", juce::dontSendNotification);
        chordLabel.setColour (juce::Label::textColourId, juce::Colour (0xff606080));
    }

    statusLabel.setText (connected ? "Controller: connected" : "Controller: not connected",
                         juce::dontSendNotification);
    statusLabel.setColour (juce::Label::textColourId,
                           connected ? juce::Colour (0xff00ccff) : juce::Colour (0xff808080));

    // Feed LED meters from ghostmoon MeterSource
    auto& src = processor.getMeterSource();
    src.decay();  // transfer audio-thread peaks to display values
    meterL.setLevel (src.getPeak (0));
    meterR.setLevel (src.getPeak (1));

    repaint();
}

static juce::String getDegreeString (int degree)
{
    static const char* roman[] = {"", "I", "II", "III", "IV", "V", "VI", "VII"};
    if (degree >= 1 && degree <= 7) return roman[degree];
    return juce::String(degree);
}

static juce::String getRoleLabel (const ButtonRole& role)
{
    return std::visit ([&](auto&& r) -> juce::String {
        using T = std::decay_t<decltype(r)>;
        if constexpr (std::is_same_v<T, RoleChord>)
            return getDegreeString (r.degree);
        else if constexpr (std::is_same_v<T, RoleExtension>)
        {
            if (r.type == 0) return "+7";
            if (r.type == 1) return "sus4";
            if (r.type == 2) return "sus2";
            if (r.type == 3) return "add9";
        }
        else if constexpr (std::is_same_v<T, RoleBorrowed>)
            return "b" + getDegreeString(r.degree);
        return "";
    }, role);
}

void JoychordEditor::paint (juce::Graphics& g)
{
    auto& typo = gm::Typography::getInstance();
    auto& theme = gm::ThemeManager::getInstance();
    auto accent = theme.getAccent();
    g.fillAll (findColour (gm::DarkMetallicTheme::bgColourId));

    int canvasX = sidebarWidth;
    int canvasW = mainWidth - sidebarWidth;
    int statusH = 24;

    // ── SIDEBAR BACKGROUND ──
    auto sidebarArea = juce::Rectangle<int> (0, 0, sidebarWidth, getHeight() - statusH);
    auto sbf = sidebarArea.toFloat().reduced (2.0f);

    // Gradient fill: cool top to warm bottom
    {
        auto sideGrad = gm::buttons::buildPanelGradient (
            juce::Colour (0xff0e0e16), sbf.getCentreX(), sbf.getY(), sbf.getBottom(), 0.35f);
        g.setGradientFill (sideGrad);
        g.fillRoundedRectangle (sbf, 4.0f);
    }

    // Tiled brushed metal texture on sidebar
    {
        auto sidebarTile = juce::ImageCache::getFromMemory (
            BinaryData::tile_brushed_metal_png, BinaryData::tile_brushed_metal_pngSize);
        if (sidebarTile.isValid())
        {
            int tw = sidebarTile.getWidth();
            int th = sidebarTile.getHeight();
            g.saveState();
            g.reduceClipRegion (sbf.toNearestInt());
            g.setOpacity (0.15f);
            for (int ty = 0; ty < getHeight(); ty += th)
                for (int tx = 0; tx < sidebarWidth; tx += tw)
                    g.drawImageAt (sidebarTile, tx, ty);
            g.restoreState();
        }
    }

    // Sidebar border
    g.setColour (juce::Colour (0xff252535));
    g.drawRoundedRectangle (sbf, 4.0f, 1.0f);

    // Top bevel highlight (subtle light edge at top)
    gm::buttons::drawTopBevel (g, sbf, 4.0f, 0.15f, 0.04f);

    // Sidebar drop shadow (melatonin blur)
    {
        juce::Path sidebarPath;
        sidebarPath.addRoundedRectangle (sbf, 4.0f);
        sidebarShadow.render (g, sidebarPath);
    }

    // Sidebar section headers with accent glow
    auto drawSectionHeader = [&](const juce::String& text, int y) {
        g.setFont (typo.getHeaderFont (10.0f));
        g.setColour (accent.withAlpha (0.15f));
        g.drawText (text, 13, y + 1, sidebarWidth - 24, 14, juce::Justification::centredLeft);
        g.setColour (accent);
        g.drawText (text, 12, y, sidebarWidth - 24, 14, juce::Justification::centredLeft);
    };

    // Match resized() layout cursor to calculate separator positions
    int sideRowH = 26, sideRowGap = 4, sideSecH = 14;
    int paintSy = 8 + sideSecH + 4;   // after HARMONY header
    paintSy += sideRowH + 6;           // preset bar
    int harmonyStartY = paintSy;
    // 3 ComboSelectors (key, scale, voicing) at (rowH+16+gap) + 1 placeRow (octave)
    paintSy += (sideRowH + 16 + sideRowGap) * 3 + (sideRowH + sideRowGap);
    int harmonyEndY = paintSy;

    drawSectionHeader ("HARMONY", 8);

    // Gradient divider after harmony section
    {
        juce::ColourGradient divGrad (juce::Colours::transparentBlack, 10.0f, (float)harmonyEndY,
                                      juce::Colour (0xff404055), (float)(sidebarWidth / 2), (float)harmonyEndY, false);
        divGrad.addColour (1.0, juce::Colours::transparentBlack);
        g.setGradientFill (divGrad);
        g.fillRect (10, harmonyEndY, sidebarWidth - 20, 1);
    }

    int perfHeaderY = harmonyEndY + 4;
    drawSectionHeader ("PERFORMANCE", perfHeaderY);

    // Gradient divider after performance section
    int perfStartY = perfHeaderY + sideSecH;
    int perfEndY = perfStartY + (sideRowH + sideRowGap) * 3 + 44 + sideRowH + 4 + 4;
    {
        juce::ColourGradient divGrad (juce::Colours::transparentBlack, 10.0f, (float)perfEndY,
                                      juce::Colour (0xff404055), (float)(sidebarWidth / 2), (float)perfEndY, false);
        divGrad.addColour (1.0, juce::Colours::transparentBlack);
        g.setGradientFill (divGrad);
        g.fillRect (10, perfEndY, sidebarWidth - 20, 1);
    }

    // ── CANVAS BACKGROUND ──
    auto canvasArea = juce::Rectangle<int> (canvasX, 0, canvasW, getHeight() - statusH);
    // Vertical gradient: slightly lighter top to darker bottom
    {
        juce::ColourGradient canvasGrad (juce::Colour (0xff161620), (float)canvasX, 0.0f,
                                         juce::Colour (0xff0e0e14), (float)canvasX, (float)(getHeight() - statusH), false);
        g.setGradientFill (canvasGrad);
        g.fillRect (canvasArea);
    }

    // Tiled crosshatch texture overlay
    if (canvasTile.isValid())
    {
        int tw = canvasTile.getWidth();
        int th = canvasTile.getHeight();
        g.saveState();
        g.reduceClipRegion (canvasArea);
        g.setOpacity (0.18f);
        for (int ty = 0; ty < getHeight(); ty += th)
            for (int tx = canvasX; tx < canvasX + canvasW; tx += tw)
                g.drawImageAt (canvasTile, tx, ty);
        g.restoreState();
    }

    // Radial vignette: darker edges for focus
    {
        float cx = (float)(canvasX + canvasW / 2);
        float cy = (float)((getHeight() - statusH) / 2);
        float r = (float)juce::jmax (canvasW, getHeight()) * 0.7f;
        juce::ColourGradient vignette (juce::Colours::transparentBlack, cx, cy,
                                       juce::Colour (0x20000000), cx, cy + r, true);
        g.setGradientFill (vignette);
        g.fillRect (canvasArea);
    }

    // Title with glow
    g.setFont (typo.getHeaderFont (20.0f));
    g.setColour (accent.withAlpha (0.12f));
    g.drawText ("Joychord", canvasX + 2, 10, canvasW, 24, juce::Justification::centred);
    g.setColour (accent.withAlpha (0.25f));
    g.drawText ("Joychord", canvasX + 1, 9, canvasW, 24, juce::Justification::centred);
    g.setColour (accent);
    g.drawText ("Joychord", canvasX, 8, canvasW, 24, juce::Justification::centred);

    // ── CHORD READOUT GLOW (melatonin blur) ──
    if (processor.lastChordName.isNotEmpty())
    {
        float glowCx = (float)(canvasX + canvasW / 2);
        float glowW = 200.0f;
        float glowH = 80.0f;
        auto chordGlowRect = juce::Rectangle<float> (glowCx - glowW / 2, 50.0f, glowW, glowH);
        juce::Path glowPath;
        glowPath.addRoundedRectangle (chordGlowRect, 16.0f);
        chordGlow.render (g, glowPath);
    }

    // ── CHORD TEXT with drop shadow ──
    {
        auto chordArea = juce::Rectangle<int> (canvasX, 20, canvasW, 150);
        auto chordText = processor.lastChordName.isNotEmpty()
                             ? processor.lastChordName
                             : juce::String ("---");
        auto chordCol  = processor.lastChordName.isNotEmpty()
                             ? juce::Colour (0xffffffff)
                             : juce::Colour (0xff606080);

        auto chordFont = gm::Typography::getInstance().getClassicalDisplayFont (120.0f);
        g.setFont (chordFont);

        // Text shadow (offset 2px down, semi-transparent black)
        g.setColour (juce::Colour (0x60000000));
        g.drawText (chordText, chordArea.translated (0, 3), juce::Justification::centred);
        // Accent underglow on text
        if (processor.lastChordName.isNotEmpty())
        {
            g.setColour (accent.withAlpha (0.08f));
            g.drawText (chordText, chordArea.translated (0, 1), juce::Justification::centred);
        }
        // Main text
        g.setColour (chordCol);
        g.drawText (chordText, chordArea, juce::Justification::centred);
    }
    // ── STATUS BAR ──
    {
        int sbY = getHeight() - statusH;
        // Drop shadow above status bar
        juce::ColourGradient shadowGrad (juce::Colours::transparentBlack, 0.0f, (float)(sbY - 8),
                                         juce::Colour (0x30000000), 0.0f, (float)sbY, false);
        g.setGradientFill (shadowGrad);
        g.fillRect (0, sbY - 8, getWidth(), 8);
    }
    auto statusArea = getLocalBounds().removeFromBottom (statusH);
    // Gradient fill: slightly lighter left to darker right
    {
        juce::ColourGradient statusGrad (juce::Colour (0xff141420), 0.0f, (float)statusArea.getY(),
                                         juce::Colour (0xff0c0c14), 0.0f, (float)statusArea.getBottom(), false);
        g.setGradientFill (statusGrad);
        g.fillRect (statusArea);
    }
    // Accent-tinged top divider
    g.setColour (accent.withAlpha (0.15f));
    g.drawHorizontalLine (statusArea.getY(), 0.0f, (float)getWidth());
    g.setColour (juce::Colour (0xff252535));
    g.drawHorizontalLine (statusArea.getY() + 1, 0.0f, (float)getWidth());

    // Connection status dot (green=connected, dim red=disconnected)
    {
        float dotX = 12.0f;
        float dotY = (float)statusArea.getCentreY();
        float dotR = 3.5f;
        auto dotCol = connected ? juce::Colour (0xff00cc66) : juce::Colour (0xff663333);
        if (connected) {
            g.setColour (dotCol.withAlpha (0.2f));
            g.fillEllipse (dotX - dotR - 2, dotY - dotR - 2, (dotR + 2) * 2, (dotR + 2) * 2);
        }
        g.setColour (dotCol);
        g.fillEllipse (dotX - dotR, dotY - dotR, dotR * 2, dotR * 2);
    }

    // Version watermark (right-justified in status bar)
    g.setFont (typo.getLabelFont (9.0f));
    g.setColour (juce::Colour (0xff404050));
    g.drawText ("v0.1.0", statusArea.reduced (12, 0), juce::Justification::centredRight);

    // ── GAMEPAD HUD (compact, in canvas) ──
    int hudCx = canvasX + canvasW / 2;
    int hudCy = 220;
    int r = 15;
    int spacing = 22;

    auto drawBtn = [&](int x, int y, const juce::String& label, bool pressed, juce::Colour col, int radius = 15)
    {
        float fx = (float)x, fy = (float)y, fr = (float)radius;
        auto btnRect = juce::Rectangle<float> (fx - fr, fy - fr, fr * 2.0f, fr * 2.0f);

        if (pressed) {
            // Accent glow bloom (uses melatonin_blur when available)
            gm::buttons::drawAccentGlow (g, btnRect, fr, col, 10.0f, 0.25f);
        }

        // Drop shadow beneath button
        gm::buttons::drawButtonShadow (g, btnRect, fr, pressed ? 0.1f : 0.25f);

        // Body fill
        g.setColour (pressed ? col : col.withAlpha (0.15f));
        g.fillEllipse (btnRect);

        // Inner shadow for depth
        gm::buttons::drawInnerShadow (g, btnRect, fr, 0.3f);

        // Specular dome (3D curvature highlight)
        gm::buttons::drawSpecularDome (g, btnRect, fr, pressed ? 0.6f : 0.25f);

        // Rim lighting (beveled edge)
        gm::buttons::drawRimLighting (g, btnRect, fr, 0.12f, 0.2f);

        // Border
        g.setColour (pressed ? col.brighter (0.3f) : juce::Colour (0xff505060));
        g.drawEllipse (btnRect, 1.5f);

        // Label text
        g.setColour (pressed ? juce::Colours::white : juce::Colour (0xff909090));
        g.setFont (typo.getLabelFont (10.0f));
        g.drawText (label, x - radius, y - radius, radius * 2, radius * 2, juce::Justification::centred);
    };

    // Face buttons (right side)
    int faceCx = hudCx + 55;
    drawBtn (faceCx,              hudCy - spacing, "Y",  btnY, juce::Colour (0xffccaa00));
    drawBtn (faceCx - spacing,    hudCy,           "X",  btnX, juce::Colour (0xff0066cc));
    drawBtn (faceCx + spacing,    hudCy,           "B",  btnB, juce::Colour (0xffcc2200));
    drawBtn (faceCx,              hudCy + spacing, "A",  btnA, juce::Colour (0xff00aa44));

    // D-pad (left side)
    int dpadCx = hudCx - 55;
    int ds = 18;
    int dr = 12;
    auto grey = juce::Colour (0xff666688);
    drawBtn (dpadCx,       hudCy - ds, "U",  dUp,    grey, dr);
    drawBtn (dpadCx - ds,  hudCy,      "L",  dLeft,  grey, dr);
    drawBtn (dpadCx + ds,  hudCy,      "R",  dRight, grey, dr);
    drawBtn (dpadCx,       hudCy + ds, "D",  dDown,  grey, dr);

    // Shoulders
    int shoulderY = hudCy - spacing - 30;
    drawBtn (dpadCx, shoulderY, "LB", lb, juce::Colour (0xffff4400), 12);
    drawBtn (faceCx, shoulderY, "RB", rb, juce::Colour (0xff00ccff), 12);

    // Degree labels (accent-tinted pill background)
    g.setFont (typo.getValueFont (9.0f));

    auto drawRoleLabel = [&](ButtonId btnId, int x, int y, juce::Justification just) {
        juce::String labelStr = getRoleLabel (processor.getRoleMap().getRole (btnId));
        if (labelStr.isNotEmpty()) {
            int textW = labelStr.length() * 7 + 8;  // approximate pill width
            int pillX = (just == juce::Justification::centredRight) ? x + 28 - textW : x;
            auto pill = juce::Rectangle<float> ((float)pillX, (float)y, (float)textW, 13.0f);
            g.setColour (accent.withAlpha (0.08f));
            g.fillRoundedRectangle (pill, 3.0f);
            g.setColour (accent.withAlpha (0.2f));
            g.drawRoundedRectangle (pill, 3.0f, 0.5f);
            g.setColour (juce::Colour (0xffc0c0d0));
            g.drawText (labelStr, x, y, 28, 13, just);
        }
    };

    drawRoleLabel (ButtonId::Y, faceCx + r + 2,            hudCy - spacing - 5, juce::Justification::centredLeft);
    drawRoleLabel (ButtonId::X, faceCx - spacing - r - 30, hudCy - 5,           juce::Justification::centredRight);
    drawRoleLabel (ButtonId::B, faceCx + spacing + r + 2,  hudCy - 5,           juce::Justification::centredLeft);
    drawRoleLabel (ButtonId::A, faceCx + r + 2,            hudCy + spacing - 5, juce::Justification::centredLeft);

    drawRoleLabel (ButtonId::DUp,    dpadCx - dr - 30,      hudCy - ds - 5, juce::Justification::centredRight);
    drawRoleLabel (ButtonId::DLeft,  dpadCx - ds - dr - 30, hudCy - 5,      juce::Justification::centredRight);
    drawRoleLabel (ButtonId::DRight, dpadCx + ds + dr + 2,  hudCy - 5,      juce::Justification::centredLeft);
    drawRoleLabel (ButtonId::DDown,  dpadCx - dr - 30,      hudCy + ds - 5, juce::Justification::centredRight);

    // ── Effects section gradient divider in canvas ──
    int effectsDivY = 280;
    {
        float cx = (float)(canvasX + canvasW / 2);
        float halfW = (float)(canvasW / 2 - 16);
        juce::ColourGradient divGrad (juce::Colours::transparentBlack, cx - halfW, (float)effectsDivY,
                                      juce::Colour (0xff505068), cx, (float)effectsDivY, false);
        divGrad.addColour (1.0, juce::Colours::transparentBlack);
        g.setGradientFill (divGrad);
        g.fillRect (canvasX + 16, effectsDivY, canvasW - 32, 1);
    }

}

void JoychordEditor::resized()
{
    int statusH = 24;
    int canvasX = sidebarWidth;
    int canvasW = mainWidth - sidebarWidth;

    // ══════════════════════════════════════════
    // SIDEBAR LAYOUT (left panel)
    // ══════════════════════════════════════════
    int sx = 10;
    int sw = sidebarWidth - 20;
    int labelW = 55;
    int boxW = sw - labelW;
    int rowH = 26;
    int rowGap = 4;
    int sectionHeaderH = 14;

    int sy = 8 + sectionHeaderH + 4; // after "HARMONY" header

    // Preset bar (above everything else in sidebar)
    int presetComboW = sw - 90;
    presetPrevBtn.setBounds    (sx, sy, 14, rowH);
    paramPresetBox.setBounds   (sx + 16, sy, presetComboW, rowH);
    presetNextBtn.setBounds    (sx + 16 + presetComboW + 2, sy, 14, rowH);
    presetSaveBtn.setBounds    (sx + sw - 40, sy, 20, rowH);
    presetDeleteBtn.setBounds  (sx + sw - 18, sy, 18, rowH);
    sy += rowH + 6;

    // Harmony section: Key, Scale, Voicing, Octave
    auto placeSelector = [&](gm::ComboSelector& sel) {
        sel.setBounds (sx, sy, sw, rowH + 16);
        sy += rowH + 16 + rowGap;
    };
    auto placeRow = [&](juce::Label& lbl, juce::Component& box) {
        lbl.setBounds (sx, sy, labelW, rowH);
        box.setBounds (sx + labelW, sy, boxW, rowH);
        sy += rowH + rowGap;
    };

    placeSelector (keySel);
    placeSelector (scaleSel);
    placeSelector (voicingSel);
    placeRow (octaveLabel, octaveBox);

    sy += 4 + sectionHeaderH + 4;

    // Performance section
    placeSelector (synthModeSel);
    placeRow (presetLabel, presetBox);
    placeRow (gamepadLabel, gamepadIndexBox);

    // Strum slider (full sidebar width)
    if (strumSlider)
        strumSlider->setBounds (sx, sy, sw, 40);
    sy += 44;

    // Load SFZ button
    loadSfzBtn.setBounds (sx, sy, sw, rowH);
    sy += rowH + 4;

    // ══════════════════════════════════════════
    // CANVAS LAYOUT (right panel)
    // ══════════════════════════════════════════

    // Chord display (large, centered in upper canvas)
    chordLabel.setBounds (canvasX, 20, canvasW, 140);

    // Effects area label
    effectsLabel.setBounds (canvasX + 12, 284, 60, 14);

    // Knobs row (below effects divider)
    int knobW = 70;
    int knobH = 84;
    int knobGap = 10;
    int knobY = 300;
    int knobStartX = canvasX + 12;

    if (masterVolumeKnob)
        masterVolumeKnob->setBounds (knobStartX, knobY, knobW, knobH);
    if (reverbMixKnob)
        reverbMixKnob->setBounds (knobStartX + 1 * (knobW + knobGap), knobY, knobW, knobH);
    if (filterCutoffKnob)
        filterCutoffKnob->setBounds (knobStartX + 2 * (knobW + knobGap), knobY, knobW, knobH);

    // FX drawer button
    fxDrawerBtn.setBounds (knobStartX + 3 * (knobW + knobGap), knobY + 20, 40, 40);

    // LED Meters (right side of canvas)
    int meterW = 14;
    int meterH = 100;
    int meterX = canvasX + canvasW - 40;
    int meterY = knobY;
    meterL.setBounds (meterX, meterY, meterW, meterH);
    meterR.setBounds (meterX + meterW + 4, meterY, meterW, meterH);

    // Toast overlay (full canvas area for centered toast messages)
    toastOverlay.setBounds (sidebarWidth, 0, canvasW, getHeight());

    // Status bar
    statusLabel.setBounds (getLocalBounds().withWidth (mainWidth).removeFromBottom (statusH).reduced (16, 0));

    // Effects drawer
    if (effectsDrawer)
        effectsDrawer->setBounds (mainWidth, 0, drawerWidth, getHeight());
}

void JoychordEditor::toggleDrawer()
{
    drawerOpen = !drawerOpen;
    int targetW = drawerOpen ? mainWidth + drawerWidth : mainWidth;
    effectsDrawer->setVisible (drawerOpen);
    setSize (targetW, getHeight());
    fxDrawerBtn.setup (drawerOpen ? "<<" : "FX");
}

void JoychordEditor::refreshPresetList()
{
    paramPresetBox.clear (juce::dontSendNotification);
    auto& presets = presetMgr->getPresets();
    for (int i = 0; i < static_cast<int>(presets.size()); ++i)
        paramPresetBox.addItem (presets[i].name, i + 1);

    int idx = presetMgr->getCurrentPresetIndex();
    if (idx >= 0)
        paramPresetBox.setSelectedId (idx + 1, juce::dontSendNotification);
}

void JoychordEditor::savePresetWithDialog()
{
    auto dlg = std::make_unique<juce::AlertWindow> (
        "Save Preset", "Enter a name for this preset:",
        juce::MessageBoxIconType::NoIcon);
    dlg->addTextEditor ("name", presetMgr->getCurrentPresetName(), "Preset Name");
    dlg->addButton ("Save", 1);
    dlg->addButton ("Cancel", 0);

    auto* dlgPtr = dlg.get();
    dlg->enterModalState (true,
        juce::ModalCallbackFunction::create ([this, dlgPtr] (int result) {
            if (result == 1)
            {
                auto name = dlgPtr->getTextEditorContents ("name").trim();
                if (name.isNotEmpty())
                {
                    presetMgr->savePreset (name);
                    refreshPresetList();
                    toastOverlay.showToast ("Saved: " + name, gm::ToastOverlay::Type::Success, 2000);
                }
            }
        }),
        true);
    dlg.release();
}

void JoychordEditor::deleteCurrentPreset()
{
    int idx = presetMgr->getCurrentPresetIndex();
    if (idx < 0) return;

    auto& presets = presetMgr->getPresets();
    if (idx >= static_cast<int>(presets.size())) return;
    if (presets[idx].isFactory) {
        toastOverlay.showToast ("Cannot delete factory preset", gm::ToastOverlay::Type::Warning, 2000);
        return;
    }

    auto file = presets[idx].file;
    if (file.existsAsFile())
        file.deleteFile();

    presetMgr->scanPresets();
    refreshPresetList();
    toastOverlay.showToast ("Preset deleted", gm::ToastOverlay::Type::Info, 1500);
}
