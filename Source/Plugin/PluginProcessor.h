#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "Engine/ChordEngine.h"
#include "Engine/StrumEngine.h"
#include "Engine/ButtonRoleMap.h"
#include "Engine/ModulationRouter.h"
#include "Input/XInputGamepad.h"

class JoychordProcessor : public juce::AudioProcessor,
                          private juce::Timer
{
public:
    JoychordProcessor();
    ~JoychordProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi()  const override { return false; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int  getNumPrograms()                                   override { return 1; }
    int  getCurrentProgram()                                override { return 0; }
    void setCurrentProgram (int)                            override {}
    const juce::String getProgramName (int)                 override { return {}; }
    void changeProgramName (int, const juce::String&)       override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    // UI-readable state (updated from audio thread via atomic)
    std::atomic<int>  currentChordRoot{-1};   // MIDI note of chord root, or -1
    std::atomic<bool> btnAState{false}, btnBState{false}, btnXState{false}, btnYState{false};
    std::atomic<bool> gamepadConnected{false};

    // Last chord name for UI display (not atomic -- read on timer, written on audio)
    juce::String lastChordName;

    // Gamepad setup
    void setControllerIndex (int idx) { gamepad.setControllerIndex (idx); }

private:
    void timerCallback() override; // poll gamepad at ~100Hz

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    ChordEngine      chordEngine;
    StrumEngine      strumEngine;
    ButtonRoleMap    roleMap;
    ModulationRouter modulationRouter;
    XInputGamepad    gamepad;

    juce::Synthesiser synth;

    // Active MIDI notes for proper note-off tracking
    std::vector<int> activeNotes;

    // Gamepad state snapshot (written by timer thread, read by audio thread)
    GamepadState latestGamepadState;
    juce::SpinLock gamepadLock;

    // Hardcoded button-to-degree map for Phase 2.5
    // A=I, B=V, X=IV, Y=vi
    static constexpr int kDegreeA = 1;  // I
    static constexpr int kDegreeB = 5;  // V
    static constexpr int kDegreeX = 4;  // IV
    static constexpr int kDegreeY = 6;  // vi

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JoychordProcessor)
};
