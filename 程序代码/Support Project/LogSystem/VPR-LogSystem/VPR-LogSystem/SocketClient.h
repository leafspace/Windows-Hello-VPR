#pragma  comment(lib, "ws2_32.lib")

#include <fstream>
#include <iostream>
#include <winsock.h>
#include <stdio.h>

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

    bool pingTest(string targetIP) {
        char fileName[256] = "temp//";
        char cmdstr[256] = "";
        strcat_s(fileName, targetIP.data());
        strcat_s(fileName, ".txt");
    
        strcat_s(cmdstr, "cmd /c ping ");
        strcat_s(cmdstr, targetIP.data());
        strcat_s(cmdstr, " -n 1 -w 500 >");
        strcat_s(cmdstr, fileName);
    
        WinExec(cmdstr, SW_HIDE);
        Sleep(500);
    
        ifstream inStream(fileName, ios::binary);
        if (!inStream.is_open()) {
            return false;
        }
    
        char buffer[512];
        inStream.getline(buffer, 512);
        inStream.getline(buffer, 512);
        inStream.getline(buffer, 512);
    
        if (strcmp(buffer, "请求超时。") == 0) {
            return false;
        } else {
            return true;
        }
    }

    bool linkTest() {
        if (connect(this->client, (struct sockaddr*)&this->server, sizeof(this->server)) == INVALID_SOCKET) {
            cout << "ERROR : Socket connect failed !" << endl;
            return false;
        } else {
            return true;
        }
    }

    bool initSocket() {
        memset(this->bufferPool, 0, this->bufferSize);                       //接收缓冲区初始化
        if (WSAStartup(MAKEWORD(2, 2), &this->wsaData) != 0) {
            cout << "ERROR : Winsock load failed !" << endl;
            return false;
        }
        
        this->server.sin_family = AF_INET;                                   //需要连接的服务器的地址信息
        this->server.sin_addr.s_addr = inet_addr(this->serverIP.data());     //将命令行的IP地址转换为二进制表示的网络字节顺序IP地址
        this->server.sin_port = htons(this->PORT);
        
        this->client = socket(AF_INET, SOCK_STREAM, 0);                      //建立客户端流套接
        if (this->client == INVALID_SOCKET) {
            cout << "ERROR : Socket create failed !" << endl;
            return false;
        }
        
        return true;
    }

    bool sendMessage(string message) {
        int iLen;
        int msgLen;
		if ((msgLen = send(this->client, message.data(), strlen(message.data()), 0)) == SOCKET_ERROR) {
            cout << "ERROR : Socket send message error !";
            return false;
		}
    }

    void freeResourse() {
        closesocket(this->client);
        WSACleanup();
        delete this->bufferPool;
    }
};
