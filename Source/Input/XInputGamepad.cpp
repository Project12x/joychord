#include "XInputGamepad.h"

// XInput is Windows-only
#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #include <xinput.h>
#endif

XInputGamepad::XInputGamepad()  = default;
XInputGamepad::~XInputGamepad() = default;

bool XInputGamepad::poll (GamepadState& out)
{
#ifdef _WIN32
    XINPUT_STATE state{};
    DWORD result = XInputGetState (static_cast<DWORD>(controllerIndex), &state);

    connected = (result == ERROR_SUCCESS);
    out.connected = connected;
    if (! connected) return false;

    const auto& gp = state.Gamepad;
    const auto  btn = gp.wButtons;

    // Buttons
    out.btnA   = (btn & XINPUT_GAMEPAD_A)              != 0;
    out.btnB   = (btn & XINPUT_GAMEPAD_B)              != 0;
    out.btnX   = (btn & XINPUT_GAMEPAD_X)              != 0;
    out.btnY   = (btn & XINPUT_GAMEPAD_Y)              != 0;
    out.dUp    = (btn & XINPUT_GAMEPAD_DPAD_UP)        != 0;
    out.dDown  = (btn & XINPUT_GAMEPAD_DPAD_DOWN)      != 0;
    out.dLeft  = (btn & XINPUT_GAMEPAD_DPAD_LEFT)      != 0;
    out.dRight = (btn & XINPUT_GAMEPAD_DPAD_RIGHT)     != 0;
    out.lb     = (btn & XINPUT_GAMEPAD_LEFT_SHOULDER)  != 0;
    out.rb     = (btn & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
    out.l3     = (btn & XINPUT_GAMEPAD_LEFT_THUMB)     != 0;
    out.r3     = (btn & XINPUT_GAMEPAD_RIGHT_THUMB)    != 0;
    out.start  = (btn & XINPUT_GAMEPAD_START)          != 0;
    out.back   = (btn & XINPUT_GAMEPAD_BACK)           != 0;

    // Normalize thumbsticks from [-32768, 32767] to [-1, 1]
    auto normAxis = [](SHORT v) -> float {
        return v >= 0 ? static_cast<float>(v) / 32767.f
                      : static_cast<float>(v) / 32768.f;
    };

    out.lStickX = normAxis (gp.sThumbLX);
    out.lStickY = normAxis (gp.sThumbLY);
    out.rStickX = normAxis (gp.sThumbRX);
    out.rStickY = normAxis (gp.sThumbRY);

    // Normalize triggers [0, 255] to [0, 1]
    out.lt = static_cast<float>(gp.bLeftTrigger)  / 255.f;
    out.rt = static_cast<float>(gp.bRightTrigger) / 255.f;

    prevButtons = btn;
    return true;
#else
    out = {};
    return false;
#endif
}
