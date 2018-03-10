#include <afx.h>
#include <iostream>
#include <Windows.h>
using namespace std;

int main()
{
	/*
	HWND hWnd = ::FindWindowW(L"LogonUI", NULL);                                                                     //搜索记事本程序主窗口句柄 
	HWND hWndc = ::GetWindow(hWnd, GW_CHILD);                                                                        //获得记事本客户区句柄（该窗口是记事本主窗口的子窗口，即那个白色的可编辑区域） 

	Sleep(5000);
	char password[256] = "sandaozhishu429";
	if(hWndc) {                                                                                                      //如果获得了该句柄
		for (int i = 0; i < strlen(password); ++i) {
			::SendMessage(hWnd, WM_CHAR, password[i], NULL);                                                         //发送按键消息
		}
	}
	*/
	Sleep(5000);
	HWND wnd;                                                                                                        //窗口句柄
	wnd = GetForegroundWindow();                                                                                     //获得当前激活的窗口句柄
	DWORD SelfThreadId = GetCurrentThreadId();                                                                       //获取本身的线程ID
	DWORD ForeThreadId = GetWindowThreadProcessId(wnd, NULL);                                                        //根据窗口句柄获取线程ID
	AttachThreadInput(ForeThreadId, SelfThreadId, true);                                                             //附加线程
	wnd = GetFocus();                                                                                                //获取具有输入焦点的窗口句柄
	AttachThreadInput(ForeThreadId, SelfThreadId, false);                                                            //取消附加的线程

	char password[256] = "sandaozhishu429";
	for (int i = 0; i < (int) strlen(password); ++i) {
		SendMessage(wnd, WM_CHAR, WPARAM(password[i]), 0);                                                           //发送一个字消息
	}
	SendMessage(wnd, WM_KEYDOWN, VK_RETURN, 0);
	return 0;
}