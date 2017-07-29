//////////////////////////////////////////////////////////////////////////
//Copyright(C) Dake，2010-05
//编程方式：win32 SDK C语言编程
//文件名：global.c
//描述：程序相关的全局变量、函数的定义
//主要函数：EditPrintf()用于向指定文本框输出经格式化的字串
//版本：V1.1
//////////////////////////////////////////////////////////////////////////

#include "global.h"
#include "voiceprint.h"

#ifdef _DEBUG
HWND hwndDebugDlg = NULL;      //调试使用
HWND hwndEdit           = NULL;
#endif

int g_build_buff_num = 0;       // 建模缓冲数
BOOL bExit             = FALSE;           // 退出登录（关闭程序）
BOOL killThreading = FALSE;           // 关闭线程
BOOL g_buildGMM = FALSE;          //标记提取声纹
TCHAR * szAppName  = TEXT("MAINDLG");
TCHAR * szAppVersion = TEXT("Voice Print V1.1");
HWND hwndMainWin  = NULL;
HWND hwndMainDlg  = NULL;
CRITICAL_SECTION cs; 


//////////////////////////////////////////////////////////////////////////
//函数名：EditPrintf(）
//功能：向指定文本框输出经格式化的字串
//输入：hwndEdit：文本框句柄，szFormat：格式字串， ...：数量可变的参数
//调用：wvsprintf()
//主调函数：
//////////////////////////////////////////////////////////////////////////
void EditPrintf(HWND hwndEdit, TCHAR * szFormat, ...)
{
	TCHAR szBuffer [2200];
	va_list pArgList;
	va_start(pArgList, szFormat);
	vsprintf(szBuffer, szFormat, pArgList);
	va_end(pArgList);
	SendMessage(hwndEdit, EM_SETSEL, (WPARAM) -1, (LPARAM) -1);
	SendMessage(hwndEdit, EM_REPLACESEL, FALSE, (LPARAM) szBuffer);
	SendMessage(hwndEdit, EM_SCROLLCARET, 0, 0);
}


//////////////////////////////////////////////////////////////////////////
//函数名：QuitMyWindow(）
//功能：关闭所有子线程后再退出程序
//输入：
//调用：
//主调函数：UsrDlg( )，voiceSockThread( )，myPlayThread( )，myRecThread( )
//////////////////////////////////////////////////////////////////////////
void QuitMyWindow()
{
	if(bExit || killThreading)    //判断bExit防止重复关闭造成消息重复发送
	{//因为子线程会把bExit置为FALSE
		return;
	}
	if (my_voiceprint.g_RecThreadID)    
	{
		bExit = TRUE;
		PostThreadMessage(my_voiceprint.g_RecThreadID, WM_RECORD_END, 0, 0);    //关闭录音线程
		return;
	}
	PostMessage(GetParent(hwndMainDlg), WM_DESTROY, 0, 0);
}