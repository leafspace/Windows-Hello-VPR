#pragma once

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <windows.h>
#include "SocketClient.h"
#include "MessageQueue.h"
using namespace std;

class LogSystem
{
private:
    int semaphore;
    string filePath;
    filebuf *pFilebuf;
    bool sendFileFlag;
    SocketClient socketClient;
    MessageQueue messageQueue;

    DWORD WINAPI run(LPVOID lpParameter) {
        while (true) {
            if (this->sendFileFlag) {
                ifstream in;
                in.open(this->filePath.data(), ios::binary);
                this->pFilebuf = in.rdbuf();
                int fileSize = this->pFilebuf->pubseekoff(0, ios::end, ios::in);
                this->pFilebuf->pubseekpos(0, ios::in);
                
                int bufferSize = this->socketClient.getBufferSize();
                char *bufferPool = new char[bufferSize];
                while (fileSize > 0) {
                    if (fileSize >= bufferSize) {
                        this->pFilebuf->sgetn(bufferPool, bufferSize);
                        fileSize -= bufferSize;
                    } else {
                        this->pFilebuf->sgetn(bufferPool, fileSize);
                        fileSize -= fileSize;
                    }
                    
                    this->socketClient.sendMessage(bufferPool);
                }

                in.close();
                delete bufferPool;

                this->sendFileFlag = false;
            } else {
                while (this->messageQueue.getListSize() > 0) {
                    string message = this->messageQueue.popMessage();
                    this->socketClient.sendMessage(message);
                }
            }
        }
        return 0;
    }
public:
    LogSystem() {
        this->socketClient.setServerIP("10.16.16.114");
        this->sendFileFlag = false;
    }

    bool initSocket() {
        bool isSuccess = this->socketClient.pingTest();
        if (!isSuccess) {
            return false;
        }

        isSuccess = this->socketClient.linkTest();
        if (!isSuccess) {
            return false;
        }

        isSuccess = this->socketClient.initSocket();
        if (!isSuccess) {
            return false;
        }
        return true;
    }

    void stopSocket() {
        this->socketClient.freeResourse();
    }

    void sendFile(string filePath) {
        this->filePath = filePath;
        this->sendFileFlag = true;
    }

    bool sendMessage(string message) {
        if (messageQueue.getLength() >= messageQueue.getListSize()) {
            return false;
        } else {
            messageQueue.pushMessage(message);
        }
    }
};