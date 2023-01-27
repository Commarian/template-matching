#include "image_native.h"

#include <iostream>
#include <fstream>
#include <gdiplus.h>
#include <debugapi.h>
#include "_log.h"

using std::array;
using std::tuple;
using _log::log;
using std::equal;
using std::begin;
using std::end;
using std::to_string;
using std::string;
using std::tuple;
using std::make_tuple;
using std::get;

static int return_x = 0;
static int return_y = 0;

static Gdiplus::Color color;

struct RGB_
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
};


const static int teneightyp = 1920*1080;



static int pixel_match = 0;



namespace INAT {
    std::tuple<int, int> imaging(int ref_width, int ref_height, BYTE* pBuf)
    {
		/*
        if (game == 1) {
            OutputDebugStringA("GAME == 1");

            //bitmap bmp{ "ss.src","_c.src" };
            return make_tuple(return_x, return_y);
        }
        else if (game == 2) {
            try {
                OutputDebugStringA("GAME == 2");
            }
            catch (std::exception e) {
                log(e.what());
            }
            
            return make_tuple(return_x, return_y);
        }*/
		ULONG_PTR gdiplusToken;
		//GDIPLUS INITIALIZATION

		Gdiplus::GdiplusStartupInput gdiplusStartupInput;

		// Initialize GDI+.
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);




		std::string colorstring = "\n";

		//GDIPLUS BITMAP FOR TEMPLATE
		std::wstring filename = L"autoaccept.bmp";
		Gdiplus::Bitmap* temp_bmp;
		temp_bmp = Gdiplus::Bitmap::FromFile(filename.c_str());

		


		int temp_height = temp_bmp->GetHeight();
		int temp_width = temp_bmp->GetWidth();


		//allocate the array for reference		

		/*
		//FOR LOOP FOR BITMAP TEMPLATE
		for (int y = 0; y < teneightyp; y++) {
			
			for (int x = 0; x < temp_width; x++) {
				temp_bmp->GetPixel(x, y, &color);
				temp_a[x][y].r = (int)color.GetR();
				temp_a[x][y].g = (int)color.GetG();
				temp_a[x][y].b = (int)color.GetB();
				//colorstring += std::to_string(color.GetR()) + "\t" + std::to_string(color.GetG()) +
				//	"\t" + std::to_string(color.GetB()) + "\n";
			}
		}*/


		//_log::log(colorstring);
		//_log::log("END OF FIRST");

		colorstring = "\n";



		static int buff_int = 1;
		//REFERENCE
		for (int yy = ref_height; yy > (0); yy--) {
			//OutputDebugStringA("\n");
			//OutputDebugStringA(std::to_string(yy).c_str());
			//OutputDebugStringA("\n");
			OutputDebugStringA("\n");
			OutputDebugStringA(to_string(yy).c_str());
			for (int xx = 0; xx < (ref_width); xx++) {
				//starts at x = 0 and y = height
				//OutputDebugStringA("\n");
				//OutputDebugStringA(std::to_string(yy).c_str());
				for (int x = 0; x < temp_width; x++) {
					for (int y = temp_height; y > 0; y--) {
						temp_bmp->GetPixel(x, y, &color);
						if ((unsigned char)pBuf[buff_int] == (unsigned char)color.GetB())
						{
							if ((unsigned char)pBuf[(buff_int + 1)] == (unsigned char)color.GetG())
							{
								if ((unsigned char)pBuf[(buff_int + 2)] == (unsigned char)color.GetR())
								{
									pixel_match++;
								}
							}
						}
						buff_int += 4;//next
					}
				}

			}
		}
		OutputDebugStringA(to_string(pixel_match).c_str());

		//NESTED FOR LOOP FOR BITMAP REFERENCE
		/*
		for (int yy = ref_height; yy > (0); yy--) {

			OutputDebugStringA("\n");
			OutputDebugStringA(std::to_string(yy).c_str());
			//OutputDebugStringA("\n");

			for (int xx = 0; xx < (ref_width); xx++) {
				//starts at x = 0 and y = height
				//OutputDebugStringA("\n");
				
				//OutputDebugStringA(std::to_string(yy).c_str());
				try {
					ref_a[xx][yy].b = (unsigned char)pBuf[buff_int];
					//OutputDebugStringA(std::to_string(ref_a[xx][yy].b).c_str());
					buff_int++;
					ref_a[xx][yy].g = (unsigned char)pBuf[buff_int];
					buff_int++;
					ref_a[xx][yy].r = (unsigned char)pBuf[buff_int];
					buff_int++;//alpha value skip
					buff_int++;//alpha value skip
				}
				catch (std::exception e) {
					string s = e.what();
					OutputDebugStringA(s.c_str());
				}
			}
			//colorstring += std::to_string(ref_a[xx][yy].r) + "\t" + std::to_string(ref_a[xx][yy].g) + "\t" + std::to_string(ref_a[xx][yy].b) + "\n";
		}

		colorstring += "\tREF\t" + std::to_string(ref_a[750][750].b) + "\tTEMP\t" + std::to_string(temp_a[750][750].b) + "\n";
		colorstring += "\tREF\t" + std::to_string(ref_a[1500][250].g) + "\tTEMP\t" + std::to_string(temp_a[1500][250].g) + "\n";
		colorstring += "\tREF\t" + std::to_string(ref_a[1500][800].g) + "\tTEMP\t" + std::to_string(temp_a[1500][800].g) + "\n";
		colorstring += "\tREF\t" + std::to_string(ref_a[450][250].r) + "\tTEMP\t" + std::to_string(temp_a[450][250].r) + "\n";
		_log::log(colorstring);
		*/
		//ShellExecute(0, 0, lFilePath.c_str(), 0, 0, SW_SHOW);
		//deallocate the array

		//delete[] pBuf;
		
		Gdiplus::GdiplusShutdown(gdiplusToken);
        return make_tuple(0, 0);
    }
}

