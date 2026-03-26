#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <ghostmoon/SafetyLimiter.h>
#include <ghostmoon/CpuMeter.h>
#include <ghostmoon/MeterSource.h>
#include <ghostmoon/PlateReverb.h>
#include <ghostmoon/MoogLadder.h>
#include <ghostmoon/Chorus.h>
#include <ghostmoon/ParamSmoother.h>
#include <ghostmoon/Compressor.h>
#include <ghostmoon/Flanger.h>
#include <ghostmoon/Phaser.h>
#include <ghostmoon/PingPongDelay.h>
#include <ghostmoon/ShimmerReverb.h>
#include <ghostmoon/Dither.h>
#include "Engine/ChordEngine.h"
#include "Engine/StrumEngine.h"
#include "Engine/ButtonRoleMap.h"
#include "Synth/GhostmoonVoice.h"
#include "Engine/ModulationRouter.h"
#include "Input/XInputGamepad.h"
#include <set>

#if JOYCHORD_HAS_SFIZZ
#include <sfizz.hpp>
#endif

// Forward declare TinySoundFont
struct tsf;

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

    void loadCustomSfz (const juce::String& path);

    ButtonRoleMap& getRoleMap() { return roleMap; }

    // Ghostmoon DSP accessors for UI
    float getCpuLoad() const { return cpuMeter.getLoadPercent(); }
    gm::MeterSource& getMeterSource() { return meterSource; }
    ModulationRouter& getModulationRouter() { return modulationRouter; }

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

    // Synth Engines
    juce::Synthesiser analogSynth;
#if JOYCHORD_HAS_SFIZZ
    std::unique_ptr<sfz::Sfizz> sfzSynth;
    juce::String currentSfzPath;
#endif

    // TinySoundFont for .sf2
    tsf* tsfSynth = nullptr;
    std::vector<float> tsfInterleavedBuffer;

    // Active MIDI notes for proper note-off tracking
    std::set<int> activeNotes;

    // Tracker for dynamic polyphony gain (Equal Power: 1 / sqrt(N))
    float currentPolyGain = 1.0f;

    // Ghostmoon DSP
    gm::SafetyLimiter safetyLimiter;
    gm::CpuMeter      cpuMeter;
    gm::MeterSource    meterSource;

    // Effects chain
    gm::PlateReverb    reverb;
    gm::MoogLadder     filterL, filterR;
    gm::Chorus         chorus;
    gm::Compressor     compressor;
    gm::Flanger        flanger;
    gm::Phaser         phaser;
    gm::PingPongDelay  pingPongDelay;
    gm::ShimmerReverb  shimmerReverb;
    gm::Dither         ditherL, ditherR;

    // Parameter smoothers for RT-safe control
    gm::ParamSmoother<float> smoothReverbDecay{0.02f};
    gm::ParamSmoother<float> smoothReverbDamp{0.02f};
    gm::ParamSmoother<float> smoothReverbMix{0.02f};
    gm::ParamSmoother<float> smoothFilterCutoff{0.02f};
    gm::ParamSmoother<float> smoothFilterRes{0.02f};
    gm::ParamSmoother<float> smoothChorusRate{0.02f};
    gm::ParamSmoother<float> smoothChorusMix{0.02f};
    gm::ParamSmoother<float> smoothCompThresh{0.02f};
    gm::ParamSmoother<float> smoothCompRatio{0.02f};
    gm::ParamSmoother<float> smoothFlangerRate{0.02f};
    gm::ParamSmoother<float> smoothFlangerMix{0.02f};
    gm::ParamSmoother<float> smoothPhaserRate{0.02f};
    gm::ParamSmoother<float> smoothPhaserMix{0.02f};
    gm::ParamSmoother<float> smoothDelayTime{0.02f};
    gm::ParamSmoother<float> smoothDelayFb{0.02f};
    gm::ParamSmoother<float> smoothDelayMix{0.02f};
    gm::ParamSmoother<float> smoothShimmerDecay{0.02f};
    gm::ParamSmoother<float> smoothShimmerAmt{0.02f};
    gm::ParamSmoother<float> smoothShimmerMix{0.02f};

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
