#include "PluginEditor.h"

static const juce::StringArray kNoteNames {"C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"};

JoychordEditor::JoychordEditor (JoychordProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (480, 380);

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

    // Gamepad controller index selector
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

    // UI refresh at 30Hz
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
    // Background
    g.fillAll (juce::Colour (0xff1a1a2e));

    // Title
    g.setColour (juce::Colour (0xffe0e0ff));
    g.setFont (juce::FontOptions (20.0f));
    g.drawText ("Joychord", 0, 8, getWidth(), 28, juce::Justification::centred);

    // Button indicators -- diamond layout mimicking Xbox face buttons
    auto cx = getWidth() / 2;
    auto cy = 290;
    int r = 20;
    int spacing = 30;

    auto drawBtn = [&](int x, int y, const juce::String& label, bool pressed, juce::Colour col)
    {
        g.setColour (pressed ? col : col.withAlpha (0.2f));
        g.fillEllipse ((float)(x - r), (float)(y - r), (float)(r * 2), (float)(r * 2));
        g.setColour (juce::Colours::white);
        g.drawEllipse ((float)(x - r), (float)(y - r), (float)(r * 2), (float)(r * 2), 1.5f);
        g.setFont (juce::FontOptions (14.0f));
        g.drawText (label, x - r, y - r, r * 2, r * 2, juce::Justification::centred);
    };

    // Xbox diamond: Y top, X left, B right, A bottom
    drawBtn (cx,            cy - spacing, "Y",  btnY, juce::Colour (0xffccaa00)); // yellow
    drawBtn (cx - spacing,  cy,           "X",  btnX, juce::Colour (0xff0066cc)); // blue
    drawBtn (cx + spacing,  cy,           "B",  btnB, juce::Colour (0xffcc2200)); // red
    drawBtn (cx,            cy + spacing, "A",  btnA, juce::Colour (0xff00aa44)); // green

    // Degree labels next to buttons
    g.setFont (juce::FontOptions (11.0f));
    g.setColour (juce::Colour (0xff909090));
    g.drawText ("vi",  cx + r + 2,            cy - spacing - 8, 30, 16, juce::Justification::centredLeft);
    g.drawText ("IV",  cx - spacing - r - 32, cy - 8,           30, 16, juce::Justification::centredRight);
    g.drawText ("V",   cx + spacing + r + 2,  cy - 8,           30, 16, juce::Justification::centredLeft);
    g.drawText ("I",   cx + r + 2,            cy + spacing - 8, 30, 16, juce::Justification::centredLeft);
}

void JoychordEditor::resized()
{
    auto area = getLocalBounds().reduced (16);
    area.removeFromTop (36); // title

    auto labelW = 60;
    auto boxW = 100;
    auto gap = 12;

    // Row 1: Key + Scale
    auto row = area.removeFromTop (28);
    keyLabel.setBounds   (row.removeFromLeft (labelW));
    keyBox.setBounds     (row.removeFromLeft (boxW));
    row.removeFromLeft (gap);
    scaleLabel.setBounds (row.removeFromLeft (labelW));
    scaleBox.setBounds   (row.removeFromLeft (boxW));

    area.removeFromTop (8);

    // Row 2: Voicing + Octave
    row = area.removeFromTop (28);
    voicingLabel.setBounds (row.removeFromLeft (labelW));
    voicingBox.setBounds   (row.removeFromLeft (boxW));
    row.removeFromLeft (gap);
    octaveLabel.setBounds (row.removeFromLeft (labelW));
    octaveBox.setBounds   (row.removeFromLeft (boxW));

    area.removeFromTop (8);

    // Row 3: Gamepad setup
    row = area.removeFromTop (28);
    gamepadLabel.setBounds    (row.removeFromLeft (labelW));
    gamepadIndexBox.setBounds (row.removeFromLeft (140));

    area.removeFromTop (16);

    // Chord display
    chordLabel.setBounds (area.removeFromTop (50));

    // Status at bottom
    statusLabel.setBounds (getLocalBounds().removeFromBottom (24).reduced (16, 0));
}
