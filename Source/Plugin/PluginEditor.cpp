#include "PluginEditor.h"

static const juce::StringArray kNoteNames {"C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"};

JoychordEditor::JoychordEditor (JoychordProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (480, 420);

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

    startTimerHz (30);
}

JoychordEditor::~JoychordEditor()
{
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

    auto name = processor.lastChordName;
    chordLabel.setText (name.isNotEmpty() ? name : "---", juce::dontSendNotification);

    statusLabel.setText (connected ? "Controller: connected" : "Controller: not connected",
                         juce::dontSendNotification);
    statusLabel.setColour (juce::Label::textColourId,
                           connected ? juce::Colour (0xff40c040) : juce::Colour (0xff808080));

    repaint();
}

void JoychordEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1a1a2e));

    // Title
    g.setColour (juce::Colour (0xffe0e0ff));
    g.setFont (juce::FontOptions (20.0f));
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

    // Shoulders (top of button area)
    int shoulderY = 275;
    drawBtn (dpadCx - 10, shoulderY, "LB", lb, juce::Colour (0xff886688), 14);
    drawBtn (faceCx + 10, shoulderY, "RB", rb, juce::Colour (0xff886688), 14);

    // Degree labels
    g.setFont (juce::FontOptions (10.0f));
    g.setColour (juce::Colour (0xff909090));
    // Face button degree labels
    g.drawText ("vi",  faceCx + r + 2,            faceCy - spacing - 6, 25, 12, juce::Justification::centredLeft);
    g.drawText ("IV",  faceCx - spacing - r - 27, faceCy - 6,          25, 12, juce::Justification::centredRight);
    g.drawText ("V",   faceCx + spacing + r + 2,  faceCy - 6,          25, 12, juce::Justification::centredLeft);
    g.drawText ("I",   faceCx + r + 2,            faceCy + spacing - 6, 25, 12, juce::Justification::centredLeft);
}

void JoychordEditor::resized()
{
    auto area = getLocalBounds().reduced (16);
    area.removeFromTop (36); // title

    auto labelW = 55;
    auto boxW = 100;
    auto gap = 12;

    // Row 1: Key + Scale
    auto row = area.removeFromTop (26);
    keyLabel.setBounds   (row.removeFromLeft (labelW));
    keyBox.setBounds     (row.removeFromLeft (boxW));
    row.removeFromLeft (gap);
    scaleLabel.setBounds (row.removeFromLeft (labelW));
    scaleBox.setBounds   (row.removeFromLeft (boxW));

    area.removeFromTop (6);

    // Row 2: Voicing + Octave
    row = area.removeFromTop (26);
    voicingLabel.setBounds (row.removeFromLeft (labelW));
    voicingBox.setBounds   (row.removeFromLeft (boxW));
    row.removeFromLeft (gap);
    octaveLabel.setBounds (row.removeFromLeft (labelW));
    octaveBox.setBounds   (row.removeFromLeft (boxW));

    area.removeFromTop (6);

    // Row 3: Preset + Gamepad
    row = area.removeFromTop (26);
    presetLabel.setBounds     (row.removeFromLeft (labelW));
    presetBox.setBounds       (row.removeFromLeft (boxW));
    row.removeFromLeft (gap);
    gamepadLabel.setBounds    (row.removeFromLeft (labelW));
    gamepadIndexBox.setBounds (row.removeFromLeft (boxW));

    area.removeFromTop (12);

    // Chord display
    chordLabel.setBounds (area.removeFromTop (50));

    // Status at bottom
    statusLabel.setBounds (getLocalBounds().removeFromBottom (24).reduced (16, 0));
}
