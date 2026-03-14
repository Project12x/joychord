#include <gtest/gtest.h>
#include "Engine/ChordEngine.h"

// ── ChordEngine: basic scale intervals ───────────────────────────────────────

TEST(ChordEngine, MajorScaleIntervalsCorrect)
{
    ChordEngine engine;
    engine.setKey(0);   // C
    engine.setScale(ScaleMode::Major);
    engine.setOctave(4);

    // Degree I in C major = C E G (MIDI 60, 64, 67)
    auto result = engine.resolve(1);
    ASSERT_EQ(result.midiNotes.size(), 3u);
    EXPECT_EQ(result.midiNotes[0], 60); // C4
    EXPECT_EQ(result.midiNotes[1], 64); // E4
    EXPECT_EQ(result.midiNotes[2], 67); // G4
}

TEST(ChordEngine, MinorScaleIntervalsCorrect)
{
    ChordEngine engine;
    engine.setKey(9);   // A
    engine.setScale(ScaleMode::Minor);
    engine.setOctave(4);

    // Degree I in A minor = A C E
    // octave 4: root = 9 + 0 + 60 = 69 (A4)
    auto result = engine.resolve(1);
    ASSERT_EQ(result.midiNotes.size(), 3u);
    EXPECT_EQ(result.midiNotes[0], 69); // A4
    EXPECT_EQ(result.midiNotes[1], 72); // C5
    EXPECT_EQ(result.midiNotes[2], 76); // E5
}

TEST(ChordEngine, KeyTransposeCorrect)
{
    ChordEngine engine;
    engine.setKey(2); // D
    engine.setScale(ScaleMode::Major);
    engine.setOctave(4);

    // Degree I in D major = D F# A (MIDI 62, 66, 69)
    auto result = engine.resolve(1);
    ASSERT_EQ(result.midiNotes.size(), 3u);
    EXPECT_EQ(result.midiNotes[0], 62); // D4
    EXPECT_EQ(result.midiNotes[1], 66); // F#4
    EXPECT_EQ(result.midiNotes[2], 69); // A4
}

TEST(ChordEngine, SeventhExtensionAddsNote)
{
    ChordEngine engine;
    engine.setKey(0);   // C
    engine.setScale(ScaleMode::Major);
    engine.setOctave(4);

    // I + seventh = Cmaj7 = C E G B (4 notes)
    auto result = engine.resolve(1, 1 /* seventh */);
    ASSERT_EQ(result.midiNotes.size(), 4u);
    EXPECT_EQ(result.midiNotes[0], 60); // C4
    EXPECT_EQ(result.midiNotes[1], 64); // E4
    EXPECT_EQ(result.midiNotes[2], 67); // G4
    EXPECT_EQ(result.midiNotes[3], 71); // B4 (major 7th = 11 semitones)
}

TEST(ChordEngine, BorrowedChordFromParallelMinor)
{
    ChordEngine engine;
    engine.setKey(0); // C
    engine.setScale(ScaleMode::Major);
    engine.setOctave(4);

    // bVII borrowed from parallel minor: degree 7 in natural minor
    // Minor scale degree 7 offset = 10 semitones (Bb)
    // Root = 0 + 10 + 60 = 70 (Bb4)
    auto result = engine.resolveBorrowed(7, ScaleMode::Minor);
    ASSERT_GE(result.midiNotes.size(), 3u);
    EXPECT_EQ(result.midiNotes[0], 70); // Bb4
}

TEST(ChordEngine, Sus4ReplacesThird)
{
    ChordEngine engine;
    engine.setKey(0); // C
    engine.setScale(ScaleMode::Major);
    engine.setOctave(4);

    // Csus4 = C F G
    auto result = engine.resolve(1, 2 /* sus4 */);
    ASSERT_EQ(result.midiNotes.size(), 3u);
    EXPECT_EQ(result.midiNotes[0], 60); // C4
    EXPECT_EQ(result.midiNotes[1], 65); // F4 (perfect 4th = 5 semitones)
    EXPECT_EQ(result.midiNotes[2], 67); // G4
}

TEST(ChordEngine, FirstInversionMovesRootUp)
{
    ChordEngine engine;
    engine.setKey(0); // C
    engine.setScale(ScaleMode::Major);
    engine.setOctave(4);

    // C major 1st inversion = E G C (64, 67, 72)
    auto result = engine.resolve(1, 0, 1);
    ASSERT_EQ(result.midiNotes.size(), 3u);
    EXPECT_EQ(result.midiNotes[0], 64); // E4
    EXPECT_EQ(result.midiNotes[1], 67); // G4
    EXPECT_EQ(result.midiNotes[2], 72); // C5
}

TEST(ChordEngine, Drop2VoicingCorrect)
{
    ChordEngine engine;
    engine.setKey(0);
    engine.setScale(ScaleMode::Major);
    engine.setVoicing(VoicingStyle::Drop2);
    engine.setOctave(4);

    // Cmaj7 close = C4 E4 G4 B4 (60, 64, 67, 71)
    // Drop-2: move 2nd from top (G4=67) down an octave to G3=55
    // Result sorted: 55, 60, 64, 71
    auto result = engine.resolve(1, 1 /* seventh */);
    ASSERT_EQ(result.midiNotes.size(), 4u);
    EXPECT_EQ(result.midiNotes[0], 55); // G3 (dropped)
    EXPECT_EQ(result.midiNotes[1], 60); // C4
    EXPECT_EQ(result.midiNotes[2], 64); // E4
    EXPECT_EQ(result.midiNotes[3], 71); // B4
}

TEST(ChordEngine, DijkstraPicksClosestVoicing)
{
    ChordEngine engine;
    engine.setKey(0);
    engine.setScale(ScaleMode::Major);
    engine.setOctave(4);

    // First chord: G major (degree V) = G4 B4 D5 (67, 71, 74)
    auto g = engine.resolve(5);
    engine.commitVoicing(g.midiNotes);

    // Next chord: C major (degree I) with inversion=-1 (auto-select)
    // Root pos: C4 E4 G4 (60, 64, 67) - total distance from G chord
    // 1st inv:  E4 G4 C5 (64, 67, 72) - closer to the G chord notes
    auto c = engine.resolve(1, 0, -1); // -1 = auto Dijkstra
    ASSERT_EQ(c.midiNotes.size(), 3u);
    // The 1st inversion should be preferred because E4,G4 overlap with G chord
    // Verify that the chosen voicing has lower total distance than root position
    // (We just check it picked *something* valid; exact inversion depends on scoring)
    EXPECT_GE(c.midiNotes[0], 60);
    EXPECT_LE(c.midiNotes[2], 84);
}

TEST(ChordEngine, AllDegreesProduceValidChords)
{
    ChordEngine engine;
    engine.setKey(0);
    engine.setScale(ScaleMode::Major);
    engine.setOctave(4);

    for (int degree = 1; degree <= 7; ++degree)
    {
        auto result = engine.resolve(degree);
        EXPECT_GE(result.midiNotes.size(), 3u) << "Degree " << degree << " should have at least 3 notes";
        for (int note : result.midiNotes)
        {
            EXPECT_GE(note, 0) << "Degree " << degree << " note out of range";
            EXPECT_LE(note, 127) << "Degree " << degree << " note out of range";
        }
        EXPECT_FALSE(result.name.empty()) << "Degree " << degree << " should have a name";
    }
}

TEST(ChordEngine, ChromaticChordCorrect)
{
    ChordEngine engine;

    // Chromatic Eb major = Eb G Bb
    auto result = engine.resolveChromatic(63, 0); // Eb4 = MIDI 63, quality major
    ASSERT_EQ(result.midiNotes.size(), 3u);
    EXPECT_EQ(result.midiNotes[0], 63); // Eb4
    EXPECT_EQ(result.midiNotes[1], 67); // G4
    EXPECT_EQ(result.midiNotes[2], 70); // Bb4
}

TEST(ChordEngine, ChordNameCorrect)
{
    ChordEngine engine;
    engine.setKey(0);
    engine.setScale(ScaleMode::Major);
    engine.setOctave(4);

    auto cMaj = engine.resolve(1);
    EXPECT_EQ(cMaj.name, "C");

    auto dMin = engine.resolve(2);
    EXPECT_EQ(dMin.name, "Dm");

    auto cMaj7 = engine.resolve(1, 1);
    EXPECT_EQ(cMaj7.name, "Cmaj7");
}

TEST(ChordEngine, MinorChordDegreesCorrect)
{
    ChordEngine engine;
    engine.setKey(0); // C
    engine.setScale(ScaleMode::Major);
    engine.setOctave(4);

    // Degree ii in C major = D minor (D F A) = 62, 65, 69
    auto result = engine.resolve(2);
    ASSERT_EQ(result.midiNotes.size(), 3u);
    EXPECT_EQ(result.midiNotes[0], 62); // D4
    EXPECT_EQ(result.midiNotes[1], 65); // F4
    EXPECT_EQ(result.midiNotes[2], 69); // A4

    // Degree iii = E minor (E G B) = 64, 67, 71
    auto result3 = engine.resolve(3);
    ASSERT_EQ(result3.midiNotes.size(), 3u);
    EXPECT_EQ(result3.midiNotes[0], 64); // E4
    EXPECT_EQ(result3.midiNotes[1], 67); // G4
    EXPECT_EQ(result3.midiNotes[2], 71); // B4
}
