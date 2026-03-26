#include "ChordEngine.h"

#include <algorithm>
#include <cmath>
#include <numeric>

// mahler.c for chord naming (C99 library)
// The restrict keyword is valid C99 but not C++; suppress it for MSVC.
#ifdef __cplusplus
  #define restrict __restrict
#endif
extern "C" {
#include "mahler.h"
}
#ifdef __cplusplus
  #undef restrict
#endif

// ── Setters ──────────────────────────────────────────────────────────────────

void ChordEngine::setKey    (int rootMidi)    { key     = rootMidi % 12; }
void ChordEngine::setScale  (ScaleMode mode)  { scale   = mode; }
void ChordEngine::setVoicing(VoicingStyle s)  { voicing = s; }
void ChordEngine::setOctave (int o)           { octave  = o; }

// ── Note name lookup ─────────────────────────────────────────────────────────

const char* ChordEngine::noteName (int midiNote)
{
    static const char* names[] = {"C","C#","D","Eb","E","F","F#","G","Ab","A","Bb","B"};
    return names[((midiNote % 12) + 12) % 12];
}

// ── Scale intervals ──────────────────────────────────────────────────────────

FixedArray<int, 12> ChordEngine::scaleIntervals (ScaleMode mode)
{
    // Semitone offsets of each scale degree (1-7) from root
    switch (mode)
    {
        case ScaleMode::Major:      return {0, 2, 4, 5, 7, 9, 11};
        case ScaleMode::Minor:      return {0, 2, 3, 5, 7, 8, 10};
        case ScaleMode::Dorian:     return {0, 2, 3, 5, 7, 9, 10};
        case ScaleMode::Mixolydian: return {0, 2, 4, 5, 7, 9, 10};
        case ScaleMode::Phrygian:   return {0, 1, 3, 5, 7, 8, 10};
        case ScaleMode::Lydian:     return {0, 2, 4, 6, 7, 9, 11};
        case ScaleMode::WholeTone:  return {0, 2, 4, 6, 8, 10};
        default:                    return {0, 2, 4, 5, 7, 9, 11};
    }
}

// ── Chord intervals per degree ───────────────────────────────────────────────

FixedArray<int, 12> ChordEngine::chordIntervalsForDegree (int degree, ScaleMode mode)
{
    auto si = scaleIntervals(mode);
    int numDegrees = static_cast<int>(si.size());
    if (numDegrees == 0) return {0, 4, 7}; // fallback

    // Clamp degree to valid range
    int d = ((degree - 1) % numDegrees + numDegrees) % numDegrees;

    // Build triad by stacking diatonic thirds: root, 3rd degree up, 5th degree up
    int root  = si[d];
    int third = si[(d + 2) % numDegrees];
    int fifth = si[(d + 4) % numDegrees];

    // Handle wraparound (interval relative to root)
    auto interval = [root](int note, int chromatic = 12) {
        return ((note - root) % chromatic + chromatic) % chromatic;
    };

    return {0, interval(third), interval(fifth)};
}

// ── Build chord ──────────────────────────────────────────────────────────────

FixedArray<int, 12> ChordEngine::buildChord (int rootMidi, const FixedArray<int, 12>& triadIntervals,
                                            int extension, int inversion) const
{
    FixedArray<int, 12> intervals = triadIntervals;

    // Apply extensions
    switch (extension)
    {
        case 1: // 7th -- add diatonic 7th interval
        {
            // Determine 7th quality: major 7th (11 semitones) for I, IV; minor 7th (10) otherwise
            // Simple heuristic based on the triad quality
            int thirdInterval = intervals.size() >= 2 ? intervals[1] : 4;
            int fifthInterval = intervals.size() >= 3 ? intervals[2] : 7;

            if (thirdInterval == 4 && fifthInterval == 7)
                intervals.push_back(11); // major triad -> major 7th
            else if (thirdInterval == 4 && fifthInterval == 7)
                intervals.push_back(10); // won't reach here, but for completeness
            else
                intervals.push_back(10); // minor/dim triad -> minor 7th
            break;
        }
        case 2: // sus4 -- replace 3rd with 4th
            if (intervals.size() >= 2)
                intervals[1] = 5;
            break;
        case 3: // sus2 -- replace 3rd with 2nd
            if (intervals.size() >= 2)
                intervals[1] = 2;
            break;
        case 4: // add9 -- add 9th (14 semitones = octave + major 2nd)
            intervals.push_back(14);
            break;
        default:
            break;
    }

    // Build absolute MIDI notes
    FixedArray<int, 12> notes;
    for (auto i : intervals)
    {
        int note = rootMidi + i;
        if (note >= 0 && note <= 127)
            notes.push_back(note);
    }

    // Apply inversion: move bottom note(s) up by octave
    int inv = std::min(inversion, static_cast<int>(notes.size()) - 1);
    for (int i = 0; i < inv; ++i)
    {
        if (!notes.empty())
        {
            int lowest = notes.front();
            notes.erase(notes.begin());
            int raised = lowest + 12;
            if (raised <= 127)
                notes.push_back(raised);
        }
    }

    // Apply drop-2 voicing: move 2nd-from-top note down an octave
    if (voicing == VoicingStyle::Drop2 && notes.size() >= 3)
    {
        std::sort(notes.begin(), notes.end());
        size_t idx = notes.size() - 2; // 2nd from top
        int dropped = notes[idx] - 12;
        if (dropped >= 0)
        {
            notes[idx] = dropped;
            std::sort(notes.begin(), notes.end());
        }
    }

    // Range clamp: keep all notes within a playable range [36, 84] (C2 - C6)
    // Transpose the entire chord by octaves to fit within bounds.
    if (!notes.empty())
    {
        std::sort(notes.begin(), notes.end());

        // If highest note is too high, bring the whole chord down
        while (notes.back() > 84 && notes.front() > 36)
        {
            for (auto& n : notes)
                n -= 12;
        }

        // If lowest note is too low, bring the whole chord up
        while (notes.front() < 36 && notes.back() < 84)
        {
            for (auto& n : notes)
                n += 12;
        }

        // Final safety: remove any notes still outside 0-127
        int j = 0;
        for (int i = 0; i < notes.size(); ++i) {
            if (notes[i] >= 0 && notes[i] <= 127) {
                notes[j++] = notes[i];
            }
        }
        notes.count = j;
    }

    return notes;
}

// ── Chord naming ─────────────────────────────────────────────────────────────

FixedString<32> ChordEngine::chordName (int rootMidi, const FixedArray<int, 12>& intervals, int extension)
{
    FixedString<32> name(noteName(rootMidi));

    if (intervals.size() < 2)
        return name;

    int third = intervals[1];
    int fifth = intervals.size() >= 3 ? intervals[2] : 7;

    // Determine triad quality
    if (third == 3 && fifth == 6)
        name += "dim";
    else if (third == 4 && fifth == 8)
        name += "aug";
    else if (third == 3)
        name += "m";
    // else major (no suffix)

    // Extension suffix
    switch (extension)
    {
        case 1:
        {
            bool hasSeventh = intervals.size() >= 4;
            if (hasSeventh)
            {
                int seventh = intervals[3];
                if (seventh == 11)
                    name += "maj7";
                else if (seventh == 10 && third == 4)
                    name += "7"; // dominant
                else if (seventh == 10)
                    name += "7";
                else if (seventh == 9)
                    name += "dim7";
            }
            break;
        }
        case 2: name += "sus4"; break;
        case 3: name += "sus2"; break;
        case 4: name += "add9"; break;
        default: break;
    }

    return name;
}

// ── Dijkstra voicing scorer ──────────────────────────────────────────────────

void ChordEngine::commitVoicing (const FixedArray<int, 12>& notes)
{
    previousNotes = notes;
}

int ChordEngine::scoreVoicing (const FixedArray<int, 12>& candidate, int rootMidi) const
{
    if (previousNotes.empty())
        return 0; // no previous chord, all voicings equally good

    // Sum of minimum distances from each candidate note to nearest previous note
    int totalDistance = 0;
    for (int cn : candidate)
    {
        int minDist = 127;
        for (int pn : previousNotes)
            minDist = std::min(minDist, std::abs(cn - pn));
        totalDistance += minDist;
    }

    // Octave gravity penalty
    // Calculate the center of the candidate chord
    if (!candidate.empty())
    {
        float center = 0;
        for (int cn : candidate)
            center += cn;
        center /= candidate.size();

        // Calculate the ideal "home" center based on the current base octave and key
        float homeCenter = (octave + 1) * 12 + key + 7.0f; // Roughly the middle of the home octave

        // Add a penalty based on how far the chord has drifted from home
        float drift = std::abs (center - homeCenter);
        
        // Only penalize if we drift more than half an octave away
        if (drift > 6.0f)
        {
            // Apply an escalating penalty the further we drift
            totalDistance += static_cast<int>((drift - 6.0f) * 2.0f);
        }
    }

    return totalDistance;
}

FixedArray<int, 12> ChordEngine::bestVoicing (int rootMidi, const FixedArray<int, 12>& intervals,
                                             int extension) const
{
    if (previousNotes.empty())
    {
        // No previous chord -- use root position
        return buildChord(rootMidi, intervals, extension, 0);
    }

    // Try all inversions, pick the one with lowest total movement
    int maxInv = static_cast<int>(intervals.size()) + (extension == 1 ? 1 : 0);
    maxInv = std::min(maxInv, 3); // cap at 3rd inversion

    int bestScore = std::numeric_limits<int>::max();
    FixedArray<int, 12> best;

    for (int inv = 0; inv < maxInv; ++inv)
    {
        auto candidate = buildChord(rootMidi, intervals, extension, inv);
        int s = scoreVoicing(candidate, rootMidi);
        if (s < bestScore)
        {
            bestScore = s;
            best = candidate;
        }
    }

    return best;
}

// ── Public resolve methods ───────────────────────────────────────────────────

ChordResult ChordEngine::resolve (int degree, int extension, int inversion) const
{
    auto si = scaleIntervals(scale);
    int numDegrees = static_cast<int>(si.size());
    if (numDegrees == 0)
        return {};

    // Scale degree to root MIDI note
    // Standard MIDI: C4 = 60 = 12 * (4 + 1). MIDI note 0 = C-1.
    int d = ((degree - 1) % numDegrees + numDegrees) % numDegrees;
    int rootMidi = (key + si[d]) + ((octave + 1) * 12);

    // Get chord intervals for this degree
    auto intervals = chordIntervalsForDegree(degree, scale);

    FixedArray<int, 12> notes;
    if (inversion >= 0)
    {
        // Explicit inversion requested
        notes = buildChord(rootMidi, intervals, extension, inversion);
    }
    else
    {
        // Auto-select best voicing (Dijkstra)
        notes = bestVoicing(rootMidi, intervals, extension);
    }

    // Build interval set including extensions for naming
    auto fullIntervals = intervals;
    if (extension == 1) {
        int thirdInterval = intervals.size() >= 2 ? intervals[1] : 4;
        if (thirdInterval == 4)
            fullIntervals.push_back(11); // major triad -> major 7th
        else
            fullIntervals.push_back(10); // minor/dim -> minor 7th
    }

    return { notes, chordName(rootMidi % 12, fullIntervals, extension) };
}

ChordResult ChordEngine::resolveBorrowed (int degree, ScaleMode sourceScale,
                                            int extension, int inversion) const
{
    auto si = scaleIntervals(sourceScale);
    int numDegrees = static_cast<int>(si.size());
    if (numDegrees == 0)
        return {};

    // Root in the source scale
    // Standard MIDI: C4 = 60 = 12 * (4 + 1).
    int d = ((degree - 1) % numDegrees + numDegrees) % numDegrees;
    int rootMidi = (key + si[d]) + ((octave + 1) * 12);

    // Get chord intervals from the source scale
    auto intervals = chordIntervalsForDegree(degree, sourceScale);

    FixedArray<int, 12> notes;
    if (inversion >= 0)
        notes = buildChord(rootMidi, intervals, extension, inversion);
    else
        notes = bestVoicing(rootMidi, intervals, extension);

    auto fullIntervals = intervals;
    if (extension == 1) {
        int thirdInterval = intervals.size() >= 2 ? intervals[1] : 4;
        if (thirdInterval == 4)
            fullIntervals.push_back(11);
        else
            fullIntervals.push_back(10);
    }

    return { notes, chordName(rootMidi % 12, fullIntervals, extension) };
}

ChordResult ChordEngine::resolveChromatic (int rootMidi, int quality) const
{
    // Fixed chord regardless of key
    FixedArray<int, 12> intervals;

    switch (quality)
    {
        case 0: intervals = {0, 4, 7};      break; // major
        case 1: intervals = {0, 3, 7};      break; // minor
        case 2: intervals = {0, 3, 6};      break; // diminished
        case 3: intervals = {0, 4, 8};      break; // augmented
        case 4: intervals = {0, 4, 7, 10};  break; // dominant 7th
        default: intervals = {0, 4, 7};     break;
    }

    auto notes = buildChord(rootMidi, intervals, 0, 0);
    return { notes, chordName(rootMidi % 12, intervals, quality == 4 ? 1 : 0) };
}
