#include "PluginEditor.h"

JoychordEditor::JoychordEditor (JoychordProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (600, 400);
}

JoychordEditor::~JoychordEditor() {}

void JoychordEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1a1a2e));
    g.setColour (juce::Colours::white);
    g.setFont (18.0f);
    g.drawText ("Joychord", getLocalBounds(), juce::Justification::centred);
}

void JoychordEditor::resized() {}
