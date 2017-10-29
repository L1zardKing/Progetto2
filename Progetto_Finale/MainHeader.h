#pragma once

#include "GestioneLista.h"
#include "GestioneEventi.h"
#include "SendKeystrokes.h"
#include "MyServerClass.h"

#include <Windows.h>
#include <thread>
#include <strsafe.h>
#include <string>
#include <future>
#include <memory>

extern int s1;
extern Mylist WinInfo;
extern std::atomic<bool> ListAltreadySent;
extern std::shared_ptr<MyServer> server;
