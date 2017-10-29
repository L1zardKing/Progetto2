#include "GestioneLista.h"


#include <Windows.h>
#include <objidl.h>

#define NOMINMAX

#include <algorithm>

namespace Gdiplus
{
	using std::min;
	using std::max;
}

#include <gdiplus.h>
#include <OleCtl.h>
#include <Shlwapi.h>
#include <iostream>

using namespace Gdiplus;


#pragma comment (lib,"Gdiplus.lib")
#pragma comment(lib, "Shlwapi.lib")


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

void saveFileIconAsPng(HICON hIcon, const std::wstring& pngFile);

static CLSID g_pngClsid = GUID_NULL;

struct BITMAP_AND_BYTES {
	Gdiplus::Bitmap* bmp;
	int32_t* bytes;
};


void Mylist::sendList(int s1, std::shared_ptr<MyServer> server) {
	
	std::unique_lock<std::mutex> l(m);
	std::list<struct windows>::iterator iter = Wininfo.begin();
	std::list<struct windows>::iterator end = Wininfo.end();
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	std::wstring filePath(L"C:\\Users\\Kira\\index.png");
	Message msg;
	server->sendSize(s1, Wininfo.size());
	for (; iter != end; iter++)
	{
		if (iter->icon != NULL) {
			GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
			GetEncoderClsid(L"image/png", &g_pngClsid);
			saveFileIconAsPng(iter->icon, filePath);
			GdiplusShutdown(gdiplusToken);
			msg.filePath = filePath;

		}
		else msg.filePath = std::wstring (L"");


		std::wstringstream ss;
		ss << iter->hWnd;
		msg.hwnd = ss.str().c_str();
		msg.appName = iter->nome;
		msg.processPath = iter->processID;
		msg.active = iter->active;
		server->SerializeSend(msg, s1);

	}


	std::cout.clear();

}

void Mylist::sendCreated(int s1, std::shared_ptr<MyServer> server, struct windows createdOne)
{
	std::unique_lock<std::mutex> l(m);
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	std::wstring filePath(L"C:\\Users\\Kira\\index.png");
	Message msg;

	if (createdOne.icon != NULL) {
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		GetEncoderClsid(L"image/png", &g_pngClsid);
		saveFileIconAsPng(createdOne.icon, filePath);
		GdiplusShutdown(gdiplusToken);
		msg.filePath = filePath;

	}
	else msg.filePath = std::wstring(L"");
	
	std::wstringstream ss;
	ss << createdOne.hWnd;
	msg.hwnd = ss.str().c_str();
	msg.appName = createdOne.nome;
	msg.processPath = createdOne.processID;
	msg.active = true;						//assume that the created one is always the active one 
	server->SerializeSend(msg, s1);

	std::cout.clear();

}



void  Mylist::StampaLista() {
	std::unique_lock<std::mutex> l(m);
	std::list<struct windows>::iterator iter = Wininfo.begin();
	std::list<struct windows>::iterator end = Wininfo.end();

	std::wcout << Wininfo.size() << std::endl;
	for (; iter != end; iter++)
	{
		std::wcout << iter->hWnd << TEXT(":") << iter->nome.c_str() << " : " << iter->processID << " : " << iter->active << std::endl;
		std::wcout.clear();
	}
}



static BITMAP_AND_BYTES createAlphaChannelBitmapFromIcon(HICON hIcon) {

	// Get the icon info
	ICONINFO iconInfo = { 0 };
	GetIconInfo(hIcon, &iconInfo);

	// Get the screen DC
	HDC dc = GetDC(NULL);

	// Get icon size info
	BITMAP bm = { 0 };
	GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bm);

	// Set up BITMAPINFO
	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = bm.bmWidth;
	bmi.bmiHeader.biHeight = -bm.bmHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	// Extract the color bitmap
	int nBits = bm.bmWidth * bm.bmHeight;
	int32_t* colorBits = new int32_t[nBits];
	GetDIBits(dc, iconInfo.hbmColor, 0, bm.bmHeight, colorBits, &bmi, DIB_RGB_COLORS);

	// Check whether the color bitmap has an alpha channel.
	// (On my Windows 7, all file icons I tried have an alpha channel.)
	BOOL hasAlpha = FALSE;
	for (int i = 0; i < nBits; i++) {
		if ((colorBits[i] & 0xff000000) != 0) {
			hasAlpha = TRUE;
			break;
		}
	}

	// If no alpha values available, apply the mask bitmap
	if (!hasAlpha) {
		// Extract the mask bitmap
		int32_t* maskBits = new int32_t[nBits];
		GetDIBits(dc, iconInfo.hbmMask, 0, bm.bmHeight, maskBits, &bmi, DIB_RGB_COLORS);
		// Copy the mask alphas into the color bits
		for (int i = 0; i < nBits; i++) {
			if (maskBits[i] == 0) {
				colorBits[i] |= 0xff000000;
			}
		}
		delete[] maskBits;
	}

	// Release DC and GDI bitmaps
	ReleaseDC(NULL, dc);
	::DeleteObject(iconInfo.hbmColor);
	::DeleteObject(iconInfo.hbmMask);

	// Create GDI+ Bitmap
	Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(bm.bmWidth, bm.bmHeight, bm.bmWidth * 4, PixelFormat32bppARGB, (BYTE*)colorBits);
	BITMAP_AND_BYTES ret = { bmp, colorBits };

	return ret;
}

static void saveFileIconAsPng(HICON hIcon, const std::wstring& pngFile) {
	//HICON hIcon = getShellIcon(shilsize, fname);
	BITMAP_AND_BYTES bbs = createAlphaChannelBitmapFromIcon(hIcon);

	IStream* fstrm = NULL;
	HRESULT l = SHCreateStreamOnFile(pngFile.c_str(), STGM_WRITE | STGM_CREATE, &fstrm);
	if (l != S_OK) {
		std::cout << l << std::endl;
		exit(-1);
	}
	bbs.bmp->Save(fstrm, &g_pngClsid, NULL);
	fstrm->Release();

	delete bbs.bmp;
	delete[] bbs.bytes;
	DestroyIcon(hIcon);
}



int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}
