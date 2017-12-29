#pragma once

class Input
{
public:
    static float KeyValue(char key);
    static bool KeyDown(char key);
    static bool KeyPressed(char key);
    static void ClearKeyPress();

    static bool pressedKeys[0xff];

    static bool enabled;
};
