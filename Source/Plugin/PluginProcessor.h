#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "Engine/ChordEngine.h"
#include "Engine/StrumEngine.h"
#include "Engine/ButtonRoleMap.h"
#include "Engine/ModulationRouter.h"
#include "Input/XInputGamepad.h"

#if JOYCHORD_HAS_SFIZZ
#include <sfizz.hpp>
#endif

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

    // UI-readable state
    std::atomic<int>  currentChordRoot{-1};
    std::atomic<bool> btnAState{false}, btnBState{false}, btnXState{false}, btnYState{false};
    std::atomic<bool> dUpState{false}, dDownState{false}, dLeftState{false}, dRightState{false};
    std::atomic<bool> lbState{false}, rbState{false};
    std::atomic<bool> gamepadConnected{false};
    juce::String lastChordName;

    // Gamepad setup
    void setControllerIndex (int idx) { gamepad.setControllerIndex (idx); }

    // Preset management (called from UI thread)
    void loadPreset (const std::string& presetId);
    const std::string& currentPresetId() const { return roleMap.currentPresetId(); }

    ButtonRoleMap& getRoleMap() { return roleMap; }

private:
    void timerCallback() override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Helper: map a ButtonId to the corresponding gamepad state bool
    static bool getButtonState (const GamepadState& gp, ButtonId btn);

    ChordEngine      chordEngine;
    StrumEngine      strumEngine;
    ButtonRoleMap    roleMap;
    ModulationRouter modulationRouter;
    XInputGamepad    gamepad;

#if JOYCHORD_HAS_SFIZZ
    std::unique_ptr<sfz::Sfizz> synth;
#endif

    // Active MIDI notes for proper note-off tracking
    std::vector<int> activeNotes;

    // Previous gamepad state for edge detection
    GamepadState prevGamepadState;

    // Gamepad state snapshot (written by timer thread, read by audio thread)
    GamepadState latestGamepadState;
    juce::SpinLock gamepadLock;

    // Runtime state modified by roles
    int  octaveOffset = 0;     // accumulated from RoleOctaveShift presses
    int  activeExtension = 0;  // 0=none, set by held modifier buttons

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JoychordProcessor)
};
