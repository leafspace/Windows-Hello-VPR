/** @file
********************************************************************************
<PRE>
模块名: 录音及放音模块
文件名: myRecorder.h
相关文件: myRecorder.c
文件实现功能: 录音模块相关函数、变量、类型的声明
作者: Dake
版本: V1.1
编程方式: win32 SDK C语言编程
授权方式: Copyright(C) Dake
联系方式: chen423@yeah.net
生成日期: 2010-05
--------------------------------------------------------------------------------
多线程安全性: <否>[录音及放音分别开启一个线程，需与程序中其它线程同步处理]
异常时安全性: <是/否>[，说明]
--------------------------------------------------------------------------------
备注: <其它说明>
--------------------------------------------------------------------------------
修改记录:
日 期              版本     修改人              修改内容
2011/03/29   1.1     <Dake>           <修改相关函数声明及文件封装性>
</PRE>
*******************************************************************************/

#include "global.h"

#ifndef _MYRECORD_
#define _MYRECORD_

typedef void * HRECORD;

typedef struct MYRECORD 
{
	LPHWAVEIN  lhWaveIn; //录音句柄
	PBYTE pBuffer[BUFF_NUM];
	PWAVEHDR pWaveHdr[BUFF_NUM];
	PWAVEFORMATEX pWaveform;
} MYRECORD;

typedef struct RECPARAM
{
	HWND hwnd;
	HRECORD myRecord;
} RECPARAM;


//录音相关函数声明
HRECORD CreateRecord(HWND hwnd);

#endif    //_MYRECORD_

