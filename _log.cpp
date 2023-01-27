#include "_log.h"
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <iostream>

namespace _log {
	void log(std::string s) 
	{
        //auto now = std::chrono::system_clock::now() + std::chrono::hours(2);
        //s = s + "\n" + std::format("{:%d-%m-%Y %H:%M:%OS}", now) + "\t";
        std::ofstream out;
        out.open("Full_DEBUG.log", std::ios_base::app);
        out << s;
        out.close();
        return;

	}
}