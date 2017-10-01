#include "LogSystem.h"
LogSystem *p_logSystem;

int main()
{
	/*
	LogSystem logSystem;
	if (logSystem.initSocket()) {
		cout << "Info : Socket begin linked !" << endl;
	}

	p_logSystem = &logSystem;
	logSystem.beginSystem();
	logSystem.sendMessage("<Type>");
	logSystem.sendMessage("Windows Hello");
	for (int i = 0; i < 25; i++) {
		logSystem.sendMessage("<Message>");
		logSystem.sendMessage("information");
		logSystem.writeMessage("information");
	}
	//logSystem.sendMessage("<File>");
	//logSystem.sendFile("mary.wav");

	logSystem.sendMessage("<Finish>");
	*/

	/*
	SocketClient socketClient;
	socketClient.setServerIP("10.16.16.114");
	if (socketClient.pingTest()) {
		if (socketClient.initSocket()) {
			if(socketClient.linkTest()) {
				socketClient.sendMessage("<Type>\n");
				socketClient.sendMessage("Windows Hello Client\n");
				for (int i = 0; i < 25; i++) {
					socketClient.sendMessage("<Message>\n");
					socketClient.sendMessage("information\n");
				}
				socketClient.sendMessage("<Message>\n");
				socketClient.sendMessage("ERROR\n");
				socketClient.sendMessage("<Finish>\n");
			}
		}
	}
	socketClient.freeResourse();
	*/

	LogSystem logSystem;
	if (!logSystem.initSocket()) {
		return -1;
	}
	p_logSystem = &logSystem;
	logSystem.beginSystem();
	logSystem.sendMessage("<Type>\n");
	logSystem.sendMessage("Windows Hello Client\n");
	for (int i = 0; i < 25; i++) {
		logSystem.sendMessage("<Message>\n");
		logSystem.sendMessage("information\n");
		logSystem.writeMessage("information\n");
	}
	logSystem.sendMessage("<File>\n");
	logSystem.sendFile("mary.wav");
	logSystem.sendMessage("<Finish>\n");
	while(true) {
	}
	return 0;
}