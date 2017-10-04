#pragma once

#include <io.h>
#include <time.h>
#include <cstdlib>
#include <fstream>
#include <direct.h>
#include <iostream>
#include <windows.h>
#include "ReadConfig.h"
#include "SocketClient.h"
#include "MessageQueue.h"
using namespace std;

DWORD WINAPI messageThread(LPVOID lpThreadParameter);

class LogSystem
{
private:
	HANDLE threadID;                                                         //线程ID
public:
    string filePath;                                                         //发送文件的路径
    filebuf *pFilebuf;                                                       //发送文件的缓冲
    bool sendFileFlag;                                                       //用于判断当前发送的是文件还是单行信息
    SocketClient socketClient;                                               //用于socket的基本操作
    MessageQueue messageQueue;                                               //发送信息的操作列表

public:
	bool linkState;

    LogSystem() {
		ReadConfig *readConfig = new ReadConfig;                             //打开文件读取
		bool isSuccess = readConfig->ReadFile();                             //读取文件
		if (isSuccess) {
			this->socketClient.setServerIP(readConfig->getServerIP());       //设置serverIP
		}
        this->sendFileFlag = false;
		delete readConfig;
    }

    bool initSocket() {
        bool isSuccess = this->socketClient.pingTest();                      //测试与目标主机是否连接着
        if (!isSuccess) {
            return false;
        }

		isSuccess = this->socketClient.initSocket();                         //初始化socket
        if (!isSuccess) {
            return false;
        }

        isSuccess = this->socketClient.linkTest();                           //测试目标的socket是否打开着
        if (!isSuccess) {
            return false;
        }
        return true;
    }

	void beginSystem() {
		this->threadID = CreateThread(NULL, 0, messageThread, NULL, 0, NULL);
	}

    void stopSocket() {
        this->socketClient.freeResourse();
    }

    void sendFile(string filePath) {
        this->filePath = filePath;
        this->sendFileFlag = true;
    }

    bool sendMessage(string message) {                                       //把消息压入消息队列
        if (messageQueue.getLength() >= messageQueue.getListSize()) {
            return false;
        } else {
            messageQueue.pushMessage(message);
			return true;
        }
    }

    bool writeMessage(string message) {
        if(_access("logs", 0) == -1) {                                       //判断logs文件夹是否存在
            if (_mkdir("logs") == -1) {                                      //创建logs文件夹
                return false;
            }
        }

        time_t date = time(0);
		char nowTime[64];
        char filePath[256];

		_getcwd(filePath, 256);
		strcat_s(filePath, "\\logs\\");
        strftime(nowTime, sizeof(nowTime), "%Y%m%d", localtime(&date));      //获取当前时间
        strcat_s(nowTime, ".txt");
		strcat_s(filePath, nowTime);

        ofstream out;
        out.open(filePath, ios::app);                                        //追加写入到文件
        if (!out.is_open()) {
            return false;
        }
        out.seekp(0, ios::beg);                                              //移动到文件首
		sprintf(nowTime, "[%d:%d:%d] ", localtime(&date)->tm_hour, 
			localtime(&date)->tm_min, localtime(&date)->tm_sec);
        out << nowTime << message.data();                                    //从文件头写入
        out.close();
        return true;
    }
};

extern LogSystem *p_logSystem;
