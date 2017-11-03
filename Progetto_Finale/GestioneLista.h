#pragma once


#include <list>
#include <mutex>
#include <sstream>
#include <boost/asio.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include "ExternVariables.h"
#include "MyServerClass.h"

#define MAX 1024

class Mylist {
private:
	std::list<struct windows> Wininfo;
	struct windows ActiveOne;
	std::mutex m;
	int prec;

public:
	~Mylist() {	}

	std::list<struct windows> getList() {
		std::unique_lock<std::mutex> l(m);
		return Wininfo;
	}

	void setList(std::list<struct windows> m1) {
		std::unique_lock<std::mutex> l(m);
		Wininfo = m1;
	}



	HWND returnHWND(std::string PROC, int PathLength) {

		std::unique_lock<std::mutex> l(m);
		size_t charsConverted;
		char* app1 = new char[MAX];

		std::list<struct windows>::iterator iter1 = Wininfo.begin();
		std::list<struct windows>::iterator end1 = Wininfo.end();

		for (; iter1 != end1; iter1++) {
			SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, 0, SPIF_UPDATEINIFILE);

			wcstombs_s(&charsConverted, app1, MAXLENGTH * sizeof(char), iter1->processID.c_str(), PathLength);
			std::string stringa = app1;

			if (!stringa.compare(PROC)) {

				HWND hApp = iter1->hWnd;


				HWND ActiveNow = GetForegroundWindow();

				if (ActiveNow == hApp) return NULL;

				DWORD dwThreadID = GetWindowThreadProcessId(hApp, NULL);
				AttachThreadInput(
					GetWindowThreadProcessId(GetForegroundWindow(), NULL),
					GetCurrentThreadId(), TRUE
					);

				SetWindowPos(hApp, HWND_TOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);
				SetWindowPos(hApp, HWND_NOTOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);

				ShowWindow( hApp, SW_MAXIMIZE);

				SetForegroundWindow(hApp);
				SetActiveWindow(hApp);
				SetFocus(hApp);

				AttachThreadInput(dwThreadID, GetCurrentThreadId(), false);


			}
		}
		return NULL;
	}


	Mylist () { }

	int length() {
		std::unique_lock<std::mutex> l(m);
		return Wininfo.size();
	}
	void push_back(struct windows Element) {
		std::unique_lock<std::mutex> l(m);
		Element.active = false;
		Wininfo.push_back(Element);
	}


	void StampaLista();

	void precSet(int prec) {
		std::unique_lock<std::mutex> l(m);
		this->prec = prec;
	}

	int precGet() {
		std::unique_lock<std::mutex> l(m);
		return this->prec;
	}

	void ActiveOneSet(HWND ActiveOne) {
		std::unique_lock<std::mutex> l(m);

		std::list<struct windows>::iterator iter = Wininfo.begin();
		std::list<struct windows>::iterator end = Wininfo.end();

		for (; iter != end; iter++) {
			if (ActiveOne == iter->hWnd) {
				iter->active = true;
				return;
			}
		}
	}

	void clear() {
		std::unique_lock<std::mutex> l(m);
		Wininfo.clear();
	}

	void sendList(int mysocket, std::shared_ptr<MyServer> server);

	void sendCreated(int mysocket, std::shared_ptr<MyServer> server, struct windows);

	void sendDestroyed(int mysocket, std::shared_ptr<MyServer> server, HWND destroyed, HWND focus);


};

