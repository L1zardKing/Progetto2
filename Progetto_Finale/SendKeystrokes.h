#pragma once
#include <mutex>

void SendKeyStrokes(std::vector<std::string> , int);

typedef struct KEY {
	std::vector<std::string> keys;
	int numKeys;
} KEYS;

class SendKeyClass {
private:
	std::mutex m;
	std::condition_variable cv;
	std::condition_variable cv1;
	// considerare l'opzione di una lista per non perdere situazioni in cui arrivano più input consecutivi
	bool keyReady = true;
	std::list<KEYS>* keysToSend;

public:
	SendKeyClass() {
	
		keysToSend = new std::list<KEYS>();
	}

	void SendKeyWrapper() {

		while (1) SendKey();
	}
	
	void SendKey() {
		std::unique_lock<std::mutex> L(m);
		keyReady = true;
		cv1.notify_one();
		cv.wait(L, [&]() {
			return !keysToSend->empty();
		});
		// send key
		KEYS key = keysToSend->back();
		keysToSend->pop_back();
		SendKeyStrokes(key.keys, key.numKeys);
		keyReady = false;
		
	}

	void PrepareKey(char* rec) {

		std::unique_lock<std::mutex> L(m);
		std::vector<std::string>* keys = new std::vector<std::string>();
		KEYS chiaviDaInviare;


		char** app;;
		int NKey, j = 0;
		NKey = strlen(rec) / 3;
		
		if (keyReady == false) cv1.wait(L, [&]() {
			return !keyReady;
		});
		

		keyReady = false;
		
		app = (char**)malloc(NKey * sizeof(char*));
		for (int i = 0; i < NKey; i++) {
			app[i] = (char*)malloc(4 * sizeof(char));
		}

		chiaviDaInviare.numKeys = NKey;

		for (int i = 0; i < strlen(rec); i += 3) {

			app[j][0] = rec[i];
			app[j][1] = rec[i + 1];
			app[j][2] = rec[i + 2];
			app[j][3] = '\0';

			keys->push_back(app[j]);
			j++;


		}

		chiaviDaInviare.keys = *keys;

		keysToSend->push_front(chiaviDaInviare);

		//SendKeyStrokes(app, NKey);

		for (int i = 0; i < NKey; i++) {
			free(app[i]);
		}

		free(app);
		//load key 
		cv.notify_one();
	}


};
