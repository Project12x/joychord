#pragma once
#include "Engine/ButtonRoleMap.h"

// Normalized gamepad state snapshot
struct GamepadState
{
    // Buttons (true = pressed this poll)
    bool btnA = false, btnB = false, btnX = false, btnY = false;
    bool dUp = false, dDown = false, dLeft = false, dRight = false;
    bool lb = false, rb = false;
    bool l3 = false, r3 = false;
    bool start = false, back = false;

    // Analog axes [-1, 1]
    float lStickX = 0.f, lStickY = 0.f;
    float rStickX = 0.f, rStickY = 0.f;

    // Triggers [0, 1]
    float lt = 0.f, rt = 0.f;

    bool connected = false;
};

// Abstract gamepad interface — XInput implementation now, SDL2 later
class GamepadInput
{
public:
    virtual ~GamepadInput() = default;
    virtual bool       poll (GamepadState& outState) = 0; // returns true if connected
    virtual bool       isConnected() const = 0;
};
