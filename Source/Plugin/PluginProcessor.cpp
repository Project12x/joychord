#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Synth/SimpleVoice.h"

JoychordProcessor::JoychordProcessor()
    : AudioProcessor (BusesProperties()
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "JOYCHORD", createParameterLayout())
{
    // Add SimpleSound + 12 SimpleVoice instances
    synth.addSound (new SimpleSound());
    for (int i = 0; i < 12; ++i)
        synth.addVoice (new SimpleVoice());
}

JoychordProcessor::~JoychordProcessor()
{
    stopTimer();
}

juce::AudioProcessorValueTreeState::ParameterLayout JoychordProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterInt>  ("key",           "Key",            0, 11, 0));
    layout.add (std::make_unique<juce::AudioParameterChoice>("scale",         "Scale",
        juce::StringArray { "Major", "Minor", "Dorian", "Mixolydian", "Phrygian", "Lydian", "Whole Tone" }, 0));
    layout.add (std::make_unique<juce::AudioParameterChoice>("voicing",       "Voicing",
        juce::StringArray { "Close", "Drop-2" }, 0));
    layout.add (std::make_unique<juce::AudioParameterChoice>("outputMode",    "Output Mode",
        juce::StringArray { "Synth", "MIDI" }, 0));
    layout.add (std::make_unique<juce::AudioParameterInt>  ("midiChannel",    "MIDI Channel",   1, 16, 1));
    layout.add (std::make_unique<juce::AudioParameterInt>  ("octave",         "Octave",         2, 6, 4));

    return layout;
}

void JoychordProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);

    // Init ADSR on all voices
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* sv = dynamic_cast<SimpleVoice*>(synth.getVoice(i)))
            sv->prepareToPlay (sampleRate);
    }

    // Start gamepad polling timer (~100Hz)
    startTimerHz (100);
}

void JoychordProcessor::releaseResources()
{
    stopTimer();
}

void JoychordProcessor::timerCallback()
{
    GamepadState state;
    gamepad.poll (state);

    const juce::SpinLock::ScopedLockType lock (gamepadLock);
    latestGamepadState = state;
}

void JoychordProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    // Snapshot gamepad state
    GamepadState gp;
    {
        const juce::SpinLock::ScopedLockType lock (gamepadLock);
        gp = latestGamepadState;
    }

    // Update atomic UI state
    gamepadConnected.store (gp.connected);
    btnAState.store (gp.btnA);
    btnBState.store (gp.btnB);
    btnXState.store (gp.btnX);
    btnYState.store (gp.btnY);

    // Sync APVTS params to ChordEngine
    int keyVal   = static_cast<int> (*apvts.getRawParameterValue ("key"));
    int scaleIdx = static_cast<int> (*apvts.getRawParameterValue ("scale"));
    int voicIdx  = static_cast<int> (*apvts.getRawParameterValue ("voicing"));
    int octave   = static_cast<int> (*apvts.getRawParameterValue ("octave"));

    chordEngine.setKey (keyVal);
    chordEngine.setScale (static_cast<ScaleMode> (scaleIdx));
    chordEngine.setVoicing (static_cast<VoicingStyle> (voicIdx));
    chordEngine.setOctave (octave);

    // Determine which degrees are active from button state
    std::vector<int> requestedDegrees;
    if (gp.btnA) requestedDegrees.push_back (kDegreeA);
    if (gp.btnB) requestedDegrees.push_back (kDegreeB);
    if (gp.btnX) requestedDegrees.push_back (kDegreeX);
    if (gp.btnY) requestedDegrees.push_back (kDegreeY);

    // Resolve all requested chords
    std::vector<int> wantedNotes;
    juce::String chordDisplay;

    for (int deg : requestedDegrees)
    {
        auto result = chordEngine.resolve (deg, 0, -1); // auto-voicing via Dijkstra
        for (int n : result.midiNotes)
            wantedNotes.push_back (n);

        if (chordDisplay.isNotEmpty())
            chordDisplay += " + ";
        chordDisplay += juce::String (result.name);
    }

    // Compute note-off: notes in activeNotes but not in wantedNotes
    for (int note : activeNotes)
    {
        if (std::find (wantedNotes.begin(), wantedNotes.end(), note) == wantedNotes.end())
            midi.addEvent (juce::MidiMessage::noteOff (1, note), 0);
    }

    // Compute note-on: notes in wantedNotes but not in activeNotes
    for (int note : wantedNotes)
    {
        if (std::find (activeNotes.begin(), activeNotes.end(), note) == activeNotes.end())
            midi.addEvent (juce::MidiMessage::noteOn (1, note, 0.8f), 0);
    }

    activeNotes = wantedNotes;

    // Update display state for UI
    if (wantedNotes.empty())
    {
        currentChordRoot.store (-1);
        lastChordName = "";
    }
    else
    {
        currentChordRoot.store (wantedNotes.front());
        lastChordName = chordDisplay;
    }

    // Commit voicing for Dijkstra scorer
    if (! wantedNotes.empty())
        chordEngine.commitVoicing (wantedNotes);

    // Render synth audio from MIDI
    synth.renderNextBlock (buffer, midi, 0, buffer.getNumSamples());
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
