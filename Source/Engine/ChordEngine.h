#pragma once
#include <vector>
#include <cstdint>
#include <string>

// Scale intervals (semitones from root) for supported modes
enum class ScaleMode { Major, Minor, Dorian, Mixolydian, Phrygian, Lydian, WholeTone };

// Chord voicing styles
enum class VoicingStyle { Close, Drop2 };

struct ChordResult
{
    std::vector<int> midiNotes; // absolute MIDI note numbers
    std::string      name;      // e.g. "C maj7"
};

class ChordEngine
{
public:
    ChordEngine() = default;

    void setKey   (int rootMidi);       // 0=C, 1=C#, ..., 11=B
    void setScale (ScaleMode mode);
    void setVoicing (VoicingStyle style);
    void setOctave  (int octave);        // center octave, default 4

    // Resolve a scale degree (1-7) to a chord.
    // degree: 1-7 (diatonic) — applies current key/scale
    // extension: 0=triad, 1=add7th, 2=sus4, 3=add9
    // inversion: 0=root, 1=first, 2=second
    ChordResult resolve (int degree, int extension = 0, int inversion = 0) const;

    // Resolve a borrowed chord from a parallel scale
    ChordResult resolveBorrowed (int degree, ScaleMode sourceScale,
                                  int extension = 0, int inversion = 0) const;

    // Resolve a fixed chromatic chord (root = absolute MIDI note, quality = 0=maj,1=min,2=dom7...)
    ChordResult resolveChromatic (int rootMidi, int quality) const;

    int  getKey()   const { return key; }
    ScaleMode getScale() const { return scale; }

private:
    int         key     = 0;    // C
    ScaleMode   scale   = ScaleMode::Major;
    VoicingStyle voicing = VoicingStyle::Close;
    int         octave  = 4;

    std::vector<int> buildChord (int rootMidi, const std::vector<int>& intervals,
                                  int extension, int inversion) const;
    std::vector<int> scaleIntervals (ScaleMode mode) const;
    std::vector<int> chordIntervals  (int degree, ScaleMode mode) const;
    std::string      chordName       (int rootMidi, const std::vector<int>& intervals,
                                      int extension) const;
};
