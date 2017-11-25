#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

void findIP(char*ip, int size)
{
	 WORD v = MAKEWORD(1, 1);
	 WSADATA wsaData;
	 WSAStartup(v, &wsaData);
	 
	 struct hostent *phostinfo = gethostbyname("");
	 char *p = inet_ntoa(*((struct in_addr*)(*phostinfo->h_addr_list)));
	 strncpy(ip, p, size - 1);
	 ip[size - 1] = '\0';
	 WSACleanup();
}

void getIPs()
{
	WORD v = MAKEWORD(1, 1);
	WSADATA wsaData;
	WSAStartup(v, &wsaData);
	
	struct hostent *phostinfo = gethostbyname("");
	for(int i = 0; NULL != phostinfo && NULL != phostinfo->h_addr_list[i]; ++i) {
		char *pszAddr = inet_ntoa(*(struct in_addr*)phostinfo->h_addr_list[i]);
		printf("%s\n", pszAddr);
	}
	WSACleanup();
}

int main()
{
	char ip[20] = {0};
	findIP(ip, sizeof(ip));
	printf("%s\n", ip);

	getIPs();
	return 0;
}