#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <array>

// Scale intervals (semitones from root) for supported modes
enum class ScaleMode { Major, Minor, Dorian, Mixolydian, Phrygian, Lydian, WholeTone };

// Chord voicing styles
enum class VoicingStyle { Close, Drop2 };

template <size_t N>
struct FixedString {
    std::array<char, N> buffer{};
    FixedString() { buffer[0] = '\0'; }
    FixedString(const char* str) { append(str); }
    void append(const char* str) {
        size_t len = 0;
        while (len < N && buffer[len] != '\0') len++;
        while (len < N - 1 && *str != '\0') buffer[len++] = *str++;
        buffer[len] = '\0';
    }
    FixedString& operator+=(const char* str) { append(str); return *this; }
    const char* c_str() const { return buffer.data(); }
    bool empty() const { return buffer[0] == '\0'; }

    bool operator==(const char* other) const {
        return std::strncmp(buffer.data(), other, N) == 0;
    }
    bool operator!=(const char* other) const {
        return !(*this == other);
    }
};

template <typename T, size_t Cap>
struct FixedArray {
    std::array<T, Cap> data{};
    int count = 0;
    void push_back(const T& v) { if (count < Cap) data[count++] = v; }
    void clear() { count = 0; }
    bool empty() const { return count == 0; }
    int size() const { return count; }
    T* begin() { return data.data(); }
    T* end() { return data.data() + count; }
    const T* begin() const { return data.data(); }
    const T* end() const { return data.data() + count; }
    T& front() { return data[0]; }
    const T& front() const { return data[0]; }
    T& back() { return data[count - 1]; }
    const T& back() const { return data[count - 1]; }
    T* erase(T* it) {
        int idx = static_cast<int>(it - data.data());
        for (int i = idx; i < count - 1; ++i) data[i] = data[i+1];
        count--;
        return it;
    }
    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
    // allow initializer list
    FixedArray() = default;
    FixedArray(std::initializer_list<T> list) {
        for (const auto& v : list) push_back(v);
    }
};

struct ChordResult
{
    FixedArray<int, 12> midiNotes; // absolute MIDI note numbers
    FixedString<32> name;      // e.g. "Cmaj7"
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
    void commitVoicing (const FixedArray<int, 12>& notes);

private:
    int          key     = 0;    // C
    ScaleMode    scale   = ScaleMode::Major;
    VoicingStyle voicing = VoicingStyle::Close;
    int          octave  = 4;

    // Previous chord notes for Dijkstra voicing scorer
    FixedArray<int, 12> previousNotes;

    // Build a chord from root + intervals, apply extension/inversion/voicing
    FixedArray<int, 12> buildChord (int rootMidi, const FixedArray<int, 12>& triadIntervals,
                                  int extension, int inversion) const;

    // Return semitone offsets for all 7 scale degrees of the given mode
    static FixedArray<int, 12> scaleIntervals (ScaleMode mode);

    // Return chord intervals (from root) for a given degree in a mode
    // e.g. degree 1 in Major = {0, 4, 7} (major triad)
    static FixedArray<int, 12> chordIntervalsForDegree (int degree, ScaleMode mode);

    // Compute human-readable chord name from root MIDI note + intervals
    static FixedString<32> chordName (int rootMidi, const FixedArray<int, 12>& intervals, int extension);

    // Score a voicing by total semitone distance from previousNotes (Dijkstra)
    int scoreVoicing (const FixedArray<int, 12>& candidate, int rootMidi) const;

    // Pick best inversion using Dijkstra voicing scorer
    FixedArray<int, 12> bestVoicing (int rootMidi, const FixedArray<int, 12>& intervals,
                                    int extension) const;

    // Note name lookup
    static const char* noteName (int midiNote);
};
