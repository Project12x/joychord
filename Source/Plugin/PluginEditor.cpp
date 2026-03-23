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

    setSize (520, 560);

    // Apply Joychord theme (DarkMetallic + Neon combos) with neon cyan accent
    setLookAndFeel (&darkTheme);
    darkTheme.setColour (gm::DarkMetallicTheme::accentColourId, juce::Colour (0xff00ccff));

    // Key dropdown
    keyLabel.setText ("Key", juce::dontSendNotification);
    keyLabel.setJustificationType (juce::Justification::centredRight);
    addAndMakeVisible (keyLabel);
    for (int i = 0; i < 12; ++i)
        keyBox.addItem (kNoteNames[i], i + 1);
    keyBox.setSelectedId (1, juce::dontSendNotification);
    addAndMakeVisible (keyBox);
    keyAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.apvts, "key", keyBox);

    // Scale dropdown
    scaleLabel.setText ("Scale", juce::dontSendNotification);
    scaleLabel.setJustificationType (juce::Justification::centredRight);
    addAndMakeVisible (scaleLabel);
    scaleBox.addItemList (juce::StringArray {"Major", "Minor", "Dorian", "Mixolydian", "Phrygian", "Lydian", "Whole Tone"}, 1);
    addAndMakeVisible (scaleBox);
    scaleAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.apvts, "scale", scaleBox);

    // Voicing dropdown
    voicingLabel.setText ("Voicing", juce::dontSendNotification);
    voicingLabel.setJustificationType (juce::Justification::centredRight);
    addAndMakeVisible (voicingLabel);
    voicingBox.addItemList (juce::StringArray {"Close", "Drop-2"}, 1);
    addAndMakeVisible (voicingBox);
    voicingAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.apvts, "voicing", voicingBox);

    // Octave dropdown
    octaveLabel.setText ("Octave", juce::dontSendNotification);
    octaveLabel.setJustificationType (juce::Justification::centredRight);
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
    addAndMakeVisible (gamepadLabel);
    for (int i = 1; i <= 4; ++i)
        gamepadIndexBox.addItem ("Controller " + juce::String(i), i);
    gamepadIndexBox.setSelectedId (1, juce::dontSendNotification);
    gamepadIndexBox.onChange = [this]() {
        processor.setControllerIndex (gamepadIndexBox.getSelectedId() - 1);
    };
    addAndMakeVisible (gamepadIndexBox);

    // Synth Mode dropdown
    synthModeLabel.setText ("Synth", juce::dontSendNotification);
    synthModeLabel.setJustificationType (juce::Justification::centredRight);
    addAndMakeVisible (synthModeLabel);
    synthModeBox.addItemList (juce::StringArray {"MIDI", "SYNTH", "PIANO", "SFZ"}, 1);
    addAndMakeVisible (synthModeBox);
    synthModeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.apvts, "synthMode", synthModeBox);

    // Strum Speed slider
    strumLabel.setText ("Strum (ms)", juce::dontSendNotification);
    strumLabel.setJustificationType (juce::Justification::centredRight);
    addAndMakeVisible (strumLabel);

    strumSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    strumSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 40, 20);
    addAndMakeVisible (strumSlider);
    strumAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.apvts, "strumSpeed", strumSlider);

    // Load SFZ Button
    addAndMakeVisible (loadSfzBtn);
    loadSfzBtn.onClick = [this]() {
        static std::unique_ptr<juce::FileChooser> chooser;
        chooser = std::make_unique<juce::FileChooser> ("Load SFZ File", juce::File(), "*.sfz");
        auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
        chooser->launchAsync (flags, [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile())
                processor.loadCustomSfz (file.getFullPathName());
        });
    };


    // Chord display — Typography bold
    chordLabel.setText ("---", juce::dontSendNotification);
    chordLabel.setFont (gm::Typography::getInstance().getHeaderFont (36.0f));
    chordLabel.setJustificationType (juce::Justification::centred);
    chordLabel.setColour (juce::Label::textColourId, juce::Colour (0xffe0e0ff));
    addAndMakeVisible (chordLabel);

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

    // Effects knobs (NeonArc style)
    auto makeEffectKnob = [&](const juce::String& paramId, const juce::String& label,
                              const juce::String& tooltip, double defaultVal) {
        auto knob = std::make_unique<gm::Knob> (processor.apvts, paramId, label, tooltip);
        knob->setStyle (gm::KnobStyle::NeonArc);
        knob->getSlider().setDoubleClickReturnValue (true, defaultVal);
        addAndMakeVisible (*knob);
        return knob;
    };

    filterCutoffKnob = makeEffectKnob ("filterCutoff", "Cutoff", "Filter cutoff frequency", 8000.0);
    filterResKnob    = makeEffectKnob ("filterRes",    "Reso",   "Filter resonance", 0.1);
    chorusRateKnob   = makeEffectKnob ("chorusRate",   "Rate",   "Chorus LFO rate", 0.5);
    chorusMixKnob    = makeEffectKnob ("chorusMix",    "Chorus", "Chorus wet/dry mix", 0.0);
    reverbDecayKnob  = makeEffectKnob ("reverbDecay",  "Decay",  "Reverb decay time", 0.5);
    reverbDampKnob   = makeEffectKnob ("reverbDamp",   "Damp",   "Reverb high-freq damping", 0.4);
    reverbMixKnob    = makeEffectKnob ("reverbMix",    "Reverb", "Reverb wet/dry mix", 0.25);

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
    loadSfzBtn.setVisible (synthModeBox.getSelectedId() == 4);

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
    g.fillAll (findColour (gm::DarkMetallicTheme::bgColourId));

    // Title with shadow glow
    auto titleY = 8;
    auto titleH = 28;
    g.setFont (typo.getHeaderFont (22.0f));
    g.setColour (juce::Colour (0xff00ccff).withAlpha (0.25f));
    g.drawText ("Joychord", 1, titleY + 1, getWidth(), titleH, juce::Justification::centred);
    g.setColour (juce::Colour (0xff00ccff));
    g.drawText ("Joychord", 0, titleY, getWidth(), titleH, juce::Justification::centred);

    // Section divider between controls and effects
    int dividerY = 210;
    g.setColour (juce::Colour (0xff303040));
    g.drawHorizontalLine (dividerY, 16.0f, (float)(getWidth() - 16));
    g.setColour (juce::Colour (0xff101018));
    g.drawHorizontalLine (dividerY + 1, 16.0f, (float)(getWidth() - 16));

    // Section divider between effects and gamepad area
    int dividerY2 = 310;
    g.setColour (juce::Colour (0xff303040));
    g.drawHorizontalLine (dividerY2, 16.0f, (float)(getWidth() - 16));
    g.setColour (juce::Colour (0xff101018));
    g.drawHorizontalLine (dividerY2 + 1, 16.0f, (float)(getWidth() - 16));

    // Status bar background
    auto statusArea = getLocalBounds().removeFromBottom (24);
    g.setColour (juce::Colour (0xff101018));
    g.fillRect (statusArea);
    g.setColour (juce::Colour (0xff303040));
    g.drawHorizontalLine (statusArea.getY(), 0.0f, (float)getWidth());

    // ── Gamepad button indicators ──

    int r = 18;
    int spacing = 26;

    auto drawBtn = [&](int x, int y, const juce::String& label, bool pressed, juce::Colour col, int radius = 18)
    {
        float fx = (float)x, fy = (float)y, fr = (float)radius;

        // Outer glow when pressed
        if (pressed) {
            g.setColour (col.withAlpha (0.15f));
            g.fillEllipse (fx - fr - 4.0f, fy - fr - 4.0f, (fr + 4.0f) * 2.0f, (fr + 4.0f) * 2.0f);
        }

        // Button fill
        g.setColour (pressed ? col : col.withAlpha (0.15f));
        g.fillEllipse (fx - fr, fy - fr, fr * 2.0f, fr * 2.0f);

        // Outline — accent-tinted when pressed, subtle when idle
        g.setColour (pressed ? col.brighter (0.3f) : juce::Colour (0xff505060));
        g.drawEllipse (fx - fr, fy - fr, fr * 2.0f, fr * 2.0f, 1.5f);

        // Label
        g.setColour (pressed ? juce::Colours::white : juce::Colour (0xff909090));
        g.setFont (typo.getLabelFont (11.0f));
        g.drawText (label, x - radius, y - radius, radius * 2, radius * 2, juce::Justification::centred);
    };

    // Face buttons (right side) -- Xbox diamond
    int faceCx = getWidth() / 2 + 70;
    int faceCy = 390;
    drawBtn (faceCx,              faceCy - spacing, "Y",  btnY, juce::Colour (0xffccaa00));
    drawBtn (faceCx - spacing,    faceCy,           "X",  btnX, juce::Colour (0xff0066cc));
    drawBtn (faceCx + spacing,    faceCy,           "B",  btnB, juce::Colour (0xffcc2200));
    drawBtn (faceCx,              faceCy + spacing, "A",  btnA, juce::Colour (0xff00aa44));

    // D-pad (left side) -- cross layout
    int dpadCx = getWidth() / 2 - 70;
    int dpadCy = 390;
    int ds = 22;
    int dr = 14;
    auto grey = juce::Colour (0xff666688);
    drawBtn (dpadCx,       dpadCy - ds, "U",  dUp,    grey, dr);
    drawBtn (dpadCx - ds,  dpadCy,      "L",  dLeft,  grey, dr);
    drawBtn (dpadCx + ds,  dpadCy,      "R",  dRight, grey, dr);
    drawBtn (dpadCx,       dpadCy + ds, "D",  dDown,  grey, dr);

    // Shoulders (top of button area)
    int shoulderY = 355;
    drawBtn (dpadCx - 10, shoulderY, "LB", lb, juce::Colour (0xffff4400), 14);
    drawBtn (faceCx + 10, shoulderY, "RB", rb, juce::Colour (0xff00ccff), 14);

    // Degree labels — mono font for data
    g.setFont (typo.getValueFont (10.0f));
    g.setColour (juce::Colour (0xffa0a0b0));

    auto drawRoleLabel = [&](ButtonId btnId, int x, int y, juce::Justification just) {
        juce::String labelStr = getRoleLabel (processor.getRoleMap().getRole (btnId));
        if (labelStr.isNotEmpty())
            g.drawText (labelStr, x, y, 30, 12, just);
    };

    // Face button degree labels
    drawRoleLabel (ButtonId::Y, faceCx + r + 2,            faceCy - spacing - 6, juce::Justification::centredLeft);
    drawRoleLabel (ButtonId::X, faceCx - spacing - r - 32, faceCy - 6,           juce::Justification::centredRight);
    drawRoleLabel (ButtonId::B, faceCx + spacing + r + 2,  faceCy - 6,           juce::Justification::centredLeft);
    drawRoleLabel (ButtonId::A, faceCx + r + 2,            faceCy + spacing - 6, juce::Justification::centredLeft);

    // D-Pad modifier labels
    drawRoleLabel (ButtonId::DUp,    dpadCx - r - 32,           dpadCy - ds - 6, juce::Justification::centredRight);
    drawRoleLabel (ButtonId::DLeft,  dpadCx - ds - r - 32,      dpadCy - 6,      juce::Justification::centredRight);
    drawRoleLabel (ButtonId::DRight, dpadCx + ds + r + 2,       dpadCy - 6,      juce::Justification::centredLeft);
    drawRoleLabel (ButtonId::DDown,  dpadCx - r - 32,           dpadCy + ds - 6, juce::Justification::centredRight);
}

void JoychordEditor::resized()
{
    auto area = getLocalBounds().reduced (16);
    area.removeFromTop (36); // title

    auto labelW = 70;
    auto boxW = 120;
    auto gap = 30;

    int col1X = area.getX();
    int col2X = col1X + labelW + boxW + gap;

    int rowH = 26;
    int curY = area.getY();

    // Row 1
    keyLabel.setBounds (col1X, curY, labelW, rowH);
    keyBox.setBounds   (col1X + labelW, curY, boxW, rowH);
    
    presetLabel.setBounds     (col2X, curY, labelW, rowH);
    presetBox.setBounds       (col2X + labelW, curY, boxW, rowH);

    curY += rowH + 6;

    // Row 2
    scaleLabel.setBounds (col1X, curY, labelW, rowH);
    scaleBox.setBounds   (col1X + labelW, curY, boxW, rowH);

    gamepadLabel.setBounds    (col2X, curY, labelW, rowH);
    gamepadIndexBox.setBounds (col2X + labelW, curY, boxW, rowH);

    curY += rowH + 6;

    // Row 3
    voicingLabel.setBounds (col1X, curY, labelW, rowH);
    voicingBox.setBounds   (col1X + labelW, curY, boxW, rowH);

    synthModeLabel.setBounds  (col2X, curY, labelW, rowH);
    synthModeBox.setBounds    (col2X + labelW, curY, boxW, rowH);

    curY += rowH + 6;

    // Row 4
    octaveLabel.setBounds (col1X, curY, labelW, rowH);
    octaveBox.setBounds   (col1X + labelW, curY, boxW, rowH);

    loadSfzBtn.setBounds      (col2X, curY, labelW + boxW, rowH);

    curY += rowH + 6;

    // Row 5
    strumLabel.setBounds  (col1X, curY, labelW, rowH);
    strumSlider.setBounds (col1X + labelW, curY, boxW, rowH);

    curY += rowH + 10;

    // Effects section label + knob row
    effectsLabel.setBounds (col1X, curY, 60, 14);
    curY += 16;

    int knobW = 58;
    int knobH = 74;  // knob + label + value
    int knobGap = 8;
    int knobStartX = col1X;

    auto placeKnob = [&](std::unique_ptr<gm::Knob>& knob, int index) {
        if (knob)
            knob->setBounds (knobStartX + index * (knobW + knobGap), curY, knobW, knobH);
    };

    placeKnob (filterCutoffKnob, 0);
    placeKnob (filterResKnob,    1);
    placeKnob (chorusRateKnob,   2);
    placeKnob (chorusMixKnob,    3);
    placeKnob (reverbDecayKnob,  4);
    placeKnob (reverbDampKnob,   5);
    placeKnob (reverbMixKnob,    6);

    curY += knobH + 8;

    // Remaining area
    auto remainingArea = getLocalBounds().withTrimmedTop (curY).reduced (16, 0);

    // Chord display
    chordLabel.setBounds (remainingArea.removeFromTop (40));

    // LED Meters (right side of gamepad area)
    int meterW = 14;
    int meterH = 120;
    int meterX = getWidth() - 40;
    int meterY = 340;
    meterL.setBounds (meterX, meterY, meterW, meterH);
    meterR.setBounds (meterX + meterW + 4, meterY, meterW, meterH);

    // Master Volume knob (above meters)
    int knobSize = 64;
    int knobXv = meterX - (knobSize / 2) + meterW;  // centered over meter pair
    int knobYv = meterY - knobSize - 8;
    if (masterVolumeKnob)
        masterVolumeKnob->setBounds (knobXv, knobYv, knobSize, knobSize + 20);  // +20 for label/value

    // Status at bottom
    statusLabel.setBounds (getLocalBounds().removeFromBottom (24).reduced (16, 0));
}
