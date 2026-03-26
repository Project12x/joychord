#pragma once
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "ChordEngine.h"

struct PendingNote {
    int noteNumber;
    float velocity;
    double samplesUntilPlay; // Countdown timer in samples
};

class StrumEngine
{
public:
    StrumEngine() = default;

    // Set the speed of the strum in milliseconds (delay between each consecutive note)
    void setStrumSpeedMs (float ms) { strumSpeedMs = std::max(0.0f, ms); }

    // Enqueue a set of notes to be strummed
    void triggerNotes (const FixedArray<int, 16>& notes, float velocity, double sampleRate);

    // Cancel a specific note if it's in the pending queue (e.g. if released before it played)
    void cancelNote (int noteNumber);

    // Clear all pending notes
    void cancelAllNotes();

    // Process the queue, advancing time by numSamples and adding due NoteOns into the midiBuffer
    void process (juce::MidiBuffer& midiBuffer, int numSamples);

private:
    float strumSpeedMs = 0.0f;
    FixedArray<PendingNote, 64> pendingNotes;
};
