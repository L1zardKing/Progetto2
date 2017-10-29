/*#pragma once

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
*/