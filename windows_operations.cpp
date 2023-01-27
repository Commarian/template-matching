#include "windows_operations.h"
#include <string>
#include <thread>
#include <chrono>
#include <Windows.h>
#include "wi.h"
#include "mouse_operations.h"
#include "_log.h"
#include "image_native.h"
#include "ScreenCapture.h"

static std::tuple<HWND, int> game_tuple = std::make_tuple(nullptr, 0);



void MonitorKeys() {
    int w_pressed = 0;
    while (WO::m_keys) {
        //nice for each loop
        //for (unsigned char i : WO::csgo_buttons) {
        if (GetKeyState('W') < 0) {
            w_pressed++;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
        
    }
    _log::log("W_PRESSED = ");
    _log::log(std::to_string(w_pressed));
}
void TimerMinutes(int minutes) {
    std::this_thread::sleep_for(std::chrono::minutes(minutes));
    WO::m_keys = false;
}

void WO::MonitorWindows(BS::thread_pool& pool, bool &s_d)
{
    //ini some variables
    std::tuple<int, int> return_location;
    const std::tuple<int, int> no_location = std::make_tuple(0, 0);

    
    //check if we are still supposed to be running
    while (!s_d) {
        game_tuple = WI::whichGameRunning();
        //get window and game type
        HWND window = std::get<0>(game_tuple);
        int game_running = std::get<1>(game_tuple);



        std::future<void> future_3 = pool.submit(MO::ReturnCursor);
        future_3.wait();

        game_running = 0;
        

        //A game is running
        if (game_running != 0) {
            //CSGO is running
            if (game_running == 1)
            {
                _sleeptimer = 2;
                //std::this_thread::sleep_for(std::chrono::seconds(4));
                //Match the template with the screenshot based on which game running
                //return_location = SS::ScreenCapture(game_running);                   
            }
            //Dota 2 is running
            else if (game_running == 2)
            {   
                _sleeptimer = 4;
                //std::this_thread::sleep_for(std::chrono::seconds(4));
                //Match the template with the screenshot based on which game running
                return_location = SS::ScreenCapture(game_running);
            }
            //didn't find a match
            else 
            {
                return_location = no_location;
            }
            //found a match
            if (return_location != no_location)
            {
                //enables key monitoring
                //m_keys = true;
                //glob_state = 2;
                //std::future<void> future_x = pool.submit(MonitorKeys);
                //std::future<void> future_z = pool.submit(TimerMinutes, 6);
                int x = std::get<0>(return_location);
                int y = std::get<1>(return_location);
                std::future<void> future_0 = pool.submit(MO::MoveCursorTo, x, y, true);
                future_0.wait();
                //std::future<void> future_1 = pool.submit(MO::ClickMouse);
                //future_1.wait();
                if (game_running == 1) {
                    std::future<void> future_2 = pool.submit(MO::MoveCursorTo, x+2, y+1, false);
                    future_2.wait();
                    //std::future<void> future_4 = pool.submit(MO::ClickMouse);
                    //future_4.wait();
                }
                //std::future<void> future_3 = pool.submit(MO::ReturnCursor);
                //future_3.wait();
            }

        }

        else { 
            glob_state = 4; _sleeptimer = 1; 
        }

        std::this_thread::sleep_for(std::chrono::seconds(_sleeptimer));
    }
    OutputDebugStringA("RETURNING");
    return;
}
