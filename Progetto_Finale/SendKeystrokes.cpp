#include "MainHeader.h"

//Send CRTL + X to the active window

void SendKeyStrokes(std::vector<std::string> app, int NKey) {

	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	/*
	VK_MENU == ALT
	VK_SHIFT == SHIFT
	VK_CONTROL == CTRL
	*/

	for (int i = 0; i < NKey; i++)
	{
		// Press the keys
		ip.ki.wVk = atoi(app.at(i).c_str());
		ip.ki.dwFlags = 0; // 0 for key press
		SendInput(1, &ip, sizeof(INPUT));
	}

	for (int i = NKey-1; i >= 0; i--) {
		// Release the keys
		ip.ki.wVk = atoi(app.at(i).c_str());
		ip.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &ip, sizeof(INPUT));
	}
	// Release the "Ctrl" key

	// We won't actually ever reach this point
	return;
}