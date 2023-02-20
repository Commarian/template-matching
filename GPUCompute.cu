#include "GPUCompute.cuh"

__global__ void vectorAdd(int* a, int* b, int* c) 
{
	int i = threadIdx.x;
	c[i] = a[i] + b[i];
	return;
}



std::tuple<std::vector<std::tuple<unsigned char, unsigned char, unsigned char>>, std::vector<double>> GPUCompute::Compute(std::vector<unsigned char> pBuf, int ref_size, int game_running) {
	//vector for our reference with only RGB not Alpha channel included
	std::vector<std::tuple<unsigned char, unsigned char, unsigned char>> ref_vector{};

	std::vector<double> position_vector{};

	/*OutputDebugStringA("\nHello from GPUCOMPUTE\n");
	std::string str = "\nHello from GPUCOMPUTE\n";
	OutputDebugStringA(str.c_str());*/

	int a[] = { 1,2,3 };
	int b[] = { 4,5,6 };
	int c[sizeof(a) / sizeof(int)] = { 0 };

	//pointers to the gpu
	int* cudaA = 0;
	int* cudaB = 0;
	int* cudaC = 0;

	//allocate memory in the GPU
	cudaMalloc(&cudaA, sizeof(a));
	cudaMalloc(&cudaB, sizeof(b));
	cudaMalloc(&cudaC, sizeof(c));

	//copy the vectors into the GPU
	cudaMemcpy(cudaA, a, sizeof(a), cudaMemcpyHostToDevice);
	cudaMemcpy(cudaB, b, sizeof(b), cudaMemcpyHostToDevice);
		
	//vectorAdd<<<GRID_SIZE, BLOCK_SIZE = number of vectors>>>
	vectorAdd <<< 1, sizeof(a) / sizeof(int) >>> (cudaA, cudaB, cudaC);
	cudaMemcpy(c, cudaC, sizeof(c), cudaMemcpyDeviceToHost);

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


			//vector<GPUCompute::_RGB> found_colors{};
			std::tuple<unsigned char, unsigned char, unsigned char> rgb;
			if (game_running == 1) {
				for (std::tuple<unsigned char, unsigned char, unsigned char> rgbt : GPUCompute::cs_colors) {
					if (r == std::get<0>(rgbt) && g == std::get<1>(rgbt) && b == std::get<2>(rgbt)) {
						rgb = std::make_tuple(r, g, b);
						int c = 0;
						for (std::tuple<unsigned char, unsigned char, unsigned char> vec_rgb : ref_vector) {
							if (r == std::get<0>(vec_rgb) && g == std::get<1>(vec_rgb) && b == std::get<2>(vec_rgb)) {
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
				for (std::tuple<unsigned char, unsigned char, unsigned char> rgbt : GPUCompute::dota_colors) {
					if (r == std::get<0>(rgbt) && g == std::get<1>(rgbt) && b == std::get<2>(rgbt)) {
						rgb = std::make_tuple(r, g, b);
						int c = 0;
						for (std::tuple<unsigned char, unsigned char, unsigned char> vec_rgb : ref_vector) {
							if (r == std::get<0>(vec_rgb) && g == std::get<1>(vec_rgb) && b == std::get<2>(vec_rgb)) {
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