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

class Mylist {
private:
	std::list<struct windows> Wininfo;
	struct windows ActiveOne;
	std::mutex m;
	int prec;

public:
	~Mylist() {	}

	std::list<struct windows> getList() {
		return Wininfo;
	}

	void setList(std::list<struct windows> m1) {
		std::unique_lock<std::mutex> l(m);
		Wininfo = m1;
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

