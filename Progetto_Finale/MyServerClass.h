#pragma once
#include "Message.h"
#include "ReceivedMessage.h"
#include <memory>
#include <mutex>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/archive/text_iarchive.hpp>

#define MAXLENGTH 1024
	
class MyServer {

private:
	std::mutex sending;
	std::mutex receiving;

public:
	MyServer () { }

	~MyServer() { }

	void SerializeSend(Message msg, int s1); 
	void sendSize(int s1, int size);
	//void SerializeRead(int s1);
	void sendChar(const char* toSend, int s1);
	void sendChangeName(int s1, HWND hwnd, std::wstring name);
	void sendInt(const int toSend, int s1);
	void sendDestroyed(int s1, HWND destroyed, HWND focus);
	void sendFocus(int s1, HWND focus);
};
