#include <windows.h>
#include "mouse_operations.h"
#include <thread>
#include <string>


namespace MO
{
    void ClickMouse()
    {
        try {
            std::string str = "\nClicking mouse";
            OutputDebugStringA(str.c_str());
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        }
        catch(std::exception e) {
            std::string str = e.what();
            OutputDebugStringA(str.c_str());
        }

    }

    void MoveCursorTo(int x, int y, bool save)
    {
        try {
            std::string str = "\nMoving mouse to = " + std::to_string(x) + ", " + std::to_string(y);
            OutputDebugStringA(str.c_str());
            if (save == true) {
                GetCursorPos(&old_pos);
            }
            SetCursorPos(x, y);
        }
        catch (std::exception e) {
            std::string str = e.what();
            OutputDebugStringA(str.c_str());
        }
    }

    void ReturnCursor() 
    {
        GetCursorPos(&old_pos);
        try {
            //std::this_thread::sleep_for(std::chrono::milliseconds(750));
            std::string str = "\nReturning to = " + std::to_string(old_pos.x) + ", " + std::to_string(old_pos.y);
            OutputDebugStringA(str.c_str());
            SetCursorPos(old_pos.x, old_pos.y);
        }
        catch (std::exception e) {
            std::string str = e.what();
            OutputDebugStringA(str.c_str());
        }

    }

}
