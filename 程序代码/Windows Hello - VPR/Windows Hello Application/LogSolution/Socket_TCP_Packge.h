#pragma once

#include <iostream>
using namespace std;

class PackgeHead
{
protected:
    string orginIP;                                                          // 源主机地址
    string targetIP;                                                         // 目标主机地址
    
    int packgeNumber;
    int packgeSize;                                                          // 发送整个包大小
    int headSize;
    int dataSize;                                                            // 发送数据包大小
    int state;                                                               // state=-1表示初始化 state=1表示发送 state=2表示响应接收成功 state=4表示接收数据失败（或丢失） state=5表示重新发送
public:
    PackgeHead(string oprinIP) {
        this->orginIP = oprinIP;
        this->targetIP = "";

        this->packgeNumber = 0;
        this->packgeSize = 0;
        this->headSize = 0;
        this->dataSize = 0;
        this->state = -1;
    }

    void setOrginIP(string orginIP);
    void setTargetIP(string targetIP);
    
    void setPackgeNumber(int packgeNumber);
    void setPackgeSize(int packgeSize);
    void setHeadSize(int headSize);
    void setDataSize(int dataSize);

    void setState(int state);

	string getOrginIP();
    string getTargetIP();
    
    int getPackgeNumber();
    int getPackgeSize();
    int getHeadSize();
    int getDataSize();

    int getState();
};

class SocketDataPackge : public PackgeHead
{
private:
    char* dataList;                                                          // 数据内容
    string hashMD5;
public:
    SocketDataPackge(string orginIP) : PackgeHead(orginIP) {
        this->dataList = NULL;
    }

    bool setData(char* data, int dataSize);
    void setHashMD5(string hashMD5);

	char* getData();
    string getHashMD5();
};

class SocketResponsePackge : public PackgeHead
{
private:
    int lostNumber;
public:
    SocketResponsePackge(string orginIP) : PackgeHead(orginIP){
    }

    void setLostNumber(int lostNumber);

	int getLostNumber();
};