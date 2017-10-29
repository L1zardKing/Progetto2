#include "MyServerClass.h"


int inviaFile(int s1, std::string path, int byteRimanenti);

void MyServer::sendSize(int s1, int size) {
	std::unique_lock<std::mutex> U(sending);
	send(s1, (char*)&size, sizeof(uint32_t), 0);
}

void MyServer::SerializeSend(Message msg, int s1) {

	std::unique_lock<std::mutex> U(sending);
	struct stat buf;
	char* app = new char[MAXLENGTH];

	size_t charsConverted;
	size_t dimensione;

	// Invio HWND 

	dimensione = (msg.hwnd.size());
	send(s1, (char*)&dimensione, 4, 0);
	wcstombs_s(&charsConverted, app, MAXLENGTH * sizeof(char), msg.hwnd.c_str(), msg.hwnd.length());
	send(s1, app, dimensione + 1, 0);


	// Invio nome applicazione
	dimensione = (msg.appName.size());
	send(s1, (char*) &dimensione, 4, 0);
	wcstombs_s(&charsConverted, app, MAXLENGTH * sizeof(char), msg.appName.c_str(), msg.appName.length());
	send(s1, app, dimensione +1 , 0);

	
	// Invio process path
	dimensione = (msg.processPath.size());
	send(s1, (char*) &dimensione, 4, 0);
	wcstombs_s(&charsConverted, app, MAXLENGTH * sizeof(char), msg.processPath.c_str(), msg.processPath.length());
	send(s1, app, dimensione +1, 0);

	// Invio file
	
	std::wstring filePath = msg.filePath;

	if (filePath == L"") {
		filePath = std::wstring(L"C:\\Users\\Kira\\IconNotFound.png");
	}

	wcstombs_s(&charsConverted, app, MAXLENGTH * sizeof(char), filePath.c_str(), filePath.length());
	stat(app, &buf);
	dimensione = buf.st_size;
	send(s1, (char*)&dimensione, 4, 0);
	std::cout << "Dimensione file" << dimensione << std::endl;
	
	if (inviaFile(s1, app, dimensione) == -1) {
		std::wcout << "PROBLEMI INVIO ICONA" << std::endl;
	}
	
	
	const char * a = (const char*)&msg.active;
	dimensione = sizeof(msg.active);
	send(s1, a, 1, 0);

}


void MyServer::sendChangeName(int s1, HWND hwnd, std::wstring name)
{
	std::unique_lock<std::mutex> U(sending);

	char* app = new char[MAXLENGTH];
	size_t charsConverted;
	size_t dimensione;

	std::wstringstream ss;
	ss << hwnd;
	std::wstring messageD = ss.str().c_str();

	// Invio HWND 
	dimensione = messageD.length();
	send(s1, (char*)&dimensione, 4, 0);
	wcstombs_s(&charsConverted, app, MAXLENGTH * sizeof(char), messageD.c_str(), messageD.length());
	send(s1, app, dimensione + 1, 0);


	// Invio nome applicazione
	dimensione = name.length();
	send(s1, (char*)&dimensione, 4, 0);
	wcstombs_s(&charsConverted, app, MAXLENGTH * sizeof(char), name.c_str(), name.length());
	send(s1, app, dimensione + 1, 0);
	

}


void MyServer::sendInt(const int toSend, int s1) {
	std::unique_lock<std::mutex> U(sending);
	send(s1, (char*)&toSend, 4, 0);
}

void MyServer::sendDestroyed(int s1, HWND destroyed, HWND focus)
{
	std::unique_lock<std::mutex> U(sending);
	char* app = new char[MAXLENGTH];
	size_t charsConverted;

	// Send the HWND of the one to destroy

	std::wstringstream ss;
	ss << destroyed;
	std::wstring messageD = ss.str().c_str();

	size_t dimensione = (messageD.size());
	send(s1, (char*)&dimensione, 4, 0);
	wcstombs_s(&charsConverted, app, MAXLENGTH * sizeof(char), messageD.c_str(), messageD.length());
	send(s1, app, dimensione + 1, 0);

	/*Send the HWND of the one in focus


	std::wstringstream ss1;
	ss1 << focus;
	std::wstring messageD1 = ss1.str().c_str();

	dimensione = (messageD1.size());
	send(s1, (char*)&dimensione, sizeof(uint32_t), 0);
	wcstombs_s(&charsConverted, app, MAXLENGTH * sizeof(char), messageD1.c_str(), messageD1.length());
	send(s1, app, dimensione * sizeof(char) + 1, 0);


	*/

}

void MyServer::sendFocus(int s1, HWND focus) {

	std::unique_lock<std::mutex> U(sending);
	char* app = new char[MAXLENGTH];
	size_t charsConverted;

	// Send the HWND of the one to destroy

	std::wstringstream ss;
	ss << focus;
	std::wstring messageD = ss.str().c_str();

	size_t dimensione = (messageD.size());
	//dimensione = 8;
	send(s1, (char*)&dimensione, 4, 0);
	wcstombs_s(&charsConverted, app, MAXLENGTH * sizeof(char), messageD.c_str(), messageD.length());
	send(s1, app, dimensione + 1, 0);


}
int inviaFile(int s1, std::string path, int byteRimanenti) {
	char x[MAXLENGTH];
	int lettura;
	FILE* fp;
	int inviati = 0;

	fp = fopen(path.c_str(), "rb");

	if (fp == NULL) {
		return -1;
	}


	while (byteRimanenti > 0) {

		if (byteRimanenti >= MAXLENGTH)
		{
			lettura = fread(x, sizeof(char), MAXLENGTH, fp);
			if (lettura != MAXLENGTH)
			{
				perror("Errore nella lettura del file");
				std::wcout << " -- " << lettura << std::endl;
				fclose(fp);
				return -1;
			}
			send(s1, x, MAXLENGTH * sizeof(char), 0);
			inviati += MAXLENGTH;
			byteRimanenti -= MAXLENGTH;
		}
		else
		{
			lettura = fread(x, sizeof(char), byteRimanenti, fp);
			if (lettura != byteRimanenti)
			{
				printf("Errore nella lettura del file %d %d %s", lettura, byteRimanenti, x);
				fclose(fp);
				return -1;
			}

			send(s1, x, byteRimanenti * sizeof(char), 0);
			inviati += byteRimanenti;
			byteRimanenti = 0;
		}
	}

	fclose(fp);
	std::cout << "Byte inviati " << inviati << std::endl;
	return 0;
}

