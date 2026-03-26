#include "ButtonRoleMap.h"
#include <juce_data_structures/juce_data_structures.h>

ButtonRoleMap::ButtonRoleMap()
{
    loadDiatonicRock();
}

void ButtonRoleMap::setRole (ButtonId btn, ButtonRole role)
{
    roles[static_cast<int>(btn)] = std::move (role);
}

const ButtonRole& ButtonRoleMap::getRole (ButtonId btn) const
{
    auto it = roles.find (static_cast<int>(btn));
    return it != roles.end() ? it->second : fallback;
}

// ── Factory Presets ──────────────────────────────────────────────────────────

std::vector<PresetInfo> ButtonRoleMap::getFactoryPresets()
{
    return {
        { "diatonic_rock",  "Diatonic Rock" },
        { "pop_ballad",     "Pop Ballad" },
        { "jazz_voicings",  "Jazz Voicings" }
    };
}

void ButtonRoleMap::loadPreset (const std::string& presetId)
{
    roles.clear();
    activePresetId = presetId;

    if (presetId == "pop_ballad")        loadPopBallad();
    else if (presetId == "jazz_voicings") loadJazzVoicings();
    else                                  loadDiatonicRock();
}

void ButtonRoleMap::loadDiatonicRock()
{
    activePresetId = "diatonic_rock";
    // Face buttons: I, IV, V, vi
    setRole (ButtonId::A,      RoleChord       { 1 });
    setRole (ButtonId::X,      RoleChord       { 4 });
    setRole (ButtonId::B,      RoleChord       { 5 });
    setRole (ButtonId::Y,      RoleChord       { 6 });
    // D-pad: remaining degrees + bVII
    setRole (ButtonId::DUp,    RoleChord       { 2 });
    setRole (ButtonId::DRight, RoleChord       { 3 });
    setRole (ButtonId::DDown,  RoleChord       { 7 });
    setRole (ButtonId::DLeft,  RoleBorrowed    { 7, 1 }); // bVII from parallel minor
    // Modifiers
    setRole (ButtonId::LB,     RoleExtension   { 0 });    // seventh
    setRole (ButtonId::RB,     RoleExtension   { 1 });    // sus4
    // Octave shift
    setRole (ButtonId::L3,     RoleOctaveShift { -1 });
    setRole (ButtonId::R3,     RoleOctaveShift { +1 });
}

void ButtonRoleMap::loadPopBallad()
{
    activePresetId = "pop_ballad";
    // Pop progression focus: I, V, vi, IV (the "four chord" progression)
    setRole (ButtonId::A,      RoleChord       { 1 });    // I
    setRole (ButtonId::B,      RoleChord       { 5 });    // V
    setRole (ButtonId::Y,      RoleChord       { 6 });    // vi
    setRole (ButtonId::X,      RoleChord       { 4 });    // IV
    // D-pad: ii and common borrowed chords
    setRole (ButtonId::DUp,    RoleChord       { 2 });    // ii
    setRole (ButtonId::DRight, RoleBorrowed    { 4, 1 }); // iv (borrowed from minor)
    setRole (ButtonId::DDown,  RoleBorrowed    { 6, 1 }); // bVI from minor
    setRole (ButtonId::DLeft,  RoleBorrowed    { 7, 1 }); // bVII from minor
    // Modifiers: sus extensions for pop feel
    setRole (ButtonId::LB,     RoleExtension   { 2 });    // sus2
    setRole (ButtonId::RB,     RoleExtension   { 1 });    // sus4
    setRole (ButtonId::L3,     RoleOctaveShift { -1 });
    setRole (ButtonId::R3,     RoleOctaveShift { +1 });
}

void ButtonRoleMap::loadJazzVoicings()
{
    activePresetId = "jazz_voicings";
    // Jazz: ii-V-I focus, all with 7ths by default
    setRole (ButtonId::A,      RoleChord       { 1 });    // Imaj7
    setRole (ButtonId::B,      RoleChord       { 5 });    // V7
    setRole (ButtonId::X,      RoleChord       { 2 });    // ii7
    setRole (ButtonId::Y,      RoleChord       { 6 });    // vi7
    // D-pad: iii, IV, diminished, tritone sub
    setRole (ButtonId::DUp,    RoleChord       { 3 });    // iii
    setRole (ButtonId::DRight, RoleChord       { 4 });    // IV
    setRole (ButtonId::DDown,  RoleChord       { 7 });    // vii dim
    setRole (ButtonId::DLeft,  RoleBorrowed    { 2, 1 }); // ii from minor (dorian feel)
    // Modifiers: 7th is the default, LB adds 9th, RB adds sus4
    setRole (ButtonId::LB,     RoleExtension   { 0 });    // seventh (add on top)
    setRole (ButtonId::RB,     RoleExtension   { 3 });    // add9
    setRole (ButtonId::L3,     RoleOctaveShift { -1 });
    setRole (ButtonId::R3,     RoleOctaveShift { +1 });
}

// ── Serialization ────────────────────────────────────────────────────────────

juce::ValueTree ButtonRoleMap::toValueTree() const
{
    juce::ValueTree tree ("ROLEMAP");
    tree.setProperty ("preset", juce::String (activePresetId), nullptr);

    for (const auto& [btnInt, role] : roles)
    {
        juce::ValueTree node ("ROLE");
        node.setProperty ("button", juce::String (buttonIdToString (static_cast<ButtonId>(btnInt))), nullptr);
        node.setProperty ("type", juce::String (roleToString (role)), nullptr);

        // Serialize role-specific attributes
        std::visit ([&node] (auto&& r) {
            using T = std::decay_t<decltype(r)>;
            if constexpr (std::is_same_v<T, RoleChord>)
            {
                node.setProperty ("degree", r.degree, nullptr);
                node.setProperty ("quality", r.qualityOverride, nullptr);
            }
            else if constexpr (std::is_same_v<T, RoleBorrowed>)
            {
                node.setProperty ("degree", r.degree, nullptr);
                node.setProperty ("sourceScale", r.sourceScaleIdx, nullptr);
            }
            else if constexpr (std::is_same_v<T, RoleChromatic>)
            {
                node.setProperty ("rootMidi", r.rootMidi, nullptr);
                node.setProperty ("quality", r.quality, nullptr);
            }
            else if constexpr (std::is_same_v<T, RoleInversion>)
                node.setProperty ("n", r.n, nullptr);
            else if constexpr (std::is_same_v<T, RoleExtension>)
                node.setProperty ("extType", r.type, nullptr);
            else if constexpr (std::is_same_v<T, RoleOctaveShift>)
                node.setProperty ("delta", r.delta, nullptr);
            else if constexpr (std::is_same_v<T, RoleKeyTranspose>)
                node.setProperty ("semitones", r.semitones, nullptr);
            // StrumDown, StrumUp, Mute have no attributes
        }, role);

        tree.appendChild (node, nullptr);
    }

    return tree;
}

void ButtonRoleMap::fromValueTree (const juce::ValueTree& tree)
{
    if (! tree.hasType ("ROLEMAP"))
        return;

    // Check for preset-based restore
    auto presetProp = tree.getProperty ("preset", "").toString().toStdString();
    if (! presetProp.empty())
    {
        loadPreset (presetProp);
        return; // Factory preset overrides individual roles
    }

    roles.clear();
    for (int i = 0; i < tree.getNumChildren(); ++i)
    {
        auto node = tree.getChild (i);
        if (! node.hasType ("ROLE")) continue;

        auto btnStr  = node.getProperty ("button", "").toString().toStdString();
        auto typeStr = node.getProperty ("type", "").toString().toStdString();

        auto btn  = stringToButtonId (btnStr);
        auto role = stringToRole (typeStr, node);
        setRole (btn, role);
    }
}

// ── String Conversion ────────────────────────────────────────────────────────

std::string ButtonRoleMap::buttonIdToString (ButtonId id)
{
    switch (id)
    {
        case ButtonId::A:      return "BTN_A";
        case ButtonId::B:      return "BTN_B";
        case ButtonId::X:      return "BTN_X";
        case ButtonId::Y:      return "BTN_Y";
        case ButtonId::DUp:    return "BTN_DUP";
        case ButtonId::DDown:  return "BTN_DDOWN";
        case ButtonId::DLeft:  return "BTN_DLEFT";
        case ButtonId::DRight: return "BTN_DRIGHT";
        case ButtonId::LB:     return "BTN_LB";
        case ButtonId::RB:     return "BTN_RB";
        case ButtonId::LT:     return "BTN_LT";
        case ButtonId::RT:     return "BTN_RT";
        case ButtonId::L3:     return "BTN_L3";
        case ButtonId::R3:     return "BTN_R3";
        case ButtonId::Start:  return "BTN_START";
        case ButtonId::Back:   return "BTN_BACK";
        default:               return "BTN_UNKNOWN";
    }
}

ButtonId ButtonRoleMap::stringToButtonId (const std::string& s)
{
    if (s == "BTN_A")      return ButtonId::A;
    if (s == "BTN_B")      return ButtonId::B;
    if (s == "BTN_X")      return ButtonId::X;
    if (s == "BTN_Y")      return ButtonId::Y;
    if (s == "BTN_DUP")    return ButtonId::DUp;
    if (s == "BTN_DDOWN")  return ButtonId::DDown;
    if (s == "BTN_DLEFT")  return ButtonId::DLeft;
    if (s == "BTN_DRIGHT") return ButtonId::DRight;
    if (s == "BTN_LB")     return ButtonId::LB;
    if (s == "BTN_RB")     return ButtonId::RB;
    if (s == "BTN_LT")     return ButtonId::LT;
    if (s == "BTN_RT")     return ButtonId::RT;
    if (s == "BTN_L3")     return ButtonId::L3;
    if (s == "BTN_R3")     return ButtonId::R3;
    if (s == "BTN_START")  return ButtonId::Start;
    if (s == "BTN_BACK")   return ButtonId::Back;
    return ButtonId::A;
}

std::string ButtonRoleMap::roleToString (const ButtonRole& role)
{
    return std::visit ([] (auto&& r) -> std::string {
        using T = std::decay_t<decltype(r)>;
        if constexpr (std::is_same_v<T, RoleChord>)        return "Chord";
        if constexpr (std::is_same_v<T, RoleBorrowed>)     return "Borrowed";
        if constexpr (std::is_same_v<T, RoleChromatic>)    return "Chromatic";
        if constexpr (std::is_same_v<T, RoleInversion>)    return "Inversion";
        if constexpr (std::is_same_v<T, RoleExtension>)    return "Extension";
        if constexpr (std::is_same_v<T, RoleOctaveShift>)  return "OctaveShift";
        if constexpr (std::is_same_v<T, RoleKeyTranspose>) return "KeyTranspose";
        if constexpr (std::is_same_v<T, RoleStrumDown>)    return "StrumDown";
        if constexpr (std::is_same_v<T, RoleStrumUp>)      return "StrumUp";
        if constexpr (std::is_same_v<T, RoleMute>)         return "Mute";
        return "Unknown";
    }, role);
}

ButtonRole ButtonRoleMap::stringToRole (const std::string& type, const juce::ValueTree& node)
{
    if (type == "Chord")
        return RoleChord { static_cast<int>(node.getProperty("degree", 1)),
                           static_cast<int>(node.getProperty("quality", -1)) };
    if (type == "Borrowed")
        return RoleBorrowed { static_cast<int>(node.getProperty("degree", 1)),
                              static_cast<int>(node.getProperty("sourceScale", 1)) };
    if (type == "Chromatic")
        return RoleChromatic { static_cast<int>(node.getProperty("rootMidi", 60)),
                               static_cast<int>(node.getProperty("quality", 0)) };
    if (type == "Inversion")
        return RoleInversion { static_cast<int>(node.getProperty("n", 0)) };
    if (type == "Extension")
        return RoleExtension { static_cast<int>(node.getProperty("extType", 0)) };
    if (type == "OctaveShift")
        return RoleOctaveShift { static_cast<int>(node.getProperty("delta", 0)) };
    if (type == "KeyTranspose")
        return RoleKeyTranspose { static_cast<int>(node.getProperty("semitones", 0)) };
    if (type == "StrumDown") return RoleStrumDown{};
    if (type == "StrumUp")   return RoleStrumUp{};
    return RoleMute{};
}
