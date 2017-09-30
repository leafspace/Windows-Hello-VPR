#pragma once

#include <string>
#include <iostream>
using namespace std;

class MessageQueue
{
private:
	string messageList[20];
	int head;
	int tail;
	int messageSize;

public:
	MessageQueue() {
		this->head = 0;
		this->tail = 0;
		this->messageSize = 0;
	}

	bool pushMessage(string message) {
		if ((this->tail + 1) % 20 == this->head) {
			cout << "ERROR : Message queue is full !" << endl;
			return false;
		} else {
			this->tail = (this->tail + 1) % 20;
			this->messageList[this->tail];
			this->messageSize++;
		}
		return true;
	}

	string popMessage() {
		if (this->tail == this->head) {
			return "";
		} else {
			this->tail = (this->tail + 19) % 20;
			this->messageSize--;
			return this->messageList[this->tail];
		}
	}

	int getListSize() {
		return 20;
	}

	int getLength() {
		return this->messageSize;
	}
};