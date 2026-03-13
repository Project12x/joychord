#include "ChordEngine.h"

void ChordEngine::setKey    (int rootMidi)    { key     = rootMidi % 12; }
void ChordEngine::setScale  (ScaleMode mode)  { scale   = mode; }
void ChordEngine::setVoicing(VoicingStyle s)  { voicing = s; }
void ChordEngine::setOctave (int o)           { octave  = o; }

ChordResult ChordEngine::resolve (int degree, int extension, int inversion) const
{
    // TODO: implement using mahler.h interval math
    (void) degree; (void) extension; (void) inversion;
    return {};
}

ChordResult ChordEngine::resolveBorrowed (int degree, ScaleMode sourceScale,
                                           int extension, int inversion) const
{
    // TODO: resolve chord in sourceScale, then apply to current key
    (void) degree; (void) sourceScale; (void) extension; (void) inversion;
    return {};
}

ChordResult ChordEngine::resolveChromatic (int rootMidi, int quality) const
{
    // TODO: fixed chromatic chord regardless of key
    (void) rootMidi; (void) quality;
    return {};
}

std::vector<int> ChordEngine::scaleIntervals (ScaleMode mode) const
{
    // Semitone offsets of each scale degree from root
    switch (mode)
    {
        case ScaleMode::Major:      return {0, 2, 4, 5, 7, 9, 11};
        case ScaleMode::Minor:      return {0, 2, 3, 5, 7, 8, 10};
        case ScaleMode::Dorian:     return {0, 2, 3, 5, 7, 9, 10};
        case ScaleMode::Mixolydian: return {0, 2, 4, 5, 7, 9, 10};
        case ScaleMode::Phrygian:   return {0, 1, 3, 5, 7, 8, 10};
        case ScaleMode::Lydian:     return {0, 2, 4, 6, 7, 9, 11};
        case ScaleMode::WholeTone:  return {0, 2, 4, 6, 8, 10, 10};
        default:                    return {0, 2, 4, 5, 7, 9, 11};
    }
}

std::vector<int> ChordEngine::buildChord (int rootMidi, const std::vector<int>& intervals,
                                           int /*extension*/, int /*inversion*/) const
{
    // TODO: apply extension intervals, apply inversion, apply voicing style
    std::vector<int> notes;
    for (auto i : intervals)
        notes.push_back (rootMidi + i);
    return notes;
}

std::vector<int> ChordEngine::chordIntervals (int /*degree*/, ScaleMode /*mode*/) const
{
    // TODO: return triad intervals for degree in mode
    return {0, 4, 7}; // placeholder: major triad
}

std::string ChordEngine::chordName (int /*rootMidi*/, const std::vector<int>& /*intervals*/,
                                     int /*extension*/) const
{
    // TODO: compute human-readable chord name
    return "?";
}
