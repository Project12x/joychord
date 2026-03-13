#pragma once
#include <vector>
#include <functional>
#include "ChordEngine.h"

// Callback types for MIDI events
using NoteOnCallback  = std::function<void(int midiNote, float velocity)>;
using NoteOffCallback = std::function<void(int midiNote)>;

class StrumEngine
{
public:
    StrumEngine() = default;

    void setCallbacks (NoteOnCallback onNote, NoteOffCallback offNote);

    // Called when a chord button is pressed (immediate sound)
    void chordPressed  (const ChordResult& chord);

    // Called when a chord button is released
    void chordReleased (const ChordResult& chord);

    // Called on strum-down (RT) or strum-up (LT) trigger
    // velocity in [0, 1] from analog trigger depth
    // direction: +1 = down (low-to-high), -1 = up (high-to-low)
    void strum (float velocity, int direction = 1);

    // Mute all active notes
    void mute();

    // Note spread between strum notes, in milliseconds
    void setStrumSpreadMs (float ms) { strumSpreadMs = ms; }

private:
    NoteOnCallback  noteOn;
    NoteOffCallback noteOff;

    ChordResult activeChord;
    ChordResult previousChord;

    float strumSpreadMs = 10.0f;

    // Computes common tones between two chords (for legato slide)
    std::vector<int> commonTones (const ChordResult& a, const ChordResult& b) const;
};
