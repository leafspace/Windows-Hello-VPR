#pragma once

#include <string>
#include <iostream>
using namespace std;

class MessageQueue
{
private:
	string messageList[20];
	int head;                                                                //列表头
	int tail;                                                                //列表尾
	int messageSize;                                                         //当前信息列表的长度

public:
	MessageQueue() {
		this->head = 0;
		this->tail = 0;
		this->messageSize = 0;
	}

	bool pushMessage(string message) {
		if ((this->tail + 1) % 20 == this->head) {                           //判断下一个文件尾巴是否是跟文件头相同，相同则满
			cout << "ERROR : Message queue is full !" << endl;
			return false;
		} else {
			this->tail = (this->tail + 1) % 20;                              //判断下一个文件头的位置
			this->messageList[this->tail] = message;
			this->messageSize++;
		}
		return true;
	}

	string popMessage() {
		if (this->tail == this->head) {                                      //没有文件
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