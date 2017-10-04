#include "stdafx.h"
#include "SocketClient.h"

void SocketClient::setServerIP(string serverIP) 
{
    this->serverIP = serverIP;
}

void SocketClient::setPORT(int PORT) 
{
    this->PORT = PORT;
}

int SocketClient::getBufferSize() 
{
    return this->bufferSize;
}

bool SocketClient::pingTest()                                                //ping目标主机，看与目标主机是否连接
{
	if(_access("temp", 0) == -1) {                                       //判断logs文件夹是否存在
        if (_mkdir("temp") == -1) {                                      //创建logs文件夹
            return false;
        }
    }

    string targetIP = this->serverIP;
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

bool SocketClient::linkTest() 
{
    if (connect(this->client, (struct sockaddr*)&this->server, sizeof(this->server)) == INVALID_SOCKET) {
        cout << "ERROR : Socket connect failed !" << endl;
        return false;
    } else {
        return true;
    }
}

bool SocketClient::initSocket() 
{
    memset(this->bufferPool, 0, this->bufferSize);                           //接收缓冲区初始化
    if (WSAStartup(MAKEWORD(2, 2), &this->wsaData) != 0) {
        cout << "ERROR : Winsock load failed !" << endl;
        return false;
    }

    this->server.sin_family = AF_INET;                                       //需要连接的服务器的地址信息
    this->server.sin_addr.s_addr = inet_addr(this->serverIP.data());         //将命令行的IP地址转换为二进制表示的网络字节顺序IP地址
    this->server.sin_port = htons(this->PORT);
        
    this->client = socket(AF_INET, SOCK_STREAM, 0);                          //建立客户端流套接
    if (this->client == INVALID_SOCKET) {
        cout << "ERROR : Socket create failed !" << endl;
        return false;
    }
        
    return true;
}

bool SocketClient::sendMessage(string message)                               //利用socket发送信息
{
    int msgLen;
	if ((msgLen = send(this->client, message.data(), strlen(message.data()), 0)) == SOCKET_ERROR) {
        cout << "ERROR : Socket send message error !" << endl;
        return false;
	}
	return true;
}

bool SocketClient::sendMessage(string message, int messageLen)               //利用socket发送信息
{
    int msgLen;
	if ((msgLen = send(this->client, message.data(), messageLen, 0)) == SOCKET_ERROR) {
        cout << "ERROR : Socket send message error !" << endl;
        return false;
	} else if (msgLen != messageLen) {
		cout << "ERROR : Socket send message has failure byte (" << msgLen << ") !" << endl;
		return false;
	}
	return true;
}

void SocketClient::freeResourse() 
{
    closesocket(this->client);
    WSACleanup();
    delete this->bufferPool;
}