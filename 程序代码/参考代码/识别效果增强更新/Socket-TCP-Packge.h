#include <iostream>
using namespace std;
class SocketPackge
{
private:
    string orginIP;      //源主机地址
    string targetIP;     //目标主机地址
    
    int packgeSize;      //发送整个包大小
    int dataSize;        //发送数据包大小
    
    int state;           //state=1表示发送 state=2表示响应接收成功 state=4表示接收数据失败（或丢失） state=5表示重新发送
    string MD5;
public:
    SocketPackge();
};