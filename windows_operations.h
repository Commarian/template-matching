#pragma once
#include "bs_threading.h"

namespace WO
{
    static int open_game_code_global = 0;
    static int _sleeptimer = 15;
    //global state controller for the app. 
    //0 = in a game/searching, 1= match accepted, 2 = monitoring keys, 3 = playing game, 4 = not in a game
    inline int glob_state = 0;
    //should be monitoring keys
    inline bool m_keys = true;
    void MonitorWindows(BS::thread_pool &pool, bool &s_d);
    //in order = LMB,TAB,SHIFT,CTRL,SPACE,A,B,D,E,F,G,Q,R,S,W,F1
    const unsigned char csgo_buttons[16] = { 0x01,0x09,0x10,0x11,0x20,0x41,0x42,0x44,0x45,0x46,0x47,0x51,0x52,0x53,0x57,0x70 };
};