#include "ReadConfig.h"

int main()
{
	ReadConfig *readConfig = new ReadConfig;
	bool isSuccess = readConfig->ReadFile();
	if (isSuccess) {
		cout << readConfig->getServerIP() << endl;
		cout << readConfig->getPassword() << endl;
	}

	delete readConfig;
	return 0;
}