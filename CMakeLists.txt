cmake_minimum_required(VERSION 3.22)

# Enable modern MSVC runtime library management (must be set BEFORE project())
cmake_policy(SET CMP0091 NEW)

# Force all targets to use the dynamic MSVC CRT (/MD, /MDd) globally.
# This avoids LNK2038 CRT-mismatch errors between sfizz/abseil (/MT by default) and JUCE (/MD).
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" CACHE STRING "" FORCE)

project(Joychord VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
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
# sfizz Phase 5 integration. Disabled on MSVC Debug by default because abseil-cpp
# (a sfizz dependency) uses /MTd (static CRT) which conflicts with JUCE's /MDd.
# Re-enable when integrating sfizz in Phase 5; resolve the CRT mismatch then.
option(JOYCHORD_ENABLE_SFIZZ "Include sfizz SFZ synthesizer" ON)
if(JOYCHORD_ENABLE_SFIZZ)
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
    # sfizz 1.2.3 + abseil uses initializer list patterns that break under C++20.
    # Temporarily drop to C++17 for sfizz, then restore C++20.
    set(CMAKE_CXX_STANDARD 17)
    FetchContent_MakeAvailable(sfizz)
    set(CMAKE_CXX_STANDARD 20)
endif()

# ── spdlog ───────────────────────────────────────────────────────────────────
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.14.1
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(spdlog)

# ── mahler.c ─────────────────────────────────────────────────────────────────
# Western music theory library (C99, MIT). Used for chord naming and validation.
# Upstream CMake only sets src/ as PRIVATE include; we patch to PUBLIC so that
# inc/mahler.h can resolve its sub-includes (err/err.h, note/note.h, etc.).
FetchContent_Declare(
    mahler
    GIT_REPOSITORY https://github.com/thelowsunoverthemoon/mahler.c.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(mahler)
# Patch include directories: upstream only exposes src/ as PRIVATE
target_include_directories(mahler PUBLIC
    ${mahler_SOURCE_DIR}/inc
    ${mahler_SOURCE_DIR}/src
)

# ── GoogleTest ───────────────────────────────────────────────────────────────
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
    GIT_SHALLOW    TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ── ghostmoon DSP framework ──────────────────────────────────────────────────
# Provides gm:: DSP headers + transitive deps (xsimd, signalsmith, pffft, etc.)
set(GHOSTMOON_DIR "C:/Users/estee/Desktop/My Stuff/Code/Antigravity/ghostmoon")
add_subdirectory("${GHOSTMOON_DIR}/dsp" ghostmoon_dsp_build)

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

# Force dynamic CRT on all JUCE-generated plugin subtargets (Standalone, VST3, SharedCode).
# Needed because sfizz's abseil-cpp links /MTd by default, causing LNK2038 unless we
# explicitly request /MDd on every subtarget that links against sfizz.
if(MSVC)
    foreach(_target Joychord Joychord_Standalone Joychord_VST3)
        set_target_properties(${_target} PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
        )
    endforeach()
endif()

target_sources(Joychord PRIVATE
    Source/Plugin/PluginProcessor.cpp
    Source/Plugin/PluginEditor.cpp
    Source/Engine/ChordEngine.cpp
    Source/Engine/StrumEngine.cpp
    Source/Engine/ButtonRoleMap.cpp
    Source/Engine/ModulationRouter.cpp
    Source/Input/XInputGamepad.cpp
    Source/Synth/TinySoundFont.cpp
)

target_include_directories(Joychord PRIVATE
    Source
    vendor/tsf
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
    spdlog::spdlog
    mahler
    ghostmoon_dsp
    # XInput is a Windows system library
    $<$<PLATFORM_ID:Windows>:xinput>
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# Copy assets folder to Standalone output directory so the relative path lookup works
add_custom_command(TARGET Joychord_Standalone POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:Joychord_Standalone>/../assets"
    COMMENT "Copying assets folder to Standalone executable directory"
)

# sfizz's abseil-cpp insists on /MTd (static CRT) and cannot be overridden via CMake cache.
# Exclude the conflicting static CRT lib from the Windows link to allow JUCE's /MDd to win.
if(MSVC)
    foreach(_target Joychord Joychord_Standalone Joychord_VST3)
        if(TARGET ${_target})
            target_link_options(${_target} PRIVATE /NODEFAULTLIB:libcmtd /NODEFAULTLIB:libcmt)
        endif()
    endforeach()
endif()

# Link sfizz only if the target was actually created (skipped on MSVC to avoid CRT mismatch)
if(TARGET sfizz::sfizz)
    target_link_libraries(Joychord PRIVATE sfizz::sfizz)
    target_compile_definitions(Joychord PUBLIC JOYCHORD_HAS_SFIZZ=1)
endif()

# ── Tests ─────────────────────────────────────────────────────────────────────
enable_testing()

add_executable(JoychordTests
    Tests/ChordEngineTests.cpp
    Source/Engine/ChordEngine.cpp
)

target_include_directories(JoychordTests PRIVATE Source)

target_link_libraries(JoychordTests PRIVATE
    GTest::gtest_main
    spdlog::spdlog
    mahler
)

include(GoogleTest)
gtest_discover_tests(JoychordTests)

# --- HACK: Force MSVC Dynamic CRT globally on all subdirectories ---
function(force_dynamic_crt dir)
    get_property(targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
    foreach(target IN LISTS targets)
        get_target_property(target_type ${target} TYPE)
        if(NOT target_type STREQUAL "INTERFACE_LIBRARY")
            set_property(TARGET ${target} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
        endif()
    endforeach()

    get_property(subdirs DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir IN LISTS subdirs)
        force_dynamic_crt(${subdir})
    endforeach()
endfunction()

if(MSVC)
    force_dynamic_crt(${CMAKE_CURRENT_SOURCE_DIR})
endif()
