#include "Socket_TCP_Packge.h"

void PackgeHead::setOrginIP(string orginIP)
{
    this->orginIP = orginIP;
}

void PackgeHead::setTargetIP(string targetIP)
{
    this->targetIP = targetIP;
}

void PackgeHead::setPackgeNumber(int packgeNumber)
{
    this->packgeNumber = packgeNumber;
}

void PackgeHead::setPackgeSize(int packgeSize)
{
    this->packgeSize = packgeSize;
}

void PackgeHead::setHeadSize(int headSize = 20)
{
    this->headSize = headSize;
}

void PackgeHead::setDataSize(int dataSize)
{
    this->dataSize = dataSize;
}


void PackgeHead::setState(int state)
{
    this->state = state;
}

string PackgeHead::getOrginIP()
{
	return this->orginIP;
}

string PackgeHead::getTargetIP()
{
	return this->targetIP;
}
    
int PackgeHead::getPackgeNumber()
{
	return this->packgeNumber;
}

int PackgeHead::getPackgeSize()
{
	return this->packgeSize;
}

int PackgeHead::getHeadSize()
{
	return this->headSize;
}

int PackgeHead::getDataSize()
{
	return this->dataSize;
}

int PackgeHead::getState()
{
	return this->state;
}

bool SocketDataPackge::setData(char* data, int dataSize)
{
    bool state = true;
    try {
        if (PackgeHead::dataSize != dataSize) {
            PackgeHead::setDataSize(dataSize);
        }

        this->dataList = new char[this->dataSize];
        memcpy(this->dataList, data, this->dataSize);
    } catch (const char* message) {
        cerr << message << endl;
        state = false;
    }

    return state;
}

void SocketDataPackge::setHashMD5(string hashMD5)
{
    this->hashMD5 = hashMD5;
}


char* SocketDataPackge::getData()
{
	return this->dataList;
}

string SocketDataPackge::getHashMD5()
{
	return this->hashMD5;
}

void SocketResponsePackge::setLostNumber(int lostNumber)
{
    this->lostNumber = lostNumber;
}

int SocketResponsePackge::getLostNumber()
{
	return this->lostNumber;
}