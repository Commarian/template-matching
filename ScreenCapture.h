#pragma once
#include <windows.h>
#include <tuple>
#include <format>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <iostream>
#include "GPUCompute.cuh"
namespace SS 
{
	std::tuple<int, int> ScreenCapture(int game_running);

	}