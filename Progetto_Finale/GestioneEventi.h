#pragma once
#include "MainHeader.h"

extern Mylist WinInfo;

namespace std {
#if defined _UNICODE || defined UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}

VOID CALLBACK WinEventProcCallback(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime);

BOOL CALLBACK enumWindowsProc(
	__in  HWND hWnd,
	__in  LPARAM lParam
	);

BOOL CALLBACK enumWindowsProcA(__in  HWND, __in LPARAM);

