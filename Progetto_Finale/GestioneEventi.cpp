#pragma once

#include "MainHeader.h"
#include <Windows.h>
#include <objidl.h>
#include <psapi.h>
#include <mutex>
#pragma comment(lib, "psapi.lib")

std::wstring getPath(unsigned long);
std::mutex myMut;
std::mutex m1;


VOID CALLBACK WinEventProcCallback(HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
	std::unique_lock<std::mutex> l(myMut);
	
	unsigned int flag = 0;
	struct windows created;
	struct windows destroyed;
	boolean flagFinded = false;

	if (dwEvent == EVENT_OBJECT_CREATE)
	{

		std::list<struct windows> m = WinInfo.getList();
		WinInfo.clear();
		::EnumWindows(enumWindowsProc, NULL);
		int dimensione = WinInfo.length() -1;

		if (dimensione == WinInfo.precGet()) {

			WinInfo.precSet(WinInfo.length());
			if (DEBUG) WinInfo.StampaLista();
			int length = ::GetWindowTextLength(hwnd);
			TCHAR* buffer;
			buffer = new TCHAR[length + 1];
			memset(buffer, 0, (length + 1) * sizeof(TCHAR));
			GetWindowText(hwnd, buffer, length + 1);
			std::tstring windowTitle = std::tstring(buffer);
			delete[] buffer;
			std::wcout << "A window has been created !!" << std::endl;
			std::wcout.clear();

			std::list<struct windows> actualList = WinInfo.getList();
			std::list<struct windows>::iterator iter = actualList.begin();
			std::list<struct windows>::iterator end = actualList.end();
			
			for (; iter != end; iter++) {
				std::list<struct windows>::iterator iter1 = m.begin();
				std::list<struct windows>::iterator end1 = m.end();
				for (; iter1 != end1; iter1++) {
					if (iter1->hWnd == iter->hWnd) {
						flag = 1;
					}
				}

				if (flag == 0) {
					std::wcout << "Finded HWND" << std::endl;
					created = *iter;
					flagFinded = true;
				}
				flag = 0;

			}

			std::wcout << "HDWN of created window is " << created.hWnd << std::endl;
			//if (flagFinded != true) exit(1);

			HWND activeWindow = GetForegroundWindow();
			if (activeWindow == NULL) {
				std::wcout << "Problem retrieving active window gestioneEventi 72" << std::endl;
				//exit(1);
			}

			WinInfo.ActiveOneSet(activeWindow);

			if (ListAltreadySent.load() && flagFinded) {
				std::wcout << "Invio created " << std::endl;
				std::wcout.clear();
				server->sendInt(1, s1);
				WinInfo.sendCreated(s1, server, created);
			}

			WinInfo.StampaLista();
			std::wcout.clear();
		}
		else (WinInfo.setList(m));


	}
		
	
	
	else if (dwEvent == EVENT_OBJECT_DESTROY)
	{
		std::list<struct windows> m = WinInfo.getList();
		boolean flagFinded = false;
		WinInfo.clear();
		::EnumWindows(enumWindowsProc, NULL);
		int dimensione = WinInfo.length() + 1;
		if (dimensione == WinInfo.precGet()) {
			if (DEBUG) WinInfo.StampaLista();
			WinInfo.precSet(WinInfo.length());
			int length = ::GetWindowTextLength(hwnd);
			TCHAR* buffer;
			buffer = new TCHAR[length + 1];
			memset(buffer, 0, (length + 1) * sizeof(TCHAR));

			GetWindowText(hwnd, buffer, length + 1);
			std::tstring windowTitle = std::tstring(buffer);
			delete[] buffer;
			std::wcout << "A window has been destroyed !!" << std::endl;
			std::wcout.clear();

			std::list<struct windows> actualList = WinInfo.getList();

			std::list<struct windows>::iterator iter1 = m.begin();
			std::list<struct windows>::iterator end1 = m.end();

			for (; iter1 != end1; iter1++) {

				std::list<struct windows>::iterator iter = actualList.begin();
				std::list<struct windows>::iterator end = actualList.end();
				flag = 0;
				for (; iter != end; iter++) {
					if (iter1->hWnd == iter->hWnd) {
						flag = 1;
						break;
					}
				}

				if (flag == 0) {
					destroyed = *iter1;
					flagFinded = true;
					break;
				}

			}

			//if (flagFinded != true) exit(1);
			//std::wcout.clear();

			HWND activeWindow = GetForegroundWindow();
			std::wcout << "HDWN of active window is " << activeWindow << std::endl;


			if (ListAltreadySent.load()) {

				std::wcout << "Invio destroyed " << std::endl;
				std::wcout.clear();
				server->sendInt(2, s1);
				server->sendDestroyed(s1, destroyed.hWnd, activeWindow);
			}
			
			std::wcout.clear();
		}
		else (WinInfo.setList(m));
	}

	else if (dwEvent == EVENT_OBJECT_FOCUS)
	{
		int notCreated = 0;

		std::cout << "focus changed" << std::endl;


		if (IsWindowVisible(hwnd) || (GetWindowLong(hwnd, GWL_STYLE) & WS_EX_APPWINDOW)) {
		
			//WinInfo.clear();
			//::EnumWindows(enumWindowsProc, NULL);
			HWND activeWindow = GetForegroundWindow();

			if (activeWindow == NULL) {
				//std::wcout << "Problem retrieving active window" << std::endl;
				server->sendInt(4, s1);
				return;
			}

			std::list<struct windows> actualList = WinInfo.getList();

			std::list<struct windows>::iterator iter1 = actualList.begin();
			std::list<struct windows>::iterator end1 = actualList.end();

			for (; iter1 != end1; iter1++) {
				if (iter1->hWnd == activeWindow) {
					notCreated = 1;
				}
			}

			//std::cout << "Focus changed" << std::endl;

			//WinInfo.ActiveOneSet(activeWindow);
			

			if (notCreated == 1) {

				if (ListAltreadySent.load()) {

					server->sendInt(3, s1);

					server->sendFocus(s1, activeWindow);

				}

			}
		}
	}
	
	else if (dwEvent == EVENT_OBJECT_NAMECHANGE) {

		int flag = 0;
		std::list<struct windows> actualList = WinInfo.getList();

		std::list<struct windows>::iterator iter1 = actualList.begin();
		std::list<struct windows>::iterator end1 = actualList.end();

		for (; iter1 != end1; iter1++) {
			if (iter1->hWnd == hwnd) {
				flag = 1;
			}
		}
		if (flag == 1) {

			int length = ::GetWindowTextLength(hwnd);
			TCHAR* buffer;
			buffer = new TCHAR[length + 1];
			memset(buffer, 0, (length + 1) * sizeof(TCHAR));

			GetWindowText(hwnd, buffer, length + 1);
			std::tstring windowTitle = std::tstring(buffer);
			std::wcout << "name changed" << buffer << std::endl;

			server->sendInt(5, s1);

			server->sendChangeName(s1, hwnd, windowTitle);

			
		
		}
	}
	
	return;
}



BOOL CALLBACK enumWindowsProc(
	__in  HWND hWnd,
	__in  LPARAM lParam
	) {

	std::unique_lock<std::mutex> l(m1);

	if (!IsWindowVisible(hWnd) || !(GetWindowLong(hWnd, GWL_STYLE) & WS_EX_APPWINDOW))
		return TRUE;

	// L'ultimo parametro ci identifica quelle nella task bar


	struct windows newElement;
	WPARAM wParam = NULL;
	int length = ::GetWindowTextLength(hWnd);
	if (length == 0) return TRUE;

	TCHAR* buffer;
	buffer = new TCHAR[length + 1];
	memset(buffer, 0, (length + 1) * sizeof(TCHAR));
	LPTSTR luna = new TCHAR[500];
	GetWindowText(hWnd, luna, length + 1);
	GetWindowText(hWnd, buffer, length + 1);

	if (length != GetWindowText(hWnd, buffer, length + 1)) {
		std::cout << "name not found" << std::endl;
		length = 20;
		memset(buffer, 0, (length + 1) * sizeof(TCHAR));
		wcscpy(buffer, L"Not found");
	}



	HICON icon = (HICON) SendMessage(hWnd, WM_GETICON, ICON_SMALL2, 0);
	if (icon == NULL)
		icon = (HICON) SendMessage(hWnd, WM_GETICON, ICON_SMALL, 0);
	if (icon  == NULL)
		icon = (HICON) SendMessage(hWnd, WM_GETICON, ICON_BIG, 0);
	if (icon  == NULL)
		icon = (HICON) GetClassLongPtr(hWnd, GCL_HICON);
	if (icon == NULL)
		icon = (HICON) GetClassLongPtr(hWnd, GCL_HICONSM);

	std::tstring windowTitle = std::tstring(buffer);
	delete[] buffer;

	unsigned long process_id = 0;

	GetWindowThreadProcessId(hWnd, &process_id);
	
	std::wstring process_path = getPath(process_id);
	newElement.processID = process_path;
	newElement.nome = windowTitle;
	newElement.icon = icon;
	newElement.hWnd = hWnd;

	WinInfo.push_back(newElement);

	return TRUE;
}

BOOL CALLBACK enumWindowsProcA(
	__in  HWND hWnd,
	__in  LPARAM lParam
	) {


	//std::unique_lock<std::mutex> l(m1);

	if (!IsWindowVisible(hWnd) || !(GetWindowLong(hWnd, GWL_STYLE) & WS_EX_APPWINDOW))
		return TRUE;

	// L'ultimo parametro ci identifica quelle nella task bar


	struct windows newElement;
	WPARAM wParam = NULL;
	int length = ::GetWindowTextLength(hWnd);
	if (length == 0) return TRUE;
	TCHAR* buffer;
	buffer = new TCHAR[length + 1];
	memset(buffer, 0, (length + 1) * sizeof(TCHAR));

	
	HICON icon = (HICON)SendMessage(hWnd, WM_GETICON, ICON_SMALL2, 0);
	if (icon == NULL)
		icon = (HICON)SendMessage(hWnd, WM_GETICON, ICON_SMALL, 0);
	if (icon == NULL)
		icon = (HICON)SendMessage(hWnd, WM_GETICON, ICON_BIG, 0);
	if (icon == NULL)
		icon = (HICON)GetClassLongPtr(hWnd, GCL_HICON);
	if (icon == NULL)
		icon = (HICON)GetClassLongPtr(hWnd, GCL_HICONSM);

	std::tstring windowTitle = std::tstring(buffer);
	delete[] buffer;

	unsigned long process_id = 0;

	GetWindowThreadProcessId(hWnd, &process_id);

	std::wstring process_path = getPath(process_id);
	newElement.processID = process_path;
	newElement.nome = windowTitle;
	newElement.icon = icon;
	newElement.hWnd = hWnd;
	
	WinInfo1.push_back(newElement);

	return TRUE;
}


// http://msdn.microsoft.com/en-us/library/windows/desktop/ms533843(v=vs.85).aspx

std::wstring getPath(unsigned long process_id) {

	HANDLE processHandle = NULL;
	WCHAR filename[MAX_PATH];

	processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);
	if (processHandle != NULL) {
		if (GetModuleFileNameEx(processHandle, NULL, filename, MAX_PATH) == 0) {
			//std::wcerr << "Failed to get module filename." << std::endl;
		}
		else {
			//std::wcout << "Module filename is: " << filename << std::endl;
		}
		CloseHandle(processHandle);
		return filename;
	}
	else {
		//std::wcerr << "Failed to open process." << std::endl;
		CloseHandle(processHandle);
		return std::wstring(L"");
	}
}