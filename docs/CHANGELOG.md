# Changelog

All notable changes to Joychord will be documented here.

Format: [Keep a Changelog](https://keepachangelog.com/en/1.0.0/)  
Versioning: [Semantic Versioning](https://semver.org/)

---

## [Unreleased]

### Added
- Initial architecture design and feature specification
- Standard project documentation (README, STATE, ARCHITECTURE, ROADMAP, HOWTO, CHANGELOG)
- Phase 1 project scaffold: all stub source files for ChordEngine, StrumEngine, ButtonRoleMap, ModulationRouter, GamepadInput, XInputGamepad, PluginProcessor, PluginEditor
- CMakeLists.txt: JUCE 8 FetchContent, spdlog, GoogleTest; sfizz guarded behind `if(TARGET sfizz::sfizz)` for Phase 5
- mahler.c integrated as FetchContent dependency (C99, MIT) -- replaces `vendor/mahler.h` placeholder
- ChordEngine fully implemented: diatonic resolution (I-vii), extensions (7th, sus4, sus2, add9), inversions, drop-2 voicing, Dijkstra voicing scorer, borrowed chords, chromatic chords, chord naming
- 13 ChordEngine tests passing (scale intervals, key transpose, seventh, sus4, inversion, drop-2, Dijkstra, naming, borrowed, chromatic, all-degrees validation)
- `Joychord_Standalone.exe` and `JoychordTests.exe` build cleanly on Windows (VS 2022 Debug)
- Library research: evaluated 8 candidate libraries (mahler.c, Teoria.js, optimal-voice-leading, ChordSeqAI, music-gen, MusicTheoryCpp, NotofxMusicTheory, libremidi)
- Updated ROADMAP, ARCHITECTURE with mahler.c integration plan, Dijkstra voicing scorer, chord name display, and Research References
