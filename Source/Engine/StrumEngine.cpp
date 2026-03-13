#include "StrumEngine.h"
#include <algorithm>

void StrumEngine::setCallbacks (NoteOnCallback onNote, NoteOffCallback offNote)
{
    noteOn  = std::move (onNote);
    noteOff = std::move (offNote);
}

void StrumEngine::chordPressed (const ChordResult& chord)
{
    // Legato: only re-attack notes that aren't common with the previous chord
    auto common = commonTones (activeChord, chord);

    for (auto n : activeChord.midiNotes)
    {
        bool isCommon = std::find (common.begin(), common.end(), n) != common.end();
        if (! isCommon && noteOff)
            noteOff (n);
    }

    for (auto n : chord.midiNotes)
    {
        bool isCommon = std::find (common.begin(), common.end(), n) != common.end();
        if (! isCommon && noteOn)
            noteOn (n, 0.8f); // TODO: use default velocity param
    }

    previousChord = activeChord;
    activeChord   = chord;
}

void StrumEngine::chordReleased (const ChordResult& chord)
{
    if (noteOff)
        for (auto n : chord.midiNotes)
            noteOff (n);

    if (activeChord.midiNotes == chord.midiNotes)
        activeChord = {};
}

void StrumEngine::strum (float velocity, int direction)
{
    // Re-articulate all notes of activeChord with timed spread
    // TODO: schedule note-on events with strumSpreadMs delay per note
    auto notes = activeChord.midiNotes;
    if (direction < 0)
        std::reverse (notes.begin(), notes.end());

    if (noteOff)
        for (auto n : notes) noteOff (n);

    if (noteOn)
        for (auto n : notes) noteOn (n, velocity); // TODO: apply strumSpreadMs stagger
}

void StrumEngine::mute()
{
    if (noteOff)
        for (auto n : activeChord.midiNotes) noteOff (n);
    activeChord = {};
}

std::vector<int> StrumEngine::commonTones (const ChordResult& a, const ChordResult& b) const
{
    std::vector<int> common;
    for (auto n : a.midiNotes)
        if (std::find (b.midiNotes.begin(), b.midiNotes.end(), n) != b.midiNotes.end())
            common.push_back (n);
    return common;
}
