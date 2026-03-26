#include "StrumEngine.h"
#include <algorithm>

void StrumEngine::triggerNotes (const FixedArray<int, 16>& notes, float velocity, double sampleRate)
{
    // Strum from lowest to highest note.
    FixedArray<int, 16> sortedNotes = notes;
    std::sort(sortedNotes.begin(), sortedNotes.end());

    double delaySamplesPerNote = (strumSpeedMs / 1000.0) * sampleRate;
    double currentDelay = 0.0;

    for (int note : sortedNotes)
    {
        pendingNotes.push_back ({ note, velocity, currentDelay });
        currentDelay += delaySamplesPerNote;
    }
}

void StrumEngine::cancelNote (int noteNumber)
{
    // Remove any pending NoteOn events for this note
    for (int i = pendingNotes.size() - 1; i >= 0; --i) {
        if (pendingNotes[i].noteNumber == noteNumber) {
            pendingNotes.erase(pendingNotes.begin() + i);
        }
    }
}

void StrumEngine::cancelAllNotes()
{
    pendingNotes.clear();
}

void StrumEngine::process (juce::MidiBuffer& midiBuffer, int numSamples)
{
    for (int i = 0; i < pendingNotes.size(); )
    {
        if (pendingNotes[i].samplesUntilPlay < numSamples)
        {
            // Note is due to be played within this block.
            int pos = juce::jlimit (0, numSamples - 1, static_cast<int>(pendingNotes[i].samplesUntilPlay));
            midiBuffer.addEvent (juce::MidiMessage::noteOn (1, pendingNotes[i].noteNumber, pendingNotes[i].velocity), pos);

            pendingNotes.erase(pendingNotes.begin() + i);
        }
        else
        {
            pendingNotes[i].samplesUntilPlay -= numSamples;
            ++i;
        }
    }
}
