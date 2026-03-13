#pragma once
#include <variant>
#include <unordered_map>
#include <string>

namespace juce { class ValueTree; class String; }

// ── Role Variants ─────────────────────────────────────────────────────────────

struct RoleChord       { int degree; int qualityOverride = -1; }; // -1 = follow scale
struct RoleBorrowed    { int degree; int sourceScaleIdx; };
struct RoleChromatic   { int rootMidi; int quality; };
struct RoleInversion   { int n; };
struct RoleExtension   { int type; }; // 0=seventh, 1=sus4, 2=sus2, 3=add9
struct RoleOctaveShift { int delta; };
struct RoleKeyTranspose{ int semitones; };
struct RoleStrumDown   {};
struct RoleStrumUp     {};
struct RoleMute        {};

using ButtonRole = std::variant<
    RoleChord, RoleBorrowed, RoleChromatic,
    RoleInversion, RoleExtension,
    RoleOctaveShift, RoleKeyTranspose,
    RoleStrumDown, RoleStrumUp, RoleMute
>;

// ── Button IDs ────────────────────────────────────────────────────────────────
enum class ButtonId
{
    A, B, X, Y,
    DUp, DDown, DLeft, DRight,
    LB, RB,
    LT, RT,
    L3, R3,
    Start, Back
};

// ── ButtonRoleMap ─────────────────────────────────────────────────────────────
class ButtonRoleMap
{
public:
    ButtonRoleMap();

    void             setRole (ButtonId btn, ButtonRole role);
    const ButtonRole& getRole (ButtonId btn) const;

    // Serialization — implemented in .cpp which includes JUCE
    juce::ValueTree toValueTree()   const;
    void fromValueTree (const juce::ValueTree& tree);

    // Load a named factory preset
    void loadDefaultDiatonicRock();

private:
    std::unordered_map<int, ButtonRole> roles;
    ButtonRole fallback = RoleMute{};

    static std::string buttonIdToString (ButtonId id);
    static ButtonId    stringToButtonId (const std::string& s);
    static std::string roleToString     (const ButtonRole& role);
    static ButtonRole  stringToRole     (const std::string& type, const juce::ValueTree& node);
};
