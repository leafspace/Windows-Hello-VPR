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

void SocketResponsePackge::setLostNumber(int lostNumber)
{
    this->lostNumber = lostNumber;
}