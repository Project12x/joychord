#pragma once
#include <juce_audio_basics/juce_audio_basics.h>

// Minimal sine-wave voice with ADSR. Temporary for Phase 2.5 testing.
// Will be replaced by sfizz SFZ player in Phase 6.

class SimpleVoice : public juce::SynthesiserVoice
{
public:
    bool canPlaySound (juce::SynthesiserSound* s) override { return s != nullptr; }

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        level = velocity * 0.25f; // keep it tame
        auto freq = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        angleDelta = freq / getSampleRate() * juce::MathConstants<double>::twoPi;
        adsr.noteOn();
    }

    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
            adsr.noteOff();
        else
            clearCurrentNote();
    }

    void renderNextBlock (juce::AudioBuffer<float>& output, int startSample, int numSamples) override
    {
        if (! adsr.isActive())
            return;

        for (int i = startSample; i < startSample + numSamples; ++i)
        {
            auto sample = static_cast<float> (std::sin (currentAngle)) * level * adsr.getNextSample();
            for (int ch = 0; ch < output.getNumChannels(); ++ch)
                output.addSample (ch, i, sample);

            currentAngle += angleDelta;
            if (currentAngle > juce::MathConstants<double>::twoPi)
                currentAngle -= juce::MathConstants<double>::twoPi;
        }

        if (! adsr.isActive())
            clearCurrentNote();
    }

    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

    void setADSR (float a, float d, float s, float r)
    {
        adsrParams.attack  = a;
        adsrParams.decay   = d;
        adsrParams.sustain = s;
        adsrParams.release = r;
    }

    void prepareToPlay (double sampleRate)
    {
        adsr.setSampleRate (sampleRate);
        // Quick attack, generous sustain, smooth release
        adsrParams = { 0.01f, 0.1f, 0.8f, 0.3f };
        adsr.setParameters (adsrParams);
    }

private:
    double currentAngle = 0.0;
    double angleDelta   = 0.0;
    float  level        = 0.0f;
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;
};

// Trivial sound that accepts all voices
class SimpleSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};
