cmake_minimum_required(VERSION 3.22)
project(Joychord VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── JUCE ─────────────────────────────────────────────────────────────────────
include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

# ── sfizz ────────────────────────────────────────────────────────────────────
FetchContent_Declare(
    sfizz
    GIT_REPOSITORY https://github.com/sfztools/sfizz.git
    GIT_TAG        1.2.3
    GIT_SHALLOW    TRUE
)
set(SFIZZ_CLIENTS OFF CACHE BOOL "" FORCE)
set(SFIZZ_JACK    OFF CACHE BOOL "" FORCE)
set(SFIZZ_VST3   OFF CACHE BOOL "" FORCE)
set(SFIZZ_LV2    OFF CACHE BOOL "" FORCE)
set(SFIZZ_AU     OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(sfizz)

# ── spdlog ───────────────────────────────────────────────────────────────────
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.14.1
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(spdlog)

# ── GoogleTest ───────────────────────────────────────────────────────────────
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
    GIT_SHALLOW    TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ── Plugin target ─────────────────────────────────────────────────────────────
juce_add_plugin(Joychord
    VERSION                  ${PROJECT_VERSION}
    COMPANY_NAME             "Joychord"
    PLUGIN_MANUFACTURER_CODE Jcrd
    PLUGIN_CODE              Jcrd
    FORMATS                  VST3 Standalone
    PRODUCT_NAME             "Joychord"
    IS_MIDI_EFFECT           FALSE
    NEEDS_MIDI_INPUT         FALSE
    NEEDS_MIDI_OUTPUT        TRUE
    IS_SYNTH                 TRUE
)

target_sources(Joychord PRIVATE
    Source/Plugin/PluginProcessor.cpp
    Source/Plugin/PluginEditor.cpp
    Source/Engine/ChordEngine.cpp
    Source/Engine/StrumEngine.cpp
    Source/Engine/ButtonRoleMap.cpp
    Source/Engine/ModulationRouter.cpp
    Source/Input/XInputGamepad.cpp
)

target_include_directories(Joychord PRIVATE
    Source
    vendor
)

target_compile_definitions(Joychord PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(Joychord PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    juce::juce_gui_basics
    juce::juce_gui_extra
    sfizz::sfizz
    spdlog::spdlog
    # melatonin_blur -- re-added in UI phase with manual juce_add_module
    # XInput is a Windows system library
    $<$<PLATFORM_ID:Windows>:xinput>
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# ── Tests ─────────────────────────────────────────────────────────────────────
enable_testing()

add_executable(JoychordTests
    Tests/ChordEngineTests.cpp
    Source/Engine/ChordEngine.cpp
)

target_include_directories(JoychordTests PRIVATE Source vendor)

target_link_libraries(JoychordTests PRIVATE
    GTest::gtest_main
    spdlog::spdlog
)

include(GoogleTest)
gtest_discover_tests(JoychordTests)
