#pragma once
#include "GamepadInput.h"

class XInputGamepad : public GamepadInput
{
public:
    XInputGamepad();
    ~XInputGamepad() override;

    bool poll (GamepadState& outState) override;
    bool isConnected() const override { return connected; }

    // Controller index to use (0-3 for XInput)
    void setControllerIndex (int idx) { controllerIndex = idx; }

private:
    int  controllerIndex = 0;
    bool connected       = false;

    // Previous button state for edge detection
    unsigned short prevButtons = 0;
};
