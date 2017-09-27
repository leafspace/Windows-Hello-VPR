#include <iostream>  
#include <fstream>
#include <string>
using namespace std;

class ReadConfig
{
private:
	string serverIP;
	string password;

public:
	ReadConfig() {
		this->serverIP = "192.168.1.1";
		this->password = "123456";
	}

	bool ReadFile() {
		ifstream file;
		file.open("info.conf", ios::in);
		
		if(!file.is_open()) {
			return false;
		}
		
		string strLine;  
		while(getline(file, strLine)) {
			if(strLine.empty()) {
				continue;
			}
			this->resolution(strLine);
		}
		return true;
	}

	void resolution(string strLine) {
		int labelBegin = strLine.find('<', 0);
		int labelEnd = strLine.find('>', 0);
		string label = strLine.substr(labelBegin + 1, labelEnd - labelBegin - 1);
		int infoBegin = labelEnd;
		int infoEnd = strLine.find('<', infoBegin);
		string info = strLine.substr(infoBegin + 1, infoEnd - infoBegin - 1);

		if (label == "server") {
			this->serverIP = info;
		} else if (label == "password") {
			this->password = info;
		}
	}

	string getServerIP() {
		return this->serverIP;
	}

	string getPassword() {
		return this->password;
	}
};