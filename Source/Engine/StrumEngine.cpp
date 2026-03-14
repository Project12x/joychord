#include "StrumEngine.h"
#include <algorithm>

void StrumEngine::triggerNotes (const std::vector<int>& notes, float velocity, double sampleRate)
{
    // Strum from lowest to highest note.
    std::vector<int> sortedNotes = notes;
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
    pendingNotes.erase(
        std::remove_if(pendingNotes.begin(), pendingNotes.end(),
            [noteNumber](const PendingNote& n) { return n.noteNumber == noteNumber; }),
        pendingNotes.end()
    );
}

void StrumEngine::cancelAllNotes()
{
    pendingNotes.clear();
}

void StrumEngine::process (juce::MidiBuffer& midiBuffer, int numSamples)
{
    for (auto it = pendingNotes.begin(); it != pendingNotes.end(); )
    {
        if (it->samplesUntilPlay < numSamples)
        {
            // Note is due to be played within this block.
            int pos = juce::jlimit (0, numSamples - 1, static_cast<int>(it->samplesUntilPlay));
            midiBuffer.addEvent (juce::MidiMessage::noteOn (1, it->noteNumber, it->velocity), pos);

            it = pendingNotes.erase(it);
        }
        else
        {
            it->samplesUntilPlay -= numSamples;
            ++it;
        }
    }
}
