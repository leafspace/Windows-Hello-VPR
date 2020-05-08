#include "../stdafx.h"
#include "LogSystem.h"

void* messageThread(void* arg)
{
	bool isQuit = false;
	while (true) {
		if (g_pLogSystem->sendFileFlag) {                                     //当前发送的是文件
			if ((_access(g_pLogSystem->filePath.data(), 0)) == -1) {          //如果文件不存在
				g_pLogSystem->sendFileFlag = false;
				break;
			}

			char* filePath = new char[256];
			ReadConfig *readConfig = new ReadConfig;                         //打开文件读取
			bool isSuccess = readConfig->ReadFile();                         //读取文件
			if (isSuccess) {
				strcpy(filePath, readConfig->getPath().data());
			}
			delete readConfig;

			char* fileName = new char[256];
			time_t date = time(0);
			sprintf(fileName, "%d%d%d%d%d%d.wav", localtime(&date)->tm_year,
				localtime(&date)->tm_mon, localtime(&date)->tm_yday,
				localtime(&date)->tm_hour, localtime(&date)->tm_min,
				localtime(&date)->tm_sec);
			strcat(filePath, fileName);
			delete fileName;
			cout << filePath << endl;

			char tempStr[256];
			int index = 0;
			for (int i = 0; filePath[i] != 0; ++i) {
				tempStr[index++] = filePath[i];
				if (filePath[i] == '\\') {
					tempStr[index++] = '\\';
				}
			}
			tempStr[index] = 0;

			CopyFileA(g_pLogSystem->filePath.data(), tempStr, FALSE);
			g_pLogSystem->socketClient.sendMessage(filePath);                 //利用socket发送文件路径
			delete filePath;

			/*
			//Socket 发送方法 （未成功）
			ifstream in;
			in.open(g_pLogSystem->filePath.data(), fstream::in | ios::binary);//以二进制形式打开文件
			g_pLogSystem->pFilebuf = in.rdbuf();                              //获取文件缓冲的区
			int fileSize = g_pLogSystem->pFilebuf->pubseekoff(0, ios::end, ios::in);    //获取发送文件的大小
			g_pLogSystem->pFilebuf->pubseekpos(0, ios::in);                   //移动指针到文件开头

			int bufferSize = g_pLogSystem->socketClient.getBufferSize();      //获取socket缓冲区的大小
			char *bufferPool = new char[bufferSize];                         //新建文件缓冲区

			sprintf(bufferPool, "%d\n", fileSize);
			g_pLogSystem->socketClient.sendMessage(bufferPool);               //发送文件大小

			while (fileSize > 0) {                                           //只要文件大小还在，就一直发送
				int sendSize;
				if (fileSize >= bufferSize) {
					sendSize = bufferSize;
				} else {                                                     //假如有不满足缓冲区大小的部分
					sendSize = fileSize;
				}
				g_pLogSystem->pFilebuf->sgetn(bufferPool, sendSize);          //获取缓冲数据
				fileSize -= sendSize;
				g_pLogSystem->socketClient.sendMessage(bufferPool, sendSize); //发送缓冲区的数据
			}

			in.close();                                                      //关闭文件流
			delete bufferPool;                                               //清除缓冲区
			*/
			g_pLogSystem->sendFileFlag = false;
			break;                                                           //发完文件会话就算结束
		}
		else {                                                               //当前发送的是单行信息
			while (g_pLogSystem->messageQueue.getLength() > 0) {              //只要消息列表中还存在消息，就一直发送
				string message = g_pLogSystem->messageQueue.popMessage();     //消息列表弹出消息
				g_pLogSystem->socketClient.sendMessage(message);              //利用socket发送消息
				if (message == "<File>\n") {
					break;
				}

				if (message == "<Finish>\n") {
					isQuit = true;
				}
			}
		}

		if (isQuit) {
			break;
		}
	}
	g_pLogSystem->stopSocket();
	return 0;
}