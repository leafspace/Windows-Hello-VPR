#include "LogSystem.h"
LogSystem *p_logSystem;

int main()
{
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
	return 0;
}