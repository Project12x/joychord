#include "ButtonRoleMap.h"
#include <juce_data_structures/juce_data_structures.h>

ButtonRoleMap::ButtonRoleMap()
{
    loadDefaultDiatonicRock();
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

void ButtonRoleMap::loadDefaultDiatonicRock()
{
    setRole (ButtonId::A,      RoleChord       { 1 });
    setRole (ButtonId::X,      RoleChord       { 4 });
    setRole (ButtonId::B,      RoleChord       { 5 });
    setRole (ButtonId::Y,      RoleChord       { 6 });
    setRole (ButtonId::DUp,    RoleChord       { 2 });
    setRole (ButtonId::DRight, RoleChord       { 3 });
    setRole (ButtonId::DDown,  RoleChord       { 7 });
    setRole (ButtonId::DLeft,  RoleBorrowed    { 7, 1 }); // bVII from parallel minor
    setRole (ButtonId::LB,     RoleExtension   { 0 });    // seventh
    setRole (ButtonId::RB,     RoleExtension   { 1 });    // sus4
    setRole (ButtonId::RT,     RoleStrumDown   {});
    setRole (ButtonId::LT,     RoleStrumUp     {});
    setRole (ButtonId::L3,     RoleOctaveShift { -1 });
    setRole (ButtonId::R3,     RoleOctaveShift { +1 });
}

juce::ValueTree ButtonRoleMap::toValueTree() const
{
    juce::ValueTree tree ("ROLEMAP");
    // TODO: iterate roles, serialize each as a child ValueTree node
    return tree;
}

void ButtonRoleMap::fromValueTree (const juce::ValueTree& tree)
{
    // TODO: parse ROLEMAP children back to ButtonRole variants
    juce::ignoreUnused (tree);
}

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

ButtonRole ButtonRoleMap::stringToRole (const std::string& type, const juce::ValueTree& /*node*/)
{
    // TODO: full deserialization — reads attributes from node
    if (type == "StrumDown") return RoleStrumDown{};
    if (type == "StrumUp")   return RoleStrumUp{};
    return RoleMute{}; // safe fallback
}
