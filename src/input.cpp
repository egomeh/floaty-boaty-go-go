#include "input.hpp"

#include <Windows.h>

bool Input::enabled = false;
bool Input::pressedKeys[0xff];

float Input::KeyValue(char key)
{
    if (!Input::enabled)
    {
        return 0.f;;
    }

    if (KeyDown(key))
    {
        return 1.f;
    }

    return 0.f;
}

bool Input::KeyDown(char key)
{
    if (!Input::enabled)
    {
        return false;
    }

    SHORT value = VkKeyScan(key);
    if (GetAsyncKeyState(value) != 0)
    {
        return true;
    }

    return false;
}

bool Input::KeyPressed(char key)
{
    if (!Input::enabled)
    {
        return false;
    }

    SHORT value = VkKeyScan(key);
    return Input::pressedKeys[value % 0xff];
}

void Input::ClearKeyPress()
{
    for (int i = 0; i < 0xff; ++i)
    {
        Input::pressedKeys[i] = false;
    }
}
