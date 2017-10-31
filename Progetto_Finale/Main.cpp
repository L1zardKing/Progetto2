#pragma once

#include "MainHeader.h"
#include <boost/asio.hpp>
#include <shellapi.h>


#define max_length 512
#define PORT 1500

void receiveNotification(void);


Mylist WinInfo;
Mylist WinInfo1;

int s1;
std::atomic<bool> ListAltreadySent;
std::shared_ptr<MyServer> server;

void main(void)
{

	struct sockaddr_in saddr, caddr;
	int addrlen;
	int s;
	int porta;

	server = std::make_shared<MyServer>();
	ListAltreadySent.store(false);
	::EnumWindows(enumWindowsProc, NULL);


	HWND activeWindow = GetForegroundWindow();
	if (activeWindow == NULL) {
		std::cout << "Problem retrieving active window Main" << std::endl;
		//exit(1);
	}

	WinInfo.ActiveOneSet(activeWindow);
	//enumWindowsProcA(__in  activeWindow, __in  NULL);

	std::cout << "sizeof (char) " << sizeof(char) << std::endl;
	WinInfo.StampaLista();
	WinInfo.precSet(WinInfo.length());

	/*
	Il notification thread si occupa di ricevere notifiche dal sistema e di inviare la lista tramite il metodo sendList
	Il KeyStroke si occupa di ricevere notifiche dal main thread (che ascolta sul socket) e di inviare i dati all'applicazione
	*/

	std::shared_ptr<SendKeyClass> KClass = std::make_shared<SendKeyClass>();
	std::thread KeyStrokeThread = std::thread(&SendKeyClass::SendKeyWrapper, KClass);
	std::thread NotificationThread = std::thread(receiveNotification);


	porta = htons(PORT);

	s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == -1)
	{
		printf("Errore creazione socket\nExiting...\n");
		exit(-1);
	}

	saddr.sin_addr.s_addr = INADDR_ANY;
	saddr.sin_port = porta;
	saddr.sin_family = AF_INET;

	if (bind(s, (struct sockaddr *) &saddr, sizeof(saddr)) == -1)
	{
		perror("Bind fallita!");
		exit(-1);
	}

	if (listen(s, max_length) == -1) {
		perror("Listen fallita");
		exit(-1);
	}

	addrlen = sizeof(struct sockaddr_in);

	while (1)
	{
		s1 = accept(s, (struct sockaddr *) &caddr, &addrlen);
		if (s1 == -1)
		{
			printf("Errore nell'accept \n");
			continue;
		}
		WinInfo.clear();
		::EnumWindows(enumWindowsProc, NULL);

		HWND activeWindow = GetForegroundWindow();
		if (activeWindow == NULL) {
			std::cout << "Problem retrieving active window main 88" << std::endl;
			//exit(1);
		}

		WinInfo.ActiveOneSet(activeWindow);

		WinInfo.sendList(s1, server); // Invio i dati per la prima volta
		ListAltreadySent.store(true);

		//receiveNotification();

		int NetInt;
		size_t charsConverted;
		int PathLength;
		HWND hwnd = NULL;
		char* PATH = new char[max_length];
		char* app = new char[max_length];


		while (1) {
			try {
				
				// Ricevo lunghezza path e path
				if (recv(s1, (char *)&PathLength, sizeof(long), 0) <= 0) break;
				if (recv(s1, PATH, PathLength, 0) <= 0) break;
				PATH[PathLength] = '\0';

				std::cout << "Path ricevuto" << PATH << std::endl;

				/*
				::EnumWindows(enumWindowsProcA, NULL);
				*/
				
				
				
				if (recv(s1, (char *)&NetInt, sizeof(long), 0) <= 0) break;
				//std::cout << "Ricevuto : " << NetInt << std::endl;
				if (recv(s1, app, NetInt, 0) <= 0) break;
				app[NetInt] = '\0';
				//std::cout << "Ricevuto : " << app << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(3)); // per dare il tempo di cambiare quando usi un solo pc, 
				WinInfo.returnHWND(PATH, PathLength);
				KClass->PrepareKey(app);

			}
			catch (std::exception& e) {
				std::cerr << "ERRORE : " << e.what() << std::endl;
				break;
			}

		}

		closesocket(s1);
		WinInfo.StampaLista();
		ListAltreadySent.store(false);
		WinInfo.clear();
	}

	NotificationThread.join();
	KeyStrokeThread.join();
}


void receiveNotification() {

	int ret;
	MSG msg;


	HWINEVENTHOOK hEvent = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL,
		WinEventProcCallback, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

	HWINEVENTHOOK hWinEventHook0 = SetWinEventHook(
		EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE,
		NULL, WinEventProcCallback, 0, 0,
		WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

	HWINEVENTHOOK hWinEventHook1 = SetWinEventHook(
		EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY,
		NULL, WinEventProcCallback, 0, 0,
		WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

	HWINEVENTHOOK hWinEventHook2 = SetWinEventHook(
		EVENT_OBJECT_NAMECHANGE, EVENT_OBJECT_NAMECHANGE,
		NULL, WinEventProcCallback, 0, 0,
		WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

	HWINEVENTHOOK hWinEventHook3 = SetWinEventHook(
		EVENT_OBJECT_FOCUS, EVENT_OBJECT_FOCUS,
		NULL, WinEventProcCallback, 0, 0,
		WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

	while (ret = (GetMessage(&msg, NULL, 0, 0))) {
		if (ret == -1)
		{
			return;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWinEvent(hEvent);
	UnhookWinEvent(hWinEventHook0);
	UnhookWinEvent(hWinEventHook1);

}
