#include "PluginProcessor.h"
#include "PluginEditor.h"

JoychordProcessor::JoychordProcessor()
    : AudioProcessor (BusesProperties()
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "JOYCHORD", createParameterLayout())
{
}

JoychordProcessor::~JoychordProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout JoychordProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterInt>  ("key",           "Key",            0, 11, 0));
    layout.add (std::make_unique<juce::AudioParameterChoice>("scale",         "Scale",
        juce::StringArray { "major", "minor", "dorian", "mixolydian", "phrygian", "lydian", "whole_tone" }, 0));
    layout.add (std::make_unique<juce::AudioParameterChoice>("voicing",       "Voicing",
        juce::StringArray { "close", "drop2" }, 0));
    layout.add (std::make_unique<juce::AudioParameterChoice>("outputMode",    "Output Mode",
        juce::StringArray { "synth", "midi" }, 0));
    layout.add (std::make_unique<juce::AudioParameterInt>  ("midiChannel",    "MIDI Channel",   1, 16, 1));
    layout.add (std::make_unique<juce::AudioParameterChoice>("sfzPatch",      "Patch",
        juce::StringArray { "piano", "guitar_electric", "guitar_nylon", "strings", "bass" }, 0));
    layout.add (std::make_unique<juce::AudioParameterInt>  ("pitchBendRange", "Pitch Bend Range", 1, 12, 2));
    layout.add (std::make_unique<juce::AudioParameterFloat>("stickDeadzone",  "Stick Deadzone",
        juce::NormalisableRange<float> (0.0f, 0.3f, 0.01f), 0.1f));

    return layout;
}

void JoychordProcessor::prepareToPlay (double /*sampleRate*/, int /*samplesPerBlock*/) {}
void JoychordProcessor::releaseResources() {}

void JoychordProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    // TODO: poll gamepad, resolve roles, drive chord/strum/modulation engines, fill midi
    juce::ignoreUnused (midi);
}

juce::AudioProcessorEditor* JoychordProcessor::createEditor()
{
    return new JoychordEditor (*this);
}

void JoychordProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.appendChild (roleMap.toValueTree(), nullptr);
    auto xml = state.createXml();
    copyXmlToBinary (*xml, destData);
}

void JoychordProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto xml = getXmlFromBinary (data, sizeInBytes);
    if (xml == nullptr) return;

    auto state = juce::ValueTree::fromXml (*xml);
    if (! state.isValid()) return;

    apvts.replaceState (state);

    auto roleTree = state.getChildWithName ("ROLEMAP");
    if (roleTree.isValid())
        roleMap.fromValueTree (roleTree);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JoychordProcessor();
}
