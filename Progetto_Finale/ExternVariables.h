#include <string>
#include <Windows.h>

#define DEBUG 0

namespace std {
#if defined _UNICODE || defined UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}

struct windows {
	HWND hWnd;
	std::tstring nome;
	std::wstring processID;
	HICON icon;
	bool active;

};

