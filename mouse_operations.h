#pragma once
#include <windows.h>
namespace MO
{
    static POINT old_pos;
    void ClickMouse();
    void MoveCursorTo(int x, int y, bool save);
    void ReturnCursor();
}

