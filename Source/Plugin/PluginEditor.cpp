#include "PluginEditor.h"
#include <BinaryData.h>
#include "Typography.h"

static const juce::StringArray kNoteNames {"C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"};

JoychordEditor::JoychordEditor (JoychordProcessor& p)
    : AudioProcessorEditor (&p), processor (p),
      darkTheme (
          juce::Typeface::createSystemTypefaceFor (BinaryData::InterRegular_ttf, BinaryData::InterRegular_ttfSize),
          juce::Typeface::createSystemTypefaceFor (BinaryData::InterBold_ttf, BinaryData::InterBold_ttfSize),
          juce::Typeface::createSystemTypefaceFor (BinaryData::JetBrainsMonoRegular_ttf, BinaryData::JetBrainsMonoRegular_ttfSize)
      )
{
    setSize (520, 480);

    // Init Typography singleton with same typefaces
    auto& typo = gm::Typography::getInstance();
    typo.setInterRegular  (darkTheme.getInterFont (12.0f).getTypefacePtr());
    typo.setInterBold     (darkTheme.getInterFont (12.0f, true).getTypefacePtr());
    typo.setJetBrainsMono (darkTheme.getMonoFont (12.0f).getTypefacePtr());

    // Apply ghostmoon dark metallic theme with neon cyan accent
    setLookAndFeel (&darkTheme);
    darkTheme.setColour (gm::DarkMetallicTheme::accentColourId, juce::Colour (0xff00ccff));  // Neon cyan accent

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

    // Chord display
    chordLabel.setText ("---", juce::dontSendNotification);
    chordLabel.setFont (juce::FontOptions (36.0f));
    chordLabel.setJustificationType (juce::Justification::centred);
    chordLabel.setColour (juce::Label::textColourId, juce::Colour (0xffe0e0ff));
    addAndMakeVisible (chordLabel);

    // Status
    statusLabel.setText ("Controller: not connected", juce::dontSendNotification);
    statusLabel.setFont (juce::FontOptions (12.0f));
    statusLabel.setJustificationType (juce::Justification::centredLeft);
    statusLabel.setColour (juce::Label::textColourId, juce::Colour (0xff808080));
    addAndMakeVisible (statusLabel);

    // LED Meters
    addAndMakeVisible (meterL);
    addAndMakeVisible (meterR);
    meterL.startMetering();
    meterR.startMetering();

    // Master Volume knob (LED Ladder style)
    ledLadderLnF.setColour (gm::DarkMetallicTheme::accentColourId, juce::Colour (0xff00ccff));
    masterVolumeKnob = std::make_unique<gm::Knob> (processor.apvts, "masterVolume", "Volume", "Master output volume");
    masterVolumeKnob->getSlider().setLookAndFeel (&ledLadderLnF);
    masterVolumeKnob->getSlider().setDoubleClickReturnValue (true, 0.0);  // double-click resets to 0 dB
    addAndMakeVisible (*masterVolumeKnob);

    startTimerHz (30);
}

JoychordEditor::~JoychordEditor()
{
    masterVolumeKnob->getSlider().setLookAndFeel (nullptr);
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
    g.fillAll (findColour (gm::DarkMetallicTheme::bgColourId));

    // Title
    g.setColour (juce::Colour (0xff00ccff));  // Neon cyan
    g.setFont (darkTheme.getInterFont (22.0f, true));
    g.drawText ("Joychord", 0, 8, getWidth(), 28, juce::Justification::centred);

    // ── Button indicators ──

    int r = 18;
    int spacing = 26;

    auto drawBtn = [&](int x, int y, const juce::String& label, bool pressed, juce::Colour col, int radius = 18)
    {
        g.setColour (pressed ? col : col.withAlpha (0.2f));
        g.fillEllipse ((float)(x - radius), (float)(y - radius), (float)(radius * 2), (float)(radius * 2));
        g.setColour (juce::Colours::white);
        g.drawEllipse ((float)(x - radius), (float)(y - radius), (float)(radius * 2), (float)(radius * 2), 1.5f);
        g.setFont (juce::FontOptions (12.0f));
        g.drawText (label, x - radius, y - radius, radius * 2, radius * 2, juce::Justification::centred);
    };

    // Face buttons (right side) -- Xbox diamond
    int faceCx = getWidth() / 2 + 70;
    int faceCy = 310;
    drawBtn (faceCx,              faceCy - spacing, "Y",  btnY, juce::Colour (0xffccaa00));
    drawBtn (faceCx - spacing,    faceCy,           "X",  btnX, juce::Colour (0xff0066cc));
    drawBtn (faceCx + spacing,    faceCy,           "B",  btnB, juce::Colour (0xffcc2200));
    drawBtn (faceCx,              faceCy + spacing, "A",  btnA, juce::Colour (0xff00aa44));

    // D-pad (left side) -- cross layout
    int dpadCx = getWidth() / 2 - 70;
    int dpadCy = 310;
    int ds = 22;
    int dr = 14;
    auto grey = juce::Colour (0xff666688);
    drawBtn (dpadCx,       dpadCy - ds, "U",  dUp,    grey, dr);
    drawBtn (dpadCx - ds,  dpadCy,      "L",  dLeft,  grey, dr);
    drawBtn (dpadCx + ds,  dpadCy,      "R",  dRight, grey, dr);
    drawBtn (dpadCx,       dpadCy + ds, "D",  dDown,  grey, dr);

    // Shoulders (top of button area) - Bright highlight when active
    int shoulderY = 275;
    drawBtn (dpadCx - 10, shoulderY, "LB", lb, juce::Colour (0xffff4400), 14); // Bright orange/red
    drawBtn (faceCx + 10, shoulderY, "RB", rb, juce::Colour (0xff00ccff), 14); // Bright cyan

    // Degree labels
    g.setFont (juce::FontOptions (10.0f));
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

    // Chord label glow is handled by the themed label — no manual path needed
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

    curY += rowH + 16;
    
    // Remaining area
    auto remainingArea = getLocalBounds().withTrimmedTop (curY).reduced (16, 0);

    // Chord display
    chordLabel.setBounds (remainingArea.removeFromTop (40));

    // LED Meters (right side of gamepad area)
    int meterW = 14;
    int meterH = 120;
    int meterX = getWidth() - 40;
    int meterY = 260;
    meterL.setBounds (meterX, meterY, meterW, meterH);
    meterR.setBounds (meterX + meterW + 4, meterY, meterW, meterH);

    // Master Volume knob (above meters)
    int knobSize = 64;
    int knobX = meterX - (knobSize / 2) + meterW;  // centered over meter pair
    int knobY = meterY - knobSize - 8;
    masterVolumeKnob->setBounds (knobX, knobY, knobSize, knobSize + 20);  // +20 for label/value

    // Status at bottom
    statusLabel.setBounds (getLocalBounds().removeFromBottom (24).reduced (16, 0));
}
