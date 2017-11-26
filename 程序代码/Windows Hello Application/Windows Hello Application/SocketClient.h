#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <io.h>
#include <string>
#include <fstream>
#include <iostream>
#include <direct.h>
#include <winsock.h>
using namespace std;

class SocketClient
{
private:
	int PORT;
	string serverIP;
	int bufferSize;
	char* bufferPool;
	WSADATA wsaData;
	SOCKET client;
	struct sockaddr_in server;                                               //服务器端地址

public:
	SocketClient(string serverIP = "127.0.0.1", int PORT = 5000) {
		this->serverIP = serverIP;
		this->PORT = PORT;
		this->bufferSize = 1024;
		this->bufferPool = new char[this->bufferSize];
	}

	void setServerIP(string serverIP);
	void setPORT(int PORT);
	int getBufferSize();
	bool pingTest();
	bool linkTest();
	bool initSocket();
	bool sendMessage(string message);
	bool sendMessage(string message, int messageLen);
	string recvMessage();
	void freeResourse();
};

