#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Synth/SimpleVoice.h"
#include <spdlog/spdlog.h>
#include <tsf.h>

JoychordProcessor::JoychordProcessor()
    : AudioProcessor (BusesProperties()
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "JOYCHORD", createParameterLayout())
{
    // Add a couple of voices to the analog synth
    analogSynth.addSound (new SimpleSound());
    for (int i = 0; i < 4; ++i)
        analogSynth.addVoice (new SimpleVoice());

#if JOYCHORD_HAS_SFIZZ
    sfzSynth = std::make_unique<sfz::Sfizz>();
#endif
    
    // Load default asset relative to app (will act as the PIANO tier for now)
    auto appDir = juce::File::getSpecialLocation (juce::File::currentExecutableFile).getParentDirectory();
    auto sf2File = appDir.getChildFile ("../assets/default_piano.sf2");
    if (!sf2File.existsAsFile()) sf2File = appDir.getChildFile ("../assets/TimGM6mb.sf2");
    if (!sf2File.existsAsFile()) sf2File = appDir.getChildFile ("../assets/UprightPianoKW-small-20190703.sf2");

    if (sf2File.existsAsFile())
    {
        tsfSynth = tsf_load_filename(sf2File.getFullPathName().toUTF8().getAddress());
        if (tsfSynth)
            spdlog::info ("TSF successfully loaded default SF2 asset: {}", sf2File.getFullPathName().toStdString());
        else
            spdlog::error ("TSF failed to load default SF2 asset: {}", sf2File.getFullPathName().toStdString());
    }
    else
    {
        spdlog::warn ("TSF could not find any default .sf2 asset to load in assets folder.");
    }

#if JOYCHORD_HAS_SFIZZ
    auto sfzFile = appDir.getChildFile ("../assets/basic_sine.sfz");
    if (sfzFile.existsAsFile())
    {
        if (sfzSynth->loadSfzFile (sfzFile.getFullPathName().toStdString()))
            spdlog::info ("Sfizz successfully loaded default SFZ asset: {}", sfzFile.getFullPathName().toStdString());
        else
            spdlog::error ("Sfizz failed to load default SFZ asset: {}", sfzFile.getFullPathName().toStdString());
    }
#endif
}

JoychordProcessor::~JoychordProcessor()
{
    stopTimer();
    if (tsfSynth) tsf_close(tsfSynth);
}

juce::AudioProcessorValueTreeState::ParameterLayout JoychordProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterInt>  ("key",           "Key",            0, 11, 0));
    layout.add (std::make_unique<juce::AudioParameterFloat>("strumSpeed",    "Strum Speed",    0.0f, 150.0f, 10.0f));
    layout.add (std::make_unique<juce::AudioParameterChoice>("scale",         "Scale",
        juce::StringArray { "Major", "Minor", "Dorian", "Mixolydian", "Phrygian", "Lydian", "Whole Tone" }, 0));
    layout.add (std::make_unique<juce::AudioParameterChoice>("voicing",       "Voicing",
        juce::StringArray { "Close", "Drop-2" }, 0));
    layout.add (std::make_unique<juce::AudioParameterChoice>("synthMode",     "Synth Mode",
        juce::StringArray { "MIDI", "SYNTH", "PIANO", "SFZ" }, 1)); // Default to SYNTH
    layout.add (std::make_unique<juce::AudioParameterInt>  ("midiChannel",    "MIDI Channel",   1, 16, 1));
    layout.add (std::make_unique<juce::AudioParameterInt>  ("octave",         "Octave",         2, 6, 4));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("masterVolume", 1), "Master Volume",
        juce::NormalisableRange<float> (-60.0f, 6.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("dB")
            .withStringFromValueFunction ([](float v, int) {
                if (v <= -59.9f) return juce::String ("-inf");
                return juce::String (v, 1) + " dB";
            })));

    // Effects parameters
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("reverbDecay", 1), "Reverb Decay",
        juce::NormalisableRange<float> (0.0f, 0.85f, 0.01f), 0.5f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("reverbDamp", 1), "Reverb Damp",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.4f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("reverbMix", 1), "Reverb Mix",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.25f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("filterCutoff", 1), "Filter Cutoff",
        juce::NormalisableRange<float> (20.0f, 20000.0f, 1.0f, 0.3f), 8000.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("filterRes", 1), "Filter Resonance",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.1f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("chorusRate", 1), "Chorus Rate",
        juce::NormalisableRange<float> (0.1f, 5.0f, 0.01f), 0.5f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("chorusMix", 1), "Chorus Mix",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));

    // Effect enable toggles
    layout.add (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID ("filterEnabled", 1), "Filter Enabled", true));
    layout.add (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID ("chorusEnabled", 1), "Chorus Enabled", false));
    layout.add (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID ("reverbEnabled", 1), "Reverb Enabled", true));

    // Compressor
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("compThreshold", 1), "Comp Threshold",
        juce::NormalisableRange<float> (-60.0f, 0.0f, 0.1f), -18.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("compRatio", 1), "Comp Ratio",
        juce::NormalisableRange<float> (1.0f, 20.0f, 0.1f), 4.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("compAttack", 1), "Comp Attack",
        juce::NormalisableRange<float> (0.1f, 100.0f, 0.1f), 5.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("compRelease", 1), "Comp Release",
        juce::NormalisableRange<float> (10.0f, 1000.0f, 1.0f), 100.0f));
    layout.add (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID ("compEnabled", 1), "Compressor Enabled", false));

    // Flanger
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("flangerRate", 1), "Flanger Rate",
        juce::NormalisableRange<float> (0.05f, 2.0f, 0.01f), 0.3f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("flangerDepth", 1), "Flanger Depth",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.5f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("flangerFeedback", 1), "Flanger Feedback",
        juce::NormalisableRange<float> (-0.85f, 0.85f, 0.01f), 0.3f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("flangerMix", 1), "Flanger Mix",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID ("flangerEnabled", 1), "Flanger Enabled", false));

    // Phaser
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("phaserRate", 1), "Phaser Rate",
        juce::NormalisableRange<float> (0.05f, 2.0f, 0.01f), 0.4f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("phaserDepth", 1), "Phaser Depth",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.5f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("phaserFeedback", 1), "Phaser Feedback",
        juce::NormalisableRange<float> (0.0f, 0.85f, 0.01f), 0.5f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("phaserMix", 1), "Phaser Mix",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID ("phaserEnabled", 1), "Phaser Enabled", false));

    // PingPong Delay
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("delayTime", 1), "Delay Time",
        juce::NormalisableRange<float> (50.0f, 2000.0f, 1.0f), 375.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("delayFeedback", 1), "Delay Feedback",
        juce::NormalisableRange<float> (0.0f, 0.85f, 0.01f), 0.4f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("delayMix", 1), "Delay Mix",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID ("delayEnabled", 1), "Delay Enabled", false));

    // Shimmer Reverb
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("shimmerDecay", 1), "Shimmer Decay",
        juce::NormalisableRange<float> (0.0f, 0.95f, 0.01f), 0.7f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("shimmerAmount", 1), "Shimmer Amount",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.3f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("shimmerMix", 1), "Shimmer Mix",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID ("shimmerEnabled", 1), "Shimmer Enabled", false));

    // Dither
    layout.add (std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID ("ditherBits", 1), "Dither Bits", 8, 24, 24));
    layout.add (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID ("ditherEnabled", 1), "Dither Enabled", false));

    return layout;
}

void JoychordProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    analogSynth.setCurrentPlaybackSampleRate (sampleRate);
    for (int i = 0; i < analogSynth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SimpleVoice*> (analogSynth.getVoice (i)))
            voice->prepareToPlay (sampleRate);
    }

#if JOYCHORD_HAS_SFIZZ
    if (sfzSynth != nullptr)
    {
        sfzSynth->setSampleRate (static_cast<float> (sampleRate));
        sfzSynth->setSamplesPerBlock (samplesPerBlock);
    }
#endif

    if (tsfSynth)
    {
        tsf_set_output (tsfSynth, TSF_STEREO_INTERLEAVED, static_cast<int> (sampleRate), 0);
    }
    tsfInterleavedBuffer.resize (samplesPerBlock * 2);

    // Ghostmoon DSP
    safetyLimiter.prepare (sampleRate);

    // Effects chain
    reverb.prepare (static_cast<float> (sampleRate));
    filterL.prepare (sampleRate);
    filterR.prepare (sampleRate);
    chorus.prepare (static_cast<float> (sampleRate));
    compressor.prepare (sampleRate);
    flanger.prepare (static_cast<float> (sampleRate));
    phaser.prepare (static_cast<float> (sampleRate));
    pingPongDelay.prepare (static_cast<float> (sampleRate));
    shimmerReverb.prepare (static_cast<float> (sampleRate));

    // Param smoothers
    smoothReverbDecay.prepare (sampleRate);
    smoothReverbDamp.prepare (sampleRate);
    smoothReverbMix.prepare (sampleRate);
    smoothFilterCutoff.prepare (sampleRate);
    smoothFilterRes.prepare (sampleRate);
    smoothChorusRate.prepare (sampleRate);
    smoothChorusMix.prepare (sampleRate);
    smoothCompThresh.prepare (sampleRate);
    smoothCompRatio.prepare (sampleRate);
    smoothFlangerRate.prepare (sampleRate);
    smoothFlangerMix.prepare (sampleRate);
    smoothPhaserRate.prepare (sampleRate);
    smoothPhaserMix.prepare (sampleRate);
    smoothDelayTime.prepare (sampleRate);
    smoothDelayFb.prepare (sampleRate);
    smoothDelayMix.prepare (sampleRate);
    smoothShimmerDecay.prepare (sampleRate);
    smoothShimmerAmt.prepare (sampleRate);
    smoothShimmerMix.prepare (sampleRate);

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

void JoychordProcessor::loadCustomSfz (const juce::String& path)
{
    if (path.endsWithIgnoreCase (".sf2"))
    {
        if (tsfSynth) tsf_close (tsfSynth);
        tsfSynth = tsf_load_filename (path.toUTF8().getAddress());
        if (tsfSynth)
        {
            tsf_set_output (tsfSynth, TSF_STEREO_INTERLEAVED, static_cast<int> (getSampleRate()), 0);
            spdlog::info ("TSF successfully loaded custom SF2: {}", path.toStdString());
        }
        else
        {
            spdlog::error ("TSF failed to load custom SF2: {}", path.toStdString());
        }
    }
    else
    {
#if JOYCHORD_HAS_SFIZZ
        if (sfzSynth != nullptr)
        {
            currentSfzPath = path;
            if (sfzSynth->loadSfzFile (path.toStdString()))
                spdlog::info ("Sfizz successfully loaded custom SFZ: {}", path.toStdString());
            else
                spdlog::error ("Sfizz failed to load custom SFZ: {}", path.toStdString());
        }
#endif
    }
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
    cpuMeter.startBlock();
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

    float strumSpeedMs = *apvts.getRawParameterValue ("strumSpeed");
    strumEngine.setStrumSpeedMs (strumSpeedMs);

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

    std::set<int> wantedNotes;
    juce::String chordDisplay;

    for (auto btn : allButtons)
    {
        bool pressed = getButtonState (gp, btn);
        if (!pressed) continue;

        const auto& role = roleMap.getRole (btn);

        std::visit ([&](auto&& r) {
            using T = std::decay_t<decltype(r)>;
            ChordResult result;

            bool producedNotes = false;

            if constexpr (std::is_same_v<T, RoleChord>)
            {
                result = chordEngine.resolve (r.degree, activeExtension, -1); // Dijkstra auto-voicing
                producedNotes = true;
            }
            else if constexpr (std::is_same_v<T, RoleBorrowed>)
            {
                result = chordEngine.resolveBorrowed (r.degree,
                    static_cast<ScaleMode>(r.sourceScaleIdx), activeExtension, -1);
                producedNotes = true;
            }
            else if constexpr (std::is_same_v<T, RoleChromatic>)
            {
                result = chordEngine.resolveChromatic (r.rootMidi, r.quality);
                producedNotes = true;
            }

            if (producedNotes)
            {
                for (int n : result.midiNotes)
                {
                    wantedNotes.insert (n);
                    if (n > 96)
                        spdlog::warn ("GHOST? ChordEngine produced high note: {} (name: {})", n, result.name);
                }

                if (chordDisplay.isNotEmpty())
                    chordDisplay += " + ";
                chordDisplay += juce::String (result.name);
            }
        }, role);
    }

    // ── Phase 3: Note diff -> MIDI events ──

    std::vector<int> notesToTurnOff;
    std::set_difference (activeNotes.begin(), activeNotes.end(),
                         wantedNotes.begin(), wantedNotes.end(),
                         std::back_inserter (notesToTurnOff));

    for (int note : notesToTurnOff)
    {
        spdlog::debug ("NoteOff: {}", note);
        midi.addEvent (juce::MidiMessage::noteOff (1, note, 0.0f), 0);
        strumEngine.cancelNote (note); // Remove from pending queue if not yet fired
    }

    std::vector<int> notesToTurnOn;
    std::set_difference (wantedNotes.begin(), wantedNotes.end(),
                         activeNotes.begin(), activeNotes.end(),
                         std::back_inserter (notesToTurnOn));

    for (int note : notesToTurnOn)
        spdlog::debug ("NoteOn queued: {}{}", note, note > 96 ? " *** GHOST CANDIDATE ***" : "");

    if (!notesToTurnOn.empty())
        strumEngine.triggerNotes (notesToTurnOn, 0.8f, getSampleRate());

    if (!notesToTurnOn.empty() || !notesToTurnOff.empty())
    {
        spdlog::debug ("ActiveNotes prev: {}, TurnOn: {}, TurnOff: {}, ActiveNotes next: {}", 
            activeNotes.size(), notesToTurnOn.size(), notesToTurnOff.size(), wantedNotes.size());
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
        currentChordRoot.store (*wantedNotes.begin());
        lastChordName = chordDisplay;
    }

    if (! wantedNotes.empty())
    {
        std::vector<int> vNotes (wantedNotes.begin(), wantedNotes.end());
        chordEngine.commitVoicing (vNotes);
    }

    // Save previous state for edge detection
    prevGamepadState = gp;

    // Process strum queue
    strumEngine.process (midi, buffer.getNumSamples());

    // ── Phase 3b: Modulation Router ──
    // Route analog sticks and triggers to MIDI CC / pitch bend.
    // Callbacks inject events into the midi buffer at position 0.
    modulationRouter.setCallbacks (
        [&midi](int cc, int value) {
            midi.addEvent (juce::MidiMessage::controllerEvent (1, cc, value), 0);
        },
        [&midi](int bendValue) {
            midi.addEvent (juce::MidiMessage::pitchWheel (1, bendValue), 0);
        }
    );

    modulationRouter.processLeftStick  (gp.lStickX, gp.lStickY);
    modulationRouter.processRightStick (gp.rStickX, gp.rStickY);
    modulationRouter.processTriggers   (gp.lt, gp.rt);

    // ── Phase 4: Audio Rendering ──

    int activeSynthMode = static_cast<int> (*apvts.getRawParameterValue ("synthMode"));
    // 0 = MIDI, 1 = SYNTH, 2 = PIANO, 3 = SFZ

    if (activeSynthMode == 1)
    {
        analogSynth.renderNextBlock (buffer, midi, 0, buffer.getNumSamples());
    }
    else if (activeSynthMode == 2 || activeSynthMode == 3)
    {
        bool useTsf = false;
        if (tsfSynth)
        {
            // If piano mode, prefer TSF. If SFZ mode, check if we loaded a .sf2 file into TSF recently.
            if (activeSynthMode == 2 || currentSfzPath.endsWithIgnoreCase (".sf2"))
                useTsf = true;
        }

        if (useTsf)
        {
            for (const auto meta : midi)
            {
                auto msg = meta.getMessage();
                if (msg.isNoteOn())
                {
                    spdlog::trace ("TSF NoteOn: {} vel: {:.2f}", msg.getNoteNumber(), msg.getFloatVelocity());
                    tsf_note_on (tsfSynth, 0, msg.getNoteNumber(), msg.getFloatVelocity());
                }
                else if (msg.isNoteOff())
                {
                    spdlog::trace ("TSF NoteOff: {} vel: {:.2f}", msg.getNoteNumber(), msg.getFloatVelocity());
                    tsf_note_off (tsfSynth, 0, msg.getNoteNumber());
                }
                else if (msg.isPitchWheel())
                    tsf_channel_set_pitchwheel (tsfSynth, 0, msg.getPitchWheelValue());
                else if (msg.isController())
                    tsf_channel_midi_control (tsfSynth, 0, msg.getControllerNumber(), msg.getControllerValue());
            }

            int numStereoOutputs = buffer.getNumChannels() == 2 ? 1 : 0; 
            if (numStereoOutputs == 1)
            {
                // Ensure temporary buffer is large enough (could have changed if host passes larger buffer than prepareToPlay)
                if (tsfInterleavedBuffer.size() < (size_t)buffer.getNumSamples() * 2)
                    tsfInterleavedBuffer.resize (buffer.getNumSamples() * 2);

                tsf_render_float (tsfSynth, tsfInterleavedBuffer.data(), buffer.getNumSamples(), 0);

                // Deinterleave the buffer directly into JUCE's discrete mono-channel buffers
                const float* src = tsfInterleavedBuffer.data();
                float* dstL = buffer.getWritePointer(0);
                float* dstR = buffer.getWritePointer(1);
                for (int i = 0; i < buffer.getNumSamples(); ++i)
                {
                    dstL[i] = *src++;
                    dstR[i] = *src++;
                }
            }
        }
        else
        {
#if JOYCHORD_HAS_SFIZZ
            if (sfzSynth != nullptr)
            {
                for (const auto meta : midi)
                {
                    auto msg = meta.getMessage();
                    int time = meta.samplePosition;

                    if (msg.isNoteOn())
                    {
                        spdlog::trace ("SFZ NoteOn: {} vel: {:.2f} time: {}", msg.getNoteNumber(), msg.getFloatVelocity(), time);
                        sfzSynth->hdNoteOn (time, msg.getNoteNumber(), msg.getFloatVelocity());
                    }
                    else if (msg.isNoteOff())
                    {
                        spdlog::trace ("SFZ NoteOff: {} vel: {:.2f} time: {}", msg.getNoteNumber(), msg.getFloatVelocity(), time);
                        sfzSynth->hdNoteOff (time, msg.getNoteNumber(), msg.getFloatVelocity());
                    }
                    else if (msg.isPitchWheel())
                        sfzSynth->pitchWheel (time, msg.getPitchWheelValue());
                    else if (msg.isController())
                        sfzSynth->cc (time, msg.getControllerNumber(), msg.getControllerValue());
                }

                int numStereoOutputs = buffer.getNumChannels() == 2 ? 1 : 0; 
                if (numStereoOutputs == 1)
                {
                    float* ptrs[2] = { buffer.getWritePointer(0), buffer.getWritePointer(1) };
                    sfzSynth->renderBlock (ptrs, buffer.getNumSamples(), numStereoOutputs);
                }
            }
#endif
        }
    }
    // If activeSynthMode == 0 (MIDI), buffer remains clear.

    // Apply dynamic equal-power polyphony gain scaling: 0.5 / sqrt(N)
    // We use a base multiplier of 0.5f (-6dB) to provide permanent headroom
    // for all transient sounds, even when playing only 1 note.
    float targetPolyGain = 0.5f;
    if (! activeNotes.empty())
        targetPolyGain = 0.5f / std::sqrt (static_cast<float> (activeNotes.size()));

    // Prevent snapping by ramping gain smoothly across the block
    buffer.applyGainRamp (0, buffer.getNumSamples(), currentPolyGain, targetPolyGain);
    currentPolyGain = targetPolyGain;

    // Master volume (dB to linear)
    float volDb = apvts.getRawParameterValue ("masterVolume")->load();
    float volLin = (volDb <= -59.9f) ? 0.0f : std::pow (10.0f, volDb / 20.0f);
    buffer.applyGain (volLin);

    // ── Effects Chain (per-sample) ──
    {
        float rvDecay  = apvts.getRawParameterValue ("reverbDecay")->load();
        float rvDamp   = apvts.getRawParameterValue ("reverbDamp")->load();
        float rvMix    = apvts.getRawParameterValue ("reverbMix")->load();
        float fCut     = apvts.getRawParameterValue ("filterCutoff")->load();
        float fRes     = apvts.getRawParameterValue ("filterRes")->load();
        float chRate   = apvts.getRawParameterValue ("chorusRate")->load();
        float chMix    = apvts.getRawParameterValue ("chorusMix")->load();

        float cmpThresh = apvts.getRawParameterValue ("compThreshold")->load();
        float cmpRatio  = apvts.getRawParameterValue ("compRatio")->load();
        float cmpAtk    = apvts.getRawParameterValue ("compAttack")->load();
        float cmpRel    = apvts.getRawParameterValue ("compRelease")->load();

        float flRate   = apvts.getRawParameterValue ("flangerRate")->load();
        float flDepth  = apvts.getRawParameterValue ("flangerDepth")->load();
        float flFb     = apvts.getRawParameterValue ("flangerFeedback")->load();
        float flMix    = apvts.getRawParameterValue ("flangerMix")->load();

        float phRate   = apvts.getRawParameterValue ("phaserRate")->load();
        float phDepth  = apvts.getRawParameterValue ("phaserDepth")->load();
        float phFb     = apvts.getRawParameterValue ("phaserFeedback")->load();
        float phMix    = apvts.getRawParameterValue ("phaserMix")->load();

        float dlTime   = apvts.getRawParameterValue ("delayTime")->load();
        float dlFb     = apvts.getRawParameterValue ("delayFeedback")->load();
        float dlMix    = apvts.getRawParameterValue ("delayMix")->load();

        float shDecay  = apvts.getRawParameterValue ("shimmerDecay")->load();
        float shAmt    = apvts.getRawParameterValue ("shimmerAmount")->load();
        float shMix    = apvts.getRawParameterValue ("shimmerMix")->load();

        int   dtBits   = static_cast<int> (*apvts.getRawParameterValue ("ditherBits"));

        auto* L = buffer.getWritePointer (0);
        auto* R = buffer.getNumChannels() > 1 ? buffer.getWritePointer (1) : nullptr;
        int numSamples = buffer.getNumSamples();

        bool filtOn   = apvts.getRawParameterValue ("filterEnabled")->load() > 0.5f;
        bool compOn   = apvts.getRawParameterValue ("compEnabled")->load() > 0.5f;
        bool chOn     = apvts.getRawParameterValue ("chorusEnabled")->load() > 0.5f;
        bool flOn     = apvts.getRawParameterValue ("flangerEnabled")->load() > 0.5f;
        bool phOn     = apvts.getRawParameterValue ("phaserEnabled")->load() > 0.5f;
        bool dlOn     = apvts.getRawParameterValue ("delayEnabled")->load() > 0.5f;
        bool rvOn     = apvts.getRawParameterValue ("reverbEnabled")->load() > 0.5f;
        bool shOn     = apvts.getRawParameterValue ("shimmerEnabled")->load() > 0.5f;
        bool dtOn     = apvts.getRawParameterValue ("ditherEnabled")->load() > 0.5f;

        // Set compressor params (block-level, not per-sample)
        if (compOn)
        {
            compressor.setThreshold (cmpThresh);
            compressor.setRatio (cmpRatio);
            compressor.setAttack (cmpAtk);
            compressor.setRelease (cmpRel);
        }

        for (int i = 0; i < numSamples; ++i)
        {
            // Smooth all parameters
            float srvDecay = smoothReverbDecay.process (rvDecay);
            float srvDamp  = smoothReverbDamp.process (rvDamp);
            float srvMix   = smoothReverbMix.process (rvMix);
            float sfCut    = smoothFilterCutoff.process (fCut);
            float sfRes    = smoothFilterRes.process (fRes);
            float schRate  = smoothChorusRate.process (chRate);
            float schMix   = smoothChorusMix.process (chMix);
            float sFlRate  = smoothFlangerRate.process (flRate);
            float sFlMix   = smoothFlangerMix.process (flMix);
            float sPhRate  = smoothPhaserRate.process (phRate);
            float sPhMix   = smoothPhaserMix.process (phMix);
            float sDlTime  = smoothDelayTime.process (dlTime);
            float sDlFb    = smoothDelayFb.process (dlFb);
            float sDlMix   = smoothDelayMix.process (dlMix);
            float sShDecay = smoothShimmerDecay.process (shDecay);
            float sShAmt   = smoothShimmerAmt.process (shAmt);
            float sShMix   = smoothShimmerMix.process (shMix);

            // 1. Filter (MoogLadder LP24)
            if (filtOn)
            {
                filterL.setFreq (static_cast<double> (sfCut));
                filterL.setRes (static_cast<double> (sfRes * 1.8));
                filterR.setFreq (static_cast<double> (sfCut));
                filterR.setRes (static_cast<double> (sfRes * 1.8));
                L[i] = filterL.process (L[i]);
                if (R) R[i] = filterR.process (R[i]);
            }

            // 2. Compressor (per-sample via processSample for now)
            if (compOn)
            {
                L[i] = compressor.processSample (L[i]);
                if (R) R[i] = compressor.processSample (R[i]);
            }

            // 3. Chorus
            if (chOn && schMix > 0.001f)
            {
                chorus.setRate (schRate);
                chorus.setMix (schMix);
                float cL = L[i], cR = R ? R[i] : L[i];
                chorus.processSample (cL, cR, L[i], R ? R[i] : cL);
            }

            // 4. Flanger
            if (flOn && sFlMix > 0.001f)
            {
                flanger.setRate (sFlRate);
                flanger.setDepth (flDepth);
                flanger.setFeedback (flFb);
                flanger.setMix (sFlMix);
                float fL = L[i], fR = R ? R[i] : L[i];
                flanger.processSample (fL, fR, L[i], R ? R[i] : fL);
            }

            // 5. Phaser
            if (phOn && sPhMix > 0.001f)
            {
                phaser.setRate (sPhRate);
                phaser.setDepth (phDepth);
                phaser.setFeedback (phFb);
                phaser.setMix (sPhMix);
                float pL = L[i], pR = R ? R[i] : L[i];
                phaser.processSample (pL, pR, L[i], R ? R[i] : pL);
            }

            // 6. PingPong Delay
            if (dlOn && sDlMix > 0.001f)
            {
                pingPongDelay.setTime (sDlTime / 1000.0f); // ms to seconds
                pingPongDelay.setFeedback (sDlFb);
                pingPongDelay.setMix (sDlMix);
                float dL = L[i], dR = R ? R[i] : L[i];
                pingPongDelay.processSample (dL, dR, L[i], R ? R[i] : dL);
            }

            // 7. Reverb (PlateReverb)
            if (rvOn && srvMix > 0.001f)
            {
                reverb.setDecay (srvDecay);
                reverb.setDamping (srvDamp);
                reverb.setMix (srvMix);
                float rL = L[i], rR = R ? R[i] : L[i];
                reverb.processSample (rL, rR, L[i], R ? R[i] : rL);
            }

            // 8. Shimmer Reverb
            if (shOn && sShMix > 0.001f)
            {
                shimmerReverb.setDecay (sShDecay);
                shimmerReverb.setShimmer (sShAmt);
                shimmerReverb.setMix (sShMix);
                float sL = L[i], sR = R ? R[i] : L[i];
                shimmerReverb.processSample (sL, sR, L[i], R ? R[i] : sL);
            }

            // 9. Dither
            if (dtOn)
            {
                L[i] = ditherL.process (L[i], dtBits);
                if (R) R[i] = ditherR.process (R[i], dtBits);
            }
        }
    }

    // Ghostmoon safety chain: NaN guard, DC blocker, soft limiter, hard clip
    auto* L = buffer.getWritePointer (0);
    auto* R = buffer.getNumChannels() > 1 ? buffer.getWritePointer (1) : nullptr;
    safetyLimiter.process (L, R, buffer.getNumSamples());

    // Metering (feeds UI level meters)
    meterSource.process (L, buffer.getNumSamples(), 0);
    if (R) meterSource.process (R, buffer.getNumSamples(), 1);

    cpuMeter.endBlock (getSampleRate(), buffer.getNumSamples());
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
