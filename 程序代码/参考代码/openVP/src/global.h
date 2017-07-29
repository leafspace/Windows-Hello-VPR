//////////////////////////////////////////////////////////////////////////
//Copyright(C) Dake，2010-05
//编程方式：win32 SDK C语言编程
//文件名：global.h
//描述：程序相关的全局变量、函数的声明
//主要函数：
//版本：
//////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <malloc.h> 
#include <time.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>
#include "debug/xdebug.h"
#include "resource.h"

#pragma comment(lib,"winmm.lib")

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#define BUFFLEN (512 * 2)
/* 录音缓冲数量 */
#define BUFF_NUM 10

#define PERCENT 19              /*< 识别范围 */
#define IDENTIFY_NUM 4          /*< 识别次数 */
#define IDENTIFY_FRAME_NUM 200  /*< 识别帧数 */
#define IDENTIFY_BUFF_NUM ((IDENTIFY_FRAME_NUM/2 +10)/(BUFFLEN/FRAME_LEN)) /*< 识别缓冲数 */

#ifdef _DEBUG
extern HWND hwndDebugDlg;   /*< 调试使用 */
extern HWND hwndEdit;
#define X_PRINT EditPrintf
#define X_HANDLE hwndEdit
#endif

extern int g_build_buff_num; /*< 建模缓冲数 */
extern BOOL bExit;           /*< 退出登录（关闭程序） */
extern BOOL g_bStopRecv;     /*< 标记是否继续接收对方发来的音频数据 */
extern BOOL killThreading;   /*< 正在关闭子线程 */
extern BOOL g_buildGMM;      /*< 标记提取声纹 */
extern TCHAR * szAppName;
extern TCHAR * szAppVersion;
extern HINSTANCE hInst;      /*< 存储当前程序句柄 */
extern HWND hwndMainWin;
extern HWND hwndMainDlg; 
extern CRITICAL_SECTION cs;  /*< 临界区 */

void EditPrintf(HWND hwndEdit, TCHAR * szFormat, ...);
void QuitMyWindow();

#endif /* _GLOBAL_H_ */
