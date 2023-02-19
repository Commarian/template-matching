#include <stdio.h>
#include <debugapi.h>
#include "GPUCompute.cuh"
#include "_log.h"

namespace GPUCompute
{
	std::tuple<std::vector<_RGB>, std::vector<double>> Compute(std::vector<BYTE> pBuf, int ref_size, int game_running) {
		OutputDebugStringA("\nHello from GPUCOMPUTE\n");
		std::string str = "\nHello from GPUCOMPUTE\n";
		OutputDebugStringA(str.c_str());



		try {
			for (int i = 0; i < (ref_size - 4); i++) {
				unsigned char r = pBuf.at((i * 4) + 2);
				unsigned char g = pBuf.at((i * 4) + 1);
				unsigned char b = pBuf.at(i * 4);
				/*
				if (g > 85 && r < g/2 && b < g/2) {
					std::string temp = "{" + to_string(r) + "," +
						to_string(g) + "," + to_string(b) + "}";
					if (!(colors.find(temp) != std::string::npos)) {
						colors += temp + ",";
					};

				}*/


				//vector<_RGB> found_colors{};
				_RGB rgb{ NULL };
				if (game_running == 1) {
					for (_RGB rgbt : cs_colors) {
						if (r == rgbt.r && g == rgbt.g && b == rgbt.b) {
							rgb.r = r;
							rgb.g = g;
							rgb.b = b;
							int c = 0;
							for (_RGB vec_rgb : ref_vector) {
								if (r == vec_rgb.r && g == vec_rgb.g && b == vec_rgb.b) {
									c++;
									continue;
								}
							}
							if (c == 0) {
								ref_vector.push_back(rgb);
								position_vector.push_back(i);
							}
						}
					}
				}
				else {
					for (_RGB rgbt : dota_colors) {
						if (r == rgbt.r && g == rgbt.g && b == rgbt.b) {
							rgb.r = r;
							rgb.g = g;
							rgb.b = b;
							int c = 0;
							for (_RGB vec_rgb : ref_vector) {
								if (r == vec_rgb.r && g == vec_rgb.g && b == vec_rgb.b) {
									c++;
									continue;
								}
							}
							if (c == 0) {
								ref_vector.push_back(rgb);
								position_vector.push_back(i);
							}
						}
					}
				}
			}
		}
		catch (std::exception e) {
			std::string test = e.what();
			_log::log("\nERROR -SC");
			_log::log(test);
			_log::log("\nERROR -SC");
		}
		return std::make_tuple(ref_vector, position_vector);
	};

}