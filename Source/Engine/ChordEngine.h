#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <array>

// Scale intervals (semitones from root) for supported modes
enum class ScaleMode { Major, Minor, Dorian, Mixolydian, Phrygian, Lydian, WholeTone };

// Chord voicing styles
enum class VoicingStyle { Close, Drop2 };

struct ChordResult
{
    std::vector<int> midiNotes; // absolute MIDI note numbers
    std::string      name;      // e.g. "Cmaj7"
};

class ChordEngine
{
public:
    ChordEngine() = default;

    void setKey    (int rootMidi);       // 0=C, 1=C#, ..., 11=B
    void setScale  (ScaleMode mode);
    void setVoicing(VoicingStyle style);
    void setOctave (int octave);        // center octave, default 4

    // Resolve a scale degree (1-7) to a chord.
    // degree: 1-7 (diatonic) -- applies current key/scale
    // extension: 0=triad, 1=add7th, 2=sus4, 3=add9
    // inversion: 0=root, 1=first, 2=second, 3=third (7ths only)
    ChordResult resolve (int degree, int extension = 0, int inversion = 0) const;

    // Resolve a borrowed chord from a parallel scale
    ChordResult resolveBorrowed (int degree, ScaleMode sourceScale,
                                  int extension = 0, int inversion = 0) const;

    // Resolve a fixed chromatic chord (root = absolute MIDI note, quality = 0=maj,1=min,2=dim,3=aug,4=dom7)
    ChordResult resolveChromatic (int rootMidi, int quality) const;

    int       getKey()    const { return key; }
    ScaleMode getScale()  const { return scale; }

    // Dijkstra voicing scorer: update previous chord state
    void commitVoicing (const std::vector<int>& notes);

private:
    int          key     = 0;    // C
    ScaleMode    scale   = ScaleMode::Major;
    VoicingStyle voicing = VoicingStyle::Close;
    int          octave  = 4;

    // Previous chord notes for Dijkstra voicing scorer
    std::vector<int> previousNotes;

    // Build a chord from root + intervals, apply extension/inversion/voicing
    std::vector<int> buildChord (int rootMidi, const std::vector<int>& triadIntervals,
                                  int extension, int inversion) const;

    // Return semitone offsets for all 7 scale degrees of the given mode
    static std::vector<int> scaleIntervals (ScaleMode mode);

    // Return chord intervals (from root) for a given degree in a mode
    // e.g. degree 1 in Major = {0, 4, 7} (major triad)
    static std::vector<int> chordIntervalsForDegree (int degree, ScaleMode mode);

    // Compute human-readable chord name from root MIDI note + intervals
    static std::string chordName (int rootMidi, const std::vector<int>& intervals, int extension);

    // Score a voicing by total semitone distance from previousNotes (Dijkstra)
    int scoreVoicing (const std::vector<int>& candidate) const;

    // Pick best inversion using Dijkstra voicing scorer
    std::vector<int> bestVoicing (int rootMidi, const std::vector<int>& intervals,
                                    int extension) const;

    // Note name lookup
    static const char* noteName (int midiNote);
};
