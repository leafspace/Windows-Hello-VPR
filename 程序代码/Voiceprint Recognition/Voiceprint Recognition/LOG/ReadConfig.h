#pragma once

#include <string>
#include <fstream>
#include <iostream>
using namespace std;

class ReadConfig
{
private:
	string path;
	string serverIP;
	string password;
	string method;
	string threshold;
	string probability;

public:
	ReadConfig() {
		this->serverIP = "127.0.0.1";
		this->password = "123456";
		this->method = "1";
		this->threshold = "5";
		this->probability = "0";
	}

	bool ReadFile() {
		ifstream file;
		file.open("info.conf", ios::in);                                     //读取文件信息

		if (!file.is_open()) {
			return false;
		}

		string strLine;
		while (getline(file, strLine)) {
			if (strLine.empty()) {
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
		}
		else if (label == "password") {
			this->password = info;
		}
		else if (label == "path") {
			this->path = info;
		}
		else if (label == "probability") {
			this->probability = info;
		}
		else if (label == "threshold") {
			this->threshold = info;
		}
		else if (label == "method") {
			this->method = info;
		}
	}

	string getPath() {
		return this->path;
	}

	string getServerIP() {
		return this->serverIP;
	}

	string getPassword() {
		return this->password;
	}

	string getMethod() {
		return this->method;
	}

	string getThreshold() {
		return this->threshold;
	}

	string getProbability() {
		return this->probability;
	}
};