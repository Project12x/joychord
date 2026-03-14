#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Synth/SimpleVoice.h"

JoychordProcessor::JoychordProcessor()
    : AudioProcessor (BusesProperties()
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "JOYCHORD", createParameterLayout())
{
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

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* sv = dynamic_cast<SimpleVoice*>(synth.getVoice(i)))
            sv->prepareToPlay (sampleRate);
    }

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

void JoychordProcessor::loadPreset (const std::string& presetId)
{
    roleMap.loadPreset (presetId);
}

bool JoychordProcessor::getButtonState (const GamepadState& gp, ButtonId btn)
{
    switch (btn)
    {
        case ButtonId::A:      return gp.btnA;
        case ButtonId::B:      return gp.btnB;
        case ButtonId::X:      return gp.btnX;
        case ButtonId::Y:      return gp.btnY;
        case ButtonId::DUp:    return gp.dUp;
        case ButtonId::DDown:  return gp.dDown;
        case ButtonId::DLeft:  return gp.dLeft;
        case ButtonId::DRight: return gp.dRight;
        case ButtonId::LB:     return gp.lb;
        case ButtonId::RB:     return gp.rb;
        case ButtonId::LT:     return gp.lt > 0.5f;
        case ButtonId::RT:     return gp.rt > 0.5f;
        case ButtonId::L3:     return gp.l3;
        case ButtonId::R3:     return gp.r3;
        case ButtonId::Start:  return gp.start;
        case ButtonId::Back:   return gp.back;
        default:               return false;
    }
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
    dUpState.store (gp.dUp);
    dDownState.store (gp.dDown);
    dLeftState.store (gp.dLeft);
    dRightState.store (gp.dRight);
    lbState.store (gp.lb);
    rbState.store (gp.rb);

    // Sync APVTS params to ChordEngine
    int keyVal   = static_cast<int> (*apvts.getRawParameterValue ("key"));
    int scaleIdx = static_cast<int> (*apvts.getRawParameterValue ("scale"));
    int voicIdx  = static_cast<int> (*apvts.getRawParameterValue ("voicing"));
    int baseOctave = static_cast<int> (*apvts.getRawParameterValue ("octave"));

    chordEngine.setKey (keyVal);
    chordEngine.setScale (static_cast<ScaleMode> (scaleIdx));
    chordEngine.setVoicing (static_cast<VoicingStyle> (voicIdx));

    // ── Phase 1: Process modifier roles (extension, octave shift, key transpose) ──
    // These modify engine state but don't produce notes themselves.

    activeExtension = 0; // reset each block; held modifiers re-apply

    // All 16 buttons iterated
    static constexpr ButtonId allButtons[] = {
        ButtonId::A, ButtonId::B, ButtonId::X, ButtonId::Y,
        ButtonId::DUp, ButtonId::DDown, ButtonId::DLeft, ButtonId::DRight,
        ButtonId::LB, ButtonId::RB, ButtonId::LT, ButtonId::RT,
        ButtonId::L3, ButtonId::R3, ButtonId::Start, ButtonId::Back
    };

    for (auto btn : allButtons)
    {
        bool pressed = getButtonState (gp, btn);
        bool wasPressed = getButtonState (prevGamepadState, btn);
        const auto& role = roleMap.getRole (btn);

        std::visit ([&](auto&& r) {
            using T = std::decay_t<decltype(r)>;

            if constexpr (std::is_same_v<T, RoleExtension>)
            {
                // Held modifier: active while button is down
                if (pressed)
                    activeExtension = r.type + 1; // +1 because ChordEngine uses 1=seventh, 2=sus4, etc.
            }
            else if constexpr (std::is_same_v<T, RoleOctaveShift>)
            {
                // Edge-triggered: shift on press, not hold
                if (pressed && !wasPressed)
                {
                    octaveOffset += r.delta;
                    octaveOffset = std::clamp (octaveOffset, -2, 2);
                }
            }
            else if constexpr (std::is_same_v<T, RoleKeyTranspose>)
            {
                if (pressed && !wasPressed)
                {
                    int newKey = (keyVal + r.semitones + 12) % 12;
                    if (auto* param = apvts.getParameter ("key"))
                        param->setValueNotifyingHost (param->convertTo0to1 (static_cast<float>(newKey)));
                }
            }
            // Other roles handled in Phase 2 below
        }, role);
    }

    // Apply octave offset
    chordEngine.setOctave (baseOctave + octaveOffset);

    // ── Phase 2: Process chord-producing roles ──

    std::vector<int> wantedNotes;
    juce::String chordDisplay;

    for (auto btn : allButtons)
    {
        bool pressed = getButtonState (gp, btn);
        if (!pressed) continue;

        const auto& role = roleMap.getRole (btn);

        std::visit ([&](auto&& r) {
            using T = std::decay_t<decltype(r)>;
            ChordResult result;

            if constexpr (std::is_same_v<T, RoleChord>)
            {
                result = chordEngine.resolve (r.degree, activeExtension, -1); // Dijkstra auto-voicing
            }
            else if constexpr (std::is_same_v<T, RoleBorrowed>)
            {
                result = chordEngine.resolveBorrowed (r.degree,
                    static_cast<ScaleMode>(r.sourceScaleIdx), activeExtension, -1);
            }
            else if constexpr (std::is_same_v<T, RoleChromatic>)
            {
                result = chordEngine.resolveChromatic (r.rootMidi, r.quality);
            }
            else
            {
                return; // modifiers/strum/mute don't produce notes
            }

            for (int n : result.midiNotes)
                wantedNotes.push_back (n);

            if (chordDisplay.isNotEmpty())
                chordDisplay += " + ";
            chordDisplay += juce::String (result.name);
        }, role);
    }

    // ── Phase 3: Note diff -> MIDI events ──

    for (int note : activeNotes)
    {
        if (std::find (wantedNotes.begin(), wantedNotes.end(), note) == wantedNotes.end())
            midi.addEvent (juce::MidiMessage::noteOff (1, note), 0);
    }

    for (int note : wantedNotes)
    {
        if (std::find (activeNotes.begin(), activeNotes.end(), note) == activeNotes.end())
            midi.addEvent (juce::MidiMessage::noteOn (1, note, 0.8f), 0);
    }

    activeNotes = wantedNotes;

    // Update display state
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

    if (! wantedNotes.empty())
        chordEngine.commitVoicing (wantedNotes);

    // Save previous state for edge detection
    prevGamepadState = gp;

    // Render synth audio
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
