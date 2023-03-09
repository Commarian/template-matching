#include "GPUCompute.cuh"



// CUDA kernel. Each thread takes care of one element of c
__global__ void vecAdd(unsigned char* template_vector, unsigned char* sshot_vector, int* match_count, int n)
{
    // Get our global thread ID
    int id = blockIdx.x * blockDim.x + threadIdx.x;


    // Make sure we do not go out of bounds
    if (id < n) 
    {
        if (template_vector[id] - sshot_vector[id] > 250 ||
            template_vector[id] - sshot_vector[id] < 6)
        {
            //TODO CONCURRENCY IS FAILING ME HARD HERE> need to figure
            //++match_count[id];
        }
    }
}



int GPUCompute::Compute(std::vector<  unsigned char> pBuf, int ref_size, int game_running) {
	//vector for our reference with only RGB not Alpha channel included
	std::vector<std::tuple<  char,   char,   char>> ref_vector{ {1,2,3} };

	std::vector<double> position_vector{5,5,5,5,5};

    // Size of vectors
    int pixels_times_rgb = sizeof(dota_colors) ;

    // Host input vectors
    unsigned char* templ_array_host = nullptr;
    unsigned char* sshot_array_host = nullptr;
    //Host output vector
    int* match_count_host = nullptr;

    // Device input vectors
    unsigned char* templ_array_gpu = nullptr;
    unsigned char* sshot_array_gpu = nullptr;
    //Device output vector
    int* match_count_gpu = nullptr;

    // Allocate memory for each vector on host
    templ_array_host = (unsigned char*)malloc(pixels_times_rgb * sizeof(unsigned char*));
    sshot_array_host = (unsigned char*)malloc(pixels_times_rgb * sizeof(unsigned char*));
    match_count_host = (int*)malloc(pixels_times_rgb * sizeof(int*));

    // Allocate memory for each vector on GPU
    cudaMalloc(&templ_array_gpu, pixels_times_rgb * sizeof(unsigned char*));
    cudaMalloc(&sshot_array_gpu, pixels_times_rgb * sizeof(unsigned char*));
    cudaMalloc(&match_count_gpu, pixels_times_rgb * sizeof(int*));
    //image dimensions in pixels
    const int width_screenshot = 1920;
    const int width_template = 321;
    const int height_screenshot = 1080;
    const int height_template = 65;

    // outside loop is iterating the height
    // adding the width of the screenshot means that we are going to the next height pixel
    for (int height_iterator = 1; height_iterator < height_screenshot +1; height_iterator++)
    {
        for (int width_iterator = 1; width_iterator < width_screenshot +1; width_iterator++)
        {
            int sizeofpbuf = pBuf.capacity();
            int screenshot_iterator = 0;
            int alpha_iterator = 0;

            for (int template_iterator = 0; template_iterator < (sizeof(dota_colors) / sizeof(int)); template_iterator++)
            {
                //int testt = (sizeof(dota_colors) / sizeof(int) - 3000);
                alpha_iterator++;
             
                templ_array_host[template_iterator] = dota_colors[template_iterator];

                //removes the alpha layer from the equation aka every fourth increment
                if (alpha_iterator > 3) {
                    alpha_iterator = 0;
                    screenshot_iterator++;
                }

                sshot_array_host[template_iterator] = pBuf.at(screenshot_iterator);
                //for checking the values during debug
                unsigned char xxxxx = sshot_array_host[screenshot_iterator];
                //this means we are at the end of the line of the template image
                //move the sshot to the next line
                double testing = template_iterator % width_template;
                if (template_iterator % width_template == 0/*this just means we are at the end of the line of pixels the size of template width
                    in the screenshot wich is a small percentage of the size of the screenshot*/ 
                    && template_iterator != 0/*first line will start at zero so ignore it*/)
                {
                    //width screenshot + current position minus width of template equals the next line in the 'grid' of pixels
                    screenshot_iterator = ((width_screenshot * 4) + screenshot_iterator) - width_template;
                }
                else
                {
                    screenshot_iterator++;
                }
            }
            // Copy host vectors to device
            cudaMemcpy(templ_array_gpu, templ_array_host, pixels_times_rgb * sizeof(unsigned char*), cudaMemcpyHostToDevice);
            cudaMemcpy(sshot_array_gpu, sshot_array_host, pixels_times_rgb * sizeof(unsigned char*), cudaMemcpyHostToDevice);
            

            int blockSize, gridSize;

            // Number of threads in each thread block
            blockSize = 1024;

            // Number of thread blocks in grid
            gridSize = (int)ceil((float)pixels_times_rgb / blockSize);

            // Execute the kernel
            vecAdd <<< gridSize, blockSize >>> (templ_array_gpu, sshot_array_gpu, match_count_gpu, pixels_times_rgb);

            // Copy array back to host
            cudaMemcpy(match_count_host, match_count_gpu, sizeof(int*) * pixels_times_rgb, cudaMemcpyDeviceToHost);

            int TEST0 = match_count_host[0];
            int TEST1 = sizeof(match_count_host[0]);
            int TEST2 = match_count_host[500];
            
            ++match_count_host[0];
            int TEST3 = sizeof(match_count_host[0]);
            if (TEST0 > 1)
            {
                //hello
                printf("test");
            }

            // Release device memory
            cudaFree(templ_array_gpu);
            cudaFree(sshot_array_gpu);
            cudaFree(match_count_gpu);

        }

    }



    // Release host memory
    free(templ_array_host);
    free(sshot_array_host);
    free(match_count_host);
	

	//try {
	//	for (int i = 0; i < (ref_size - 4); i++) {
	//		  char r = pBuf.at((i * 4) + 2);
	//		  char g = pBuf.at((i * 4) + 1);
	//		  char b = pBuf.at(i * 4);
	//		/*
	//		if (g > 85 && r < g/2 && b < g/2) {
	//			std::string temp = "{" + to_string(r) + "," +
	//				to_string(g) + "," + to_string(b) + "}";
	//			if (!(colors.find(temp) != std::string::npos)) {
	//				colors += temp + ",";
	//			};

	//		}*/
	//		

	//		//vector<GPUCompute::_RGB> found_colors{};
	//		std::tuple<  char,   char,   char> rgb;
	//		
	//		for (std::tuple<  char,   char,   char> rgbt : cs_colors) {
	//			if (r == std::get<0>(rgbt) && g == std::get<1>(rgbt) && b == std::get<2>(rgbt)) {
	//				rgb = std::make_tuple(r, g, b);
	//				int c = 0;
	//				for (std::tuple<  char,   char,   char> vec_rgb : ref_vector) {
	//					if (r == std::get<0>(vec_rgb) && g == std::get<1>(vec_rgb) && b == std::get<2>(vec_rgb)) {
	//						c++;
	//						continue;
	//					}
	//				}
	//				if (c == 0) {
	//					ref_vector.push_back(rgb);
	//					position_vector.push_back(i);
	//				}
	//			}
	//		}
	//		
	//	}
	//}
	//catch (std::exception e) {
	//	std::string test = e.what();
	//	_log::log("\nERROR -SC");
	//	_log::log(test);
	//	_log::log("\nERROR -SC");
	//}
    return 0;
};