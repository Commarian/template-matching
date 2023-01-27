#pragma once
#include <Windows.h>
#include <tuple>
namespace INAT
{
	std::tuple<int, int> imaging(int ref_width, int ref_height, BYTE* pBuf);

}

