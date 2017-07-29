//////////////////////////////////////////////////////////////////////////
//Copyright(C) Dake，2010-05
//编程方式：win32 SDK C语言编程
//文件名：WinMain.c
//描述：程序入口
//主要函数：WinMain()
//版本：V1.1
//////////////////////////////////////////////////////////////////////////

#include "global.h"
#include "MainDlg.h"
#include "voiceprint.h"
#include "DebugDlg.h"


HWND CreateMainWin(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE  hInst = 0; // 存储当前程序实例句柄


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,PSTR szCmdLine, int iCmdShow)
{
	MSG    msg;
	RECT    rect;
	HWND hwnd;


	hwndMainWin = hwnd = CreateMainWin(hInstance);
	my_voiceprint.f_Init(&my_voiceprint);
	hwndMainDlg = my_voiceprint.hwnd = CreateDialogParam(hInstance, szAppName, hwnd, MainDlg, iCmdShow);
	/**
	* 把主窗口大小与登录对话框匹配
	* 显示两个窗口（主窗口和登录对话框）
	*/
	GetWindowRect(hwndMainDlg, &rect);
	AdjustWindowRect(&rect, WS_CAPTION | WS_BORDER, FALSE);
	SetWindowPos(hwnd, NULL, 0, 0, rect.right - rect.left,rect.bottom - rect.top, SWP_NOMOVE);
	ShowWindow(hwndMainDlg, SW_SHOW);   
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (hwndMainDlg == NULL || !IsDialogMessage(hwndMainDlg, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HMENU hMenu;

	switch (message)
	{
	case WM_CREATE:
		srand((unsigned int)time(NULL));
		InitializeCriticalSection(&cs);   // 初始化临界区;
#ifdef _DEBUG
		if (!hwndDebugDlg)
		{
			//调试窗口
			hwndDebugDlg = CreateDialog(hInst, TEXT("DEBUG_WINDOW"), hwnd, DebugDlg);
			ShowWindow(hwndDebugDlg, SW_SHOW);
		}
#endif
		return TRUE;

	case WM_SETFOCUS:
		SetFocus(hwndMainDlg);
		return TRUE;

	case WM_COMMAND:
        hMenu = GetMenu(hwnd);

		switch (LOWORD (wParam))
		{
		case IDM_OPTION:      // 首选项   
			MessageBeep(0);
			return TRUE;

		case IDM_APP_EXIT:    // 退出
			SendMessage(hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
			return TRUE;

		case IDM_APP_HELP:    // 帮助
			MessageBox(hwnd, TEXT("<-文本无关声纹识别之win32 Demo->\n\n")
				TEXT("语音特征向量：MFCC系数（梅尔倒谱系数）\n")
				TEXT("模式匹配模型：GMM模型（高斯混合模型）\n")
				TEXT("声音数据参数：单通道，16bit的PCM码流\n")
				TEXT("项目主要内容：声纹识别模块由纯C完成，可在linux下正常编译，对应本Demo的”voice_id“目录，其余模块为win32 Demo的gui及录音模块实现。\n\n")
				TEXT("使用步骤：\n")
				TEXT("1、点击\"提取声纹\"，确认后开始录音，当采集到足够的声音数据后会提示完成。\n")
				TEXT("2、完成第1步后，点击\"身份验证\"，确认后开始录音，当采集到足够的声音数据后会提示完成，并计算识别结果。\n")
				TEXT("3、若建模或识别过程失败请按照失败提示设置参数后重新进行。\n")
				TEXT("注意事项：\n")
				TEXT("1、由于各份声音数据未进行音量归一化操作，所以不同音量会对DCT过程产生影响，请尽量保证各份声音数据在相同音量环境下录制！\n")
				TEXT("2、由于时间原因，mfcc、gmm及后验概率的计算位于主线程中，即是单线程的（录音模块单独分配一个线程），所以如果帧数设置过高，可能会引起程序无响应。\n")
				TEXT("3、\"voice_id\"目录（声纹识别模块）只提供：mfcc、gmm、后验概率的计算，要得到合理的识别结果还需设计识别结果处理方式，可参照VoicePrintIdentify()函数处理。"),
				szAppVersion,  MB_OK);
			return TRUE;

		case IDM_APP_ABOUT:  // 关于
			MessageBox(hwnd, TEXT("Voice Print V1.1\n")
				TEXT("CopyRight(c) Dake, 2010\n")
				TEXT("指导老师：吴震东\n")
				TEXT("作者：Dake\n")
				TEXT("电邮：<chen423@yeah.net>"),
				szAppVersion, MB_ICONINFORMATION | MB_OK) ;
			return TRUE;
		}
		break ;

	case WM_DESTROY:
		DestroyWindow(hwndMainDlg);
		DeleteCriticalSection(&cs); // 删除临界区
		PostQuitMessage(0);
		return TRUE;

	case WM_SYSCOMMAND:
		switch (LOWORD(wParam))
		{
		case  SC_CLOSE:
			if (hwndMainDlg)
			{
				SendMessage(hwndMainDlg, WM_SYSCOMMAND, SC_CLOSE, 0);
			}
			else
			{
				SendMessage(hwnd, WM_DESTROY, 0 ,0);
			}
			return TRUE;
		}
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}


HWND CreateMainWin(HINSTANCE hInstance)
{
	HWND hwnd;
	WNDCLASS wndclass;


	hInst = hInstance;
	wndclass.style = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
	wndclass.hCursor = NULL;
	wndclass.hbrBackground = NULL;
	wndclass.lpszMenuName = szAppName;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"),
			szAppVersion, MB_ICONERROR);
		return FALSE;
	}

	hwnd = CreateWindow(szAppName, szAppVersion,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER | WS_MINIMIZEBOX,  
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);
	return hwnd;
}