#include "LogSystem.h"

DWORD WINAPI messageThread(LPVOID lpThreadParameter)
{
	bool isQuit = false;
	while (true) {
		if (p_logSystem->sendFileFlag) {                                     //当前发送的是文件
			if ((_access(p_logSystem->filePath.data(), 0)) == -1) {          //如果文件不存在
				p_logSystem->sendFileFlag = false;
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
				localtime(&date)->tm_sec);                                   //格式化文件名
			strcat(filePath, fileName);                                      //连接文件路径
			delete fileName;
			cout << filePath << endl;

			char tempStr[256];
			int index = 0;
			for (int i = 0; filePath[i] != 0; ++i) {
				tempStr[index++] = filePath[i];
				if (filePath[i] == '\\') {
					tempStr[index++] = '\\';                                 //修改格式化路径为双杠
				}
			}
			tempStr[index] = 0;

			CopyFileA(p_logSystem->filePath.data(), tempStr, FALSE);         //复制文件到目标路径
			p_logSystem->socketClient.sendMessage(filePath);                 //利用socket发送文件路径
			delete filePath;

			/*
			//Socket 发送方法 （未成功）
			ifstream in;
			in.open(p_logSystem->filePath.data(), fstream::in | ios::binary);//以二进制形式打开文件
			p_logSystem->pFilebuf = in.rdbuf();                              //获取文件缓冲的区
			int fileSize = p_logSystem->pFilebuf->pubseekoff(0, ios::end, ios::in);    //获取发送文件的大小
			p_logSystem->pFilebuf->pubseekpos(0, ios::in);                   //移动指针到文件开头

			int bufferSize = p_logSystem->socketClient.getBufferSize();      //获取socket缓冲区的大小
			char *bufferPool = new char[bufferSize];                         //新建文件缓冲区

			sprintf(bufferPool, "%d\n", fileSize);
			p_logSystem->socketClient.sendMessage(bufferPool);               //发送文件大小

			while (fileSize > 0) {                                           //只要文件大小还在，就一直发送
				int sendSize;
				if (fileSize >= bufferSize) {
					sendSize = bufferSize;
				} else {                                                     //假如有不满足缓冲区大小的部分
					sendSize = fileSize;
				}
				p_logSystem->pFilebuf->sgetn(bufferPool, sendSize);          //获取缓冲数据
				fileSize -= sendSize;
				p_logSystem->socketClient.sendMessage(bufferPool, sendSize); //发送缓冲区的数据
			}

			in.close();                                                      //关闭文件流
			delete bufferPool;                                               //清除缓冲区
			*/
			p_logSystem->sendFileFlag = false;
			break;                                                           //发完文件会话就算结束
		}
		else {                                                               //当前发送的是单行信息
			while (p_logSystem->messageQueue.getLength() > 0) {              //只要消息列表中还存在消息，就一直发送
				string message = p_logSystem->messageQueue.popMessage();     //消息列表弹出消息
				p_logSystem->socketClient.sendMessage(message);              //利用socket发送消息
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
	p_logSystem->stopSocket();
	return 0;
}