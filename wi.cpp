#include "wi.h"
#include <string>


namespace WI
{
    std::tuple<HWND, int> whichGameRunning()
    {
        //Window handle of the desktop
        HWND desktopWindow = GetDesktopWindow();

        //Get the top-most window in the desktop
        HWND window = GetTopWindow(desktopWindow);
        //Get the window with the lowest z-index, with regards to the top-most window
        do
        {
            //Not visible? skip
            if (!IsWindowVisible(window))
                continue;
            //Get the title of the window
            std::string csgoString = "Counter-Strike: Global Offensive - Direct3D 9";
            std::string dotaString = "Dota 2";
            int len = GetWindowTextLength(window) + 1;
            std::string titleString;
            titleString.reserve(static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char>>::size_type>(len) + 1);
            GetWindowTextA(window, const_cast<char*>(titleString.c_str()), len);
            if (strcmp(titleString.c_str(), csgoString.c_str()) == 0) {
                OutputDebugStringA("\nFOUND CSGO\n");
                return std::make_tuple(window, 1);
            }
            if (strcmp(titleString.c_str(), dotaString.c_str()) == 0) {
                OutputDebugStringA("\nFOUND DOTA\n");
                return std::make_tuple(window, 2);
            }

        } while (window = GetWindow(window, GW_HWNDNEXT));
        return std::make_tuple(nullptr, 0);
    }
}