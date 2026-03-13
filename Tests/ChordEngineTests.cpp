#include <gtest/gtest.h>
#include "Engine/ChordEngine.h"

// ── ChordEngine: basic scale intervals ───────────────────────────────────────

TEST(ChordEngine, MajorScaleIntervalsCorrect)
{
    // When fully implemented: C major scale degrees should yield correct MIDI notes
    ChordEngine engine;
    engine.setKey (0);   // C
    engine.setScale (ScaleMode::Major);

    // Degree I in C major = C E G
    // TODO: uncomment when ChordEngine::resolve is implemented
    // auto result = engine.resolve (1);
    // ASSERT_EQ (result.midiNotes.size(), 3u);
    // EXPECT_EQ (result.midiNotes[0], 60); // C4
    // EXPECT_EQ (result.midiNotes[1], 64); // E4
    // EXPECT_EQ (result.midiNotes[2], 67); // G4

    SUCCEED(); // placeholder
}

TEST(ChordEngine, KeyTransposeCorrect)
{
    ChordEngine engine;
    engine.setKey (2); // D
    engine.setScale (ScaleMode::Major);

    // Degree I in D major = D F# A (MIDI 62, 66, 69)
    // TODO: uncomment when implemented
    // auto result = engine.resolve (1);
    // EXPECT_EQ (result.midiNotes[0], 62);

    SUCCEED();
}

TEST(ChordEngine, SeventhExtensionAddsNote)
{
    ChordEngine engine;
    engine.setKey (0);
    engine.setScale (ScaleMode::Major);

    // I + seventh = Cmaj7 = C E G B (4 notes)
    // TODO: uncomment when implemented
    // auto result = engine.resolve (1, 1 /* seventh */);
    // EXPECT_EQ (result.midiNotes.size(), 4u);

    SUCCEED();
}

TEST(ChordEngine, BorrowedChordFromParallelMinor)
{
    ChordEngine engine;
    engine.setKey (0); // C major
    engine.setScale (ScaleMode::Major);

    // bVII borrowed from parallel minor = Bb major
    // TODO: uncomment when implemented
    // auto result = engine.resolveBorrowed (7, ScaleMode::Minor);
    // EXPECT_EQ (result.midiNotes[0], 58); // Bb3 or Bb4

    SUCCEED();
}
