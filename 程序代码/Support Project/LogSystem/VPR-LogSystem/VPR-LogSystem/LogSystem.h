#pragma once
#pragma comment(lib, "pthreadVC2.lib")

#include <io.h>
#include <time.h>
#include <cstdlib>
#include <fstream>
#include <direct.h>
#include <iostream>
#include <pthread.h>
#include <windows.h>
#include "ReadConfig.h"
#include "SocketClient.h"
#include "MessageQueue.h"
using namespace std;

void* threadRun(void* arg);

class LogSystem
{
private:
	pthread_t threadID;                                                      //线程ID
public:
    string filePath;                                                         //发送文件的路径
    filebuf *pFilebuf;                                                       //发送文件的缓冲
    bool sendFileFlag;                                                       //用于判断当前发送的是文件还是单行信息
    SocketClient socketClient;                                               //用于socket的基本操作
    MessageQueue messageQueue;                                               //发送信息的操作列表

public:
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
		pthread_create(&threadID, NULL, threadRun, NULL);
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
        if(_access(".//logs", 0) == -1) {                                    //判断logs文件夹是否存在
            if (mkdir(".//logs") == -1) {                                    //创建logs文件夹
                return false;
            }
        }

        time_t date = time(0);
        char nowTime[64];
        strftime(nowTime, sizeof(nowTime), "%Y%m%d", localtime(&date));      //获取当前时间
        strcat_s(nowTime, ".txt");

        ofstream out;
        out.open(nowTime, ios::app);                                         //追加写入到文件
        if (!out.is_open()) {
            return false;
        }
        out.seekp(0, ios::beg);                                              //移动到文件首
        out << message.data() << endl;                                       //从文件头写入
        out.close();
        return true;
    }
};

extern LogSystem *p_logSystem;

void* threadRun(void* arg)
{
	while (true) {
        if (p_logSystem->sendFileFlag) {                                     //当前发送的是文件
            ifstream in;
            in.open(p_logSystem->filePath.data(), ios::binary);              //以二进制形式打开文件
            p_logSystem->pFilebuf = in.rdbuf();                              //获取文件缓冲的区
            int fileSize = p_logSystem->pFilebuf->pubseekoff(0, ios::end, ios::in);    //获取发送文件的大小
            p_logSystem->pFilebuf->pubseekpos(0, ios::in);                   //移动指针到文件开头
                
            int bufferSize = p_logSystem->socketClient.getBufferSize();      //获取socket缓冲区的大小
            char *bufferPool = new char[bufferSize];                         //新建文件缓冲区
            while (fileSize > 0) {                                           //只要文件大小还在，就一直发送
                if (fileSize >= bufferSize) {
                    p_logSystem->pFilebuf->sgetn(bufferPool, bufferSize);    //获取缓冲数据
                    fileSize -= bufferSize;
                } else {                                                     //假如有不满足缓冲区大小的部分
                    p_logSystem->pFilebuf->sgetn(bufferPool, fileSize);
                    fileSize -= fileSize;
                }
                    
                p_logSystem->socketClient.sendMessage(bufferPool);           //发送缓冲区的数据
            }

            in.close();                                                      //关闭文件流
            delete bufferPool;                                               //清除缓冲区

            p_logSystem->sendFileFlag = false;
        } else {                                                             //当前发送的是单行信息
            while (p_logSystem->messageQueue.getListSize() > 0) {            //只要消息列表中还存在消息，就一直发送
                string message = p_logSystem->messageQueue.popMessage();     //消息列表弹出消息
                p_logSystem->socketClient.sendMessage(message);              //利用socket发送消息
            }
        }
    }
	return 0;
}