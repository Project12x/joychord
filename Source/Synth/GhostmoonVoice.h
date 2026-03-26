#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <ghostmoon/PolyBLEPOscillator.h>
#include <ghostmoon/UnisonEngine.h>
#include <ghostmoon/AHDSREnvelope.h>
#include <ghostmoon/SubOscillator.h>
#include <ghostmoon/Portamento.h>
#include <ghostmoon/DriftModulator.h>

// Full-featured synth voice using ghostmoon DSP library.
// Replaces SimpleVoice with anti-aliased oscillators, unison, envelope, sub, drift, portamento.

class GhostmoonVoice : public juce::SynthesiserVoice
{
public:
    bool canPlaySound (juce::SynthesiserSound* s) override { return s != nullptr; }

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        level = velocity * 0.35f;
        float freq = static_cast<float> (juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber));

        // Portamento: glide from current note to new note
        bool legato = envelope.isActive();
        portamento.setTarget (freq, legato);

        // SubOsc tracks main freq internally
        subOsc.reset();

        envelope.noteOn();
    }

    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
            envelope.noteOff();
        else
        {
            envelope.reset();
            clearCurrentNote();
        }
    }

    void renderNextBlock (juce::AudioBuffer<float>& output, int startSample, int numSamples) override
    {
        if (! envelope.isActive())
            return;

        for (int i = startSample; i < startSample + numSamples; ++i)
        {
            // Get current frequency (with portamento glide)
            double freq = portamento.process();

            // Apply drift to frequency
            float driftVal = drift.tick();
            double driftedFreq = freq * (1.0 + driftVal * 0.002); // max +-0.2% drift

            // Unison engine generates stereo output
            unison.setFrequency (driftedFreq);
            double oscL, oscR;
            unison.processStereo (oscL, oscR);

            // Sub oscillator
            float subSample = subOsc.process (static_cast<float> (driftedFreq),
                                               static_cast<float> (getSampleRate()));

            // Mix osc + sub
            float mixL = static_cast<float> (oscL) + subSample;
            float mixR = static_cast<float> (oscR) + subSample;

            // Normalize to prevent sub from boosting level
            float subLevel = subOsc.getLevel();
            float normFactor = 1.0f / (1.0f + subLevel);
            mixL *= normFactor;
            mixR *= normFactor;

            // Envelope
            float envVal = static_cast<float> (envelope.nextSample());
            float sampleL = mixL * level * envVal;
            float sampleR = mixR * level * envVal;

            // Output
            output.addSample (0, i, sampleL);
            if (output.getNumChannels() > 1)
                output.addSample (1, i, sampleR);
        }

        if (! envelope.isActive())
            clearCurrentNote();
    }

    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

    // === Parameter setters (called from processor before rendering) ===

    void prepareToPlay (double sampleRate)
    {
        unison.prepare (sampleRate);
        portamento.prepare (sampleRate);
        drift.prepare (sampleRate);
        drift.setEnabled (true);
        drift.setAlgorithm (gm::DriftAlgorithm::Drift);
        drift.setTargetType (gm::DriftTarget::Pitch);
        drift.setDepth (0.1f);
        drift.setRate (0.3f);
        drift.setAmount (0.5f);

        // Default envelope
        envelope.setParameters (0.01, 0.0, 0.1, 0.8, 0.3, sampleRate);
        sampleRate_ = sampleRate;
    }

    void setWaveshape (int shapeIdx)
    {
        auto shape = static_cast<gm::PolyBLEPOscillator::Shape> (
            std::clamp (shapeIdx, 0, static_cast<int> (gm::PolyBLEPOscillator::Shape::Count) - 1));
        unison.setWaveshape (shape);
    }

    void setUnisonCount (int count) { unison.setVoiceCount (count); }
    void setUnisonDetune (double cents) { unison.setDetune (cents); }
    void setUnisonSpread (double spread) { unison.setStereoSpread (spread); }

    void setSubOscLevel (float lvl) { subOsc.setLevel (lvl); }
    void setSubOscEnabled (bool enabled) { subOsc.setLevel (enabled ? subLevel_ : 0.0f); }
    void setSubLevel (float lvl) { subLevel_ = lvl; }

    void setEnvelope (double a, double h, double d, double s, double r)
    {
        envelope.setParameters (a, h, d, s, r, sampleRate_);
    }

    void setPortamentoTime (double seconds) { portamento.setTime (seconds); }
    void setPortamentoEnabled (bool enabled) { portamento.setTime (enabled ? portaTime_ : 0.0); }
    void setPortaTime (double sec) { portaTime_ = sec; }

    void setDriftAmount (float amt)
    {
        drift.setDepth (amt);
        drift.setAmount (amt > 0.001f ? 1.0f : 0.0f);
    }

private:
    gm::UnisonEngine unison;
    gm::AHDSREnvelope envelope;
    gm::SubOscillator subOsc;
    gm::Portamento portamento;
    gm::DriftModulator drift;

    float level = 0.0f;
    float subLevel_ = 0.5f;
    double portaTime_ = 0.08;
    double sampleRate_ = 44100.0;
};

// Reuse the same trivial sound
class GhostmoonSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};
