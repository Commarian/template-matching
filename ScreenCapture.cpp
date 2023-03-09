#define WIN32_LEAN_AND_MEAN    
#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <memory>
#include <algorithm>
#include <string>
#include <thread>
#include <gdiplus.h>
#include <cstdint>
#include <gdiplus.h>
#include <gdiplusheaders.h>
#include <vector>
#include "ScreenCapture.h"
#include "_log.h"
#include "image_native.h"




#pragma comment(lib, "D3D11.lib")

//CUSTOM SETTINGS
static int glob_x = 0;
static int glob_y = 0;

template <typename T>
class CComPtrCustom
{
public:

	CComPtrCustom(T* aPtrElement)
		:element(aPtrElement)
	{
	}

	CComPtrCustom()
		:element(nullptr)
	{
	}

	virtual ~CComPtrCustom()
	{
		Release();
	}

	T* Detach()
	{
		auto lOutPtr = element;

		element = nullptr;

		return lOutPtr;
	}

	T* detach()
	{
		return Detach();
	}

	void Release()
	{
		if (element == nullptr)
			return;

		auto k = element->Release();

		element = nullptr;
	}

	CComPtrCustom& operator = (T* pElement)
	{
		Release();

		if (pElement == nullptr)
			return *this;

		auto k = pElement->AddRef();

		element = pElement;

		return *this;
	}

	void Swap(CComPtrCustom& other)
	{
		T* pTemp = element;
		element = other.element;
		other.element = pTemp;
	}

	T* operator->()
	{
		return element;
	}

	operator T* ()
	{
		return element;
	}

	operator T* () const
	{
		return element;
	}


	T* get()
	{
		return element;
	}

	T* get() const
	{
		return element;
	}

	T** operator &()
	{
		return &element;
	}

	bool operator !()const
	{
		return element == nullptr;
	}

	operator bool()const
	{
		return element != nullptr;
	}

	bool operator == (const T* pElement)const
	{
		return element == pElement;
	}


	CComPtrCustom(const CComPtrCustom& aCComPtrCustom)
	{
		if (aCComPtrCustom.operator!())
		{
			element = nullptr;

			return;
		}

		element = aCComPtrCustom;

		auto h = element->AddRef();

		h++;
	}

	CComPtrCustom& operator = (const CComPtrCustom& aCComPtrCustom)
	{
		Release();

		element = aCComPtrCustom;

		auto k = element->AddRef();

		return *this;
	}

	_Check_return_ HRESULT CopyTo(T** ppT) throw()
	{
		if (ppT == NULL)
			return E_POINTER;

		*ppT = element;

		if (element)
			element->AddRef();

		return S_OK;
	}

	HRESULT CoCreateInstance(const CLSID aCLSID)
	{
		T* lPtrTemp;

		auto lresult = ::CoCreateInstance(aCLSID, NULL, CLSCTX_INPROC, IID_PPV_ARGS(&lPtrTemp));

		if (SUCCEEDED(lresult))
		{
			if (lPtrTemp != nullptr)
			{
				Release();

				element = lPtrTemp;
			}

		}

		return lresult;
	}

protected:

	T* element;
};



// Driver types supported
D3D_DRIVER_TYPE gDriverTypes[] =
{
	D3D_DRIVER_TYPE_HARDWARE
};
UINT gNumDriverTypes = ARRAYSIZE(gDriverTypes);

// Feature levels supported
D3D_FEATURE_LEVEL gFeatureLevels[] =
{
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_1
};

UINT gNumFeatureLevels = ARRAYSIZE(gFeatureLevels);



namespace SS {

	//see if the two ints are almost equal
	boolean almost_equal(unsigned char x, unsigned char y)
	{
		if (x == y) {
			return true;
		}
		if (x > y) {
			//if positive difference between the two points are 2 pixels or less away, return true
			if ((abs(x - y) < 3))
			{
				return true;
			}
		}
		else {
			if ((abs(y - x) < 3))
			{
				return true;
			}
		}
		return false;
	}



	std::tuple<int, int> ScreenCapture(int game_running)
	{
		CComPtrCustom<ID3D11Device> lDevice;
		CComPtrCustom<ID3D11DeviceContext> lImmediateContext;
		CComPtrCustom<IDXGIOutputDuplication> lDeskDupl;
		CComPtrCustom<ID3D11Texture2D> lAcquiredDesktopImage;
		CComPtrCustom<ID3D11Texture2D> lGDIImage;
		CComPtrCustom<ID3D11Texture2D> lDestImage;
		DXGI_OUTPUT_DESC lOutputDesc;
		DXGI_OUTDUPL_DESC lOutputDuplDesc;

		int lresult(-1);

		do
		{
			D3D_FEATURE_LEVEL lFeatureLevel;
			HRESULT hr(E_FAIL);
			// Create device
			for (UINT DriverTypeIndex = 0; DriverTypeIndex < gNumDriverTypes; ++DriverTypeIndex)
			{
				hr = D3D11CreateDevice(
					nullptr,
					gDriverTypes[DriverTypeIndex],
					nullptr,
					0,
					gFeatureLevels,
					gNumFeatureLevels,
					D3D11_SDK_VERSION,
					&lDevice,
					&lFeatureLevel,
					&lImmediateContext);

				if (SUCCEEDED(hr))
				{
					// Device creation success, no need to loop anymore
					break;
				}

				lDevice.Release();

				lImmediateContext.Release();
			}

			if (FAILED(hr)) {
				_log::log("Device failed to create -SC");
				break;
			}
				
			Sleep(100);
			if (lDevice == nullptr)
				break;
			// Get DXGI device
			CComPtrCustom<IDXGIDevice> lDxgiDevice;
			hr = lDevice->QueryInterface(IID_PPV_ARGS(&lDxgiDevice));
			if (FAILED(hr)) {
				_log::log("Device failed to create -SC");
				break;
			}
			// Get DXGI adapter
			CComPtrCustom<IDXGIAdapter> lDxgiAdapter;
			hr = lDxgiDevice->GetParent(
				__uuidof(IDXGIAdapter),
				reinterpret_cast<void**>(&lDxgiAdapter));
			if (FAILED(hr)) {
				_log::log("Device failed to create -SC");
				break;
			}
			lDxgiDevice.Release();
			UINT Output = 0;
			// Get output
			CComPtrCustom<IDXGIOutput> lDxgiOutput;
			hr = lDxgiAdapter->EnumOutputs(
				Output,
				&lDxgiOutput);
			if (FAILED(hr)) {
				_log::log("Device failed to create -SC");
				break;
			}
			lDxgiAdapter.Release();
			hr = lDxgiOutput->GetDesc(&lOutputDesc);
			if (FAILED(hr)) {
				_log::log("Device failed to create -SC");
				break;
			}

			// QI for Output 1
			CComPtrCustom<IDXGIOutput1> lDxgiOutput1;
			hr = lDxgiOutput->QueryInterface(IID_PPV_ARGS(&lDxgiOutput1));
			if (FAILED(hr)) {
				_log::log("Device failed to create -SC");
				break;
			}
			lDxgiOutput.Release();

			// Create desktop duplication
			hr = lDxgiOutput1->DuplicateOutput(
				lDevice,
				&lDeskDupl);
			if (FAILED(hr)) {
				_log::log("Device failed to create -SC");
				break;
			}
			lDxgiOutput1.Release();

			// Create GUI drawing texture
			lDeskDupl->GetDesc(&lOutputDuplDesc);

			D3D11_TEXTURE2D_DESC desc;

			desc.Width = lOutputDuplDesc.ModeDesc.Width;
			desc.Height = lOutputDuplDesc.ModeDesc.Height;
			desc.Format = lOutputDuplDesc.ModeDesc.Format;
			desc.ArraySize = 1;
			desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
			desc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.MipLevels = 1;
			desc.CPUAccessFlags = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			hr = lDevice->CreateTexture2D(&desc, NULL, &lGDIImage);
			if (FAILED(hr)) {
				_log::log("Device failed to create -SC");
				break;
			}
			if (lGDIImage == nullptr) {
				_log::log("LGDIImage null pointer -SC");
				break;
			}

			// Create CPU access texture
			desc.Width = lOutputDuplDesc.ModeDesc.Width;
			desc.Height = lOutputDuplDesc.ModeDesc.Height;
			desc.Format = lOutputDuplDesc.ModeDesc.Format;
			desc.ArraySize = 1;
			desc.BindFlags = 0;
			desc.MiscFlags = 0;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.MipLevels = 1;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
			desc.Usage = D3D11_USAGE_STAGING;

			hr = lDevice->CreateTexture2D(&desc, NULL, &lDestImage);

			if (FAILED(hr)) {
				_log::log("Device failed to create -SC");
				break;
			}
			if (lDestImage == nullptr) {
				_log::log("DestImage null -SC");
				break;
			}

			CComPtrCustom<IDXGIResource> lDesktopResource;
			DXGI_OUTDUPL_FRAME_INFO lFrameInfo;

			int lTryCount = 4;

			do
			{
				Sleep(100);
				// Get new frame
				hr = lDeskDupl->AcquireNextFrame(
					250,
					&lFrameInfo,
					&lDesktopResource);

				if (SUCCEEDED(hr))
					break;

				if (hr == DXGI_ERROR_WAIT_TIMEOUT)
				{
					continue;
				}
				else if (FAILED(hr))
					break;

			} while (--lTryCount > 0);

			if (FAILED(hr)) {
				_log::log("Device failed to create -SC");
				break;
			}

			// QI for ID3D11Texture2D

			hr = lDesktopResource->QueryInterface(IID_PPV_ARGS(&lAcquiredDesktopImage));

			if (FAILED(hr)) {
				_log::log("Device failed to create -SC");
				break;
			}

			lDesktopResource.Release();

			if (lAcquiredDesktopImage == nullptr)
				break;

			// Copy image into GDI drawing texture

			lImmediateContext->CopyResource(lGDIImage, lAcquiredDesktopImage);


			// Draw cursor image into GDI drawing texture

			CComPtrCustom<IDXGISurface1> lIDXGISurface1;

			hr = lGDIImage->QueryInterface(IID_PPV_ARGS(&lIDXGISurface1));

			if (FAILED(hr)) {
				_log::log("Device failed to create -SC");
				break;
			}
			// Copy image into CPU access texture
			lImmediateContext->CopyResource(lDestImage, lGDIImage);
			// Copy from CPU access texture to bitmap buffer

			D3D11_MAPPED_SUBRESOURCE resource;
			UINT subresource = D3D11CalcSubresource(0, 0, 0);
			lImmediateContext->Map(lDestImage, subresource, D3D11_MAP_READ_WRITE, 0, &resource);		

			// Allocate temporary memory
			auto data = std::unique_ptr<uint32_t[]>(
				new uint32_t[lOutputDuplDesc.ModeDesc.Width * lOutputDuplDesc.ModeDesc.Height]);

			auto src = static_cast<uint8_t*>(resource.pData);
			uint32_t* dest = data.get();

			

			for (UINT y = 0; y < lOutputDuplDesc.ModeDesc.Height; ++y)
			{
				// Multiply by 4 as there are 4 bytes (32 bits) per pixel
				memcpy(dest, src, lOutputDuplDesc.ModeDesc.Width * sizeof(uint32_t));
				src += resource.RowPitch;
				dest += lOutputDuplDesc.ModeDesc.Width;
			}
			;
			using std::vector;
			vector<unsigned char> pBuf(8294400);
			//BYTE pBuf[8294400]{};


			UINT lBmpRowPitch = lOutputDuplDesc.ModeDesc.Width * 4;

			unsigned char* sptr = reinterpret_cast<unsigned char*>(resource.pData);
			unsigned char* dptr = pBuf.data() + lOutputDuplDesc.ModeDesc.Width
				* lOutputDuplDesc.ModeDesc.Height * 4 - lBmpRowPitch;

			unsigned char* imageData = nullptr;

			UINT lRowPitch = std::min<UINT>(lBmpRowPitch, resource.RowPitch);


			for (size_t h = 0; h < lOutputDuplDesc.ModeDesc.Height; ++h)
			{
				memcpy_s(dptr, lBmpRowPitch, sptr, lRowPitch);
				sptr += resource.RowPitch;
				dptr -= lBmpRowPitch;
			}



			std::this_thread::sleep_for(std::chrono::seconds(1));
			ULONG_PTR gdiplusToken;
			//GDIPLUS INITIALIZATION

			Gdiplus::GdiplusStartupInput gdiplusStartupInput;

			// Initialize GDI+.
			Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

			//Reference attributes - specifically the resolution of the screen
			int res_height = lOutputDuplDesc.ModeDesc.Height;
			int res_width = lOutputDuplDesc.ModeDesc.Width;
			int res_size = res_width * res_height;
			std::wstring filename;
			int colors_to_match = 0;
			double height_to_remove_half = 0;//50% total top and bottom added together
			//Template filename based on game running
			//cs go
			if (game_running == 1) {
				height_to_remove_half = res_height * 0.3;
				filename = L"_cs.bmp";
				colors_to_match = 2068 * 0.45;
			}
			//dota
			else if (game_running == 2) {
				filename = L"_d.src";
				height_to_remove_half = res_height * 0.2;
				colors_to_match = 1050 * 0.9;
			}
			//nothing error
			else {
				_log::log("Error: No game running - SC");
				return std::make_tuple(0, 0);
			}
			//reset the global return values to zero
			glob_x = 0;
			glob_y = 0;
			using std::to_string;


			//color to use for template
			static Gdiplus::Color color;
			static int pixel_match = 0;
			//_log::log(to_string(ref_size));
			std::string test = "";		
			//remove a part of the screen image
			//pBuf.erase(pBuf.end() - height_to_remove_half * res_width * 4, pBuf.end());//200 y units from top
			//pBuf.erase(pBuf.begin(), pBuf.begin() + height_to_remove_half * res_width * 4);//200 y units from bottom
			double ref_height = res_height - height_to_remove_half * 2;
			double ref_width = res_width;
			double ref_size = ref_width * ref_height;
			//makes sure the vector is not filled with empty/redundant data
			//pBuf.shrink_to_fit();

			//_RGB rgbx[] = {{0,0,0},{0,0,0}};



			/*
			_log::log("Before");
			_log::log(to_string(ref_height));
			_log::log(to_string(ref_width));
			_log::log(to_string(ref_size));
			_log::log(to_string(pBuf.size()));*/
			
			std::string colors = "";
			/*
			for (int kk = 0; kk < temp_width; kk++) {
				for (int hh = 0; hh < temp_height; hh++) {
					temp_bmp->GetPixel(kk, hh, &color);
					std::string temp = "{" + to_string(color.GetR()) + "," +
						to_string(color.GetG()) + "," + to_string(color.GetB()) + "}";
					if (!(colors.find(temp) != std::string::npos)) {
						colors += temp + ",";
					};					
				}
			}
			_log::log(colors);
			*/
			
			auto now = std::chrono::system_clock::now() + std::chrono::hours(2);
			test = std::format("{:%d-%m-%Y %H:%M:%OS}", now) + "\tTIME BEFORE\n";
			OutputDebugStringA(test.c_str());
			
			//try {
			//	for (int i = 0; i < (ref_size - 4); i++) {
			//		unsigned char r = pBuf.at((i * 4) + 2);
			//		unsigned char g = pBuf.at((i * 4) + 1);
			//		unsigned char b = pBuf.at(i * 4);
			//		/*
			//		if (g > 85 && r < g/2 && b < g/2) {
			//			std::string temp = "{" + to_string(r) + "," +
			//				to_string(g) + "," + to_string(b) + "}";
			//			if (!(colors.find(temp) != std::string::npos)) {
			//				colors += temp + ",";
			//			};

			//		}*/


			//		//vector<_RGB> found_colors{};
			//		_RGB rgb{ NULL };
			//		if (game_running == 1) {
			//			for (_RGB rgbt : cs_colors) {
			//				if (r == rgbt.r && g == rgbt.g && b == rgbt.b) {
			//					rgb.r = r;
			//					rgb.g = g;
			//					rgb.b = b;
			//					int c = 0;
			//					for (_RGB vec_rgb : ref_vector) {
			//						if (r == vec_rgb.r && g == vec_rgb.g && b == vec_rgb.b) {
			//							c++;
			//							continue;
			//						}
			//					}
			//					if (c == 0) {
			//						ref_vector.push_back(rgb);
			//						position_vector.push_back(i);
			//					}
			//				}
			//			}
			//		}
			//		else {
			//			for (_RGB rgbt : dota_colors) {
			//				if (r == rgbt.r && g == rgbt.g && b == rgbt.b) {
			//					rgb.r = r;
			//					rgb.g = g;
			//					rgb.b = b;
			//					int c = 0;
			//					for (_RGB vec_rgb : ref_vector) {
			//						if (r == vec_rgb.r && g == vec_rgb.g && b == vec_rgb.b) {
			//							c++;
			//							continue;
			//						}
			//					}
			//					if (c == 0) {
			//						ref_vector.push_back(rgb);
			//						position_vector.push_back(i);
			//					}
			//				}
			//			}
			//		}
			//	}
			//}
			//catch (std::exception e) {
			//	test = e.what();
			//	_log::log("\nERROR -SC");
			//	_log::log(test);
			//	_log::log("\nERROR -SC");
			//}
			GPUCompute::Compute(pBuf, ref_size, game_running);
			std::string computed_array;
			
			
			_log::log(colors);
			OutputDebugStringA("Resulting String");
			OutputDebugStringA(computed_array.c_str());
			

			//now = std::chrono::system_clock::now() + std::chrono::hours(2);
			//test = std::format("{:%d-%m-%Y %H:%M:%OS}", now) + "\tTIME AFTER+\n";
			//OutputDebugStringA(test.c_str());
			//pBuf.clear();
			//OutputDebugStringA("\nref_vector capacity = ");
			//OutputDebugStringA(to_string(ref_vector.capacity()).c_str());
			//OutputDebugStringA("\n");
			//if (ref_vector.capacity() < colors_to_match) {
			//	OutputDebugStringA("less than 90% matched");
			//	return  std::make_tuple(glob_x, glob_y);
			//}


			//double pos_sum = 0;
			//double pos_pseudo = 0;
			//int pos_pseudo_counter = 0;
			//for (double x : position_vector) {
			//	pos_sum += x;
			//}
			//double pos_avg = pos_sum/position_vector.capacity();


			//for (double x : position_vector) {

			//	if (x > (pos_avg + (100 * ref_width * 2)) || x < (pos_avg - (100 * ref_width * 2))) {
			//	}
			//	else {
			//		pos_pseudo += x;
			//		pos_pseudo_counter++;
			//	}

			//}

			//pos_avg = pos_pseudo / pos_pseudo_counter;
			//OutputDebugStringA("\npos_pseudo_counter = ");
			//OutputDebugStringA(to_string(pos_pseudo_counter).c_str());
			//OutputDebugStringA("\npos_pseudo = ");
			//OutputDebugStringA(to_string(pos_pseudo).c_str());
			//
			//if (pos_avg < 0) {
			//	return std::make_tuple(glob_x, glob_y);
			//}
			//int local_y = res_height - ((pos_avg / res_width) + height_to_remove_half);
			//if (local_y <= 0) {
			//	return std::make_tuple(glob_x, glob_y);
			//}
			//glob_y = local_y;
			//glob_x = res_width/2;
			return std::make_tuple(glob_x, glob_y);

		} while (false);
		return std::make_tuple(glob_x, glob_y);
	}
}
