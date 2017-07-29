/** @file
********************************************************************************
<PRE>
模块名: 录音及放音模块
文件名: myRecorder.c
相关文件: myRecorder.h
文件实现功能: 录音模块相关函数、变量、类型的定义
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

#include "myRecord.h"
#include "voiceprint.h"

// 采样频率
#define SAMPLE_FREQUENCY 16384

// 一些错误提示字串
static TCHAR * szOpenError = TEXT("打开 waveform audio错误!");
static TCHAR * szMemError = TEXT("音频分配缓冲错误!");
static WAVEFORMATEX waveform;

static void isWaveSuccess(MMRESULT waveresult);
static void SetWaveform(PWAVEFORMATEX pWaveform, WORD nChannels, DWORD nSamplesPerSec, WORD wBitsPerSample);
static void SetPWaveHDR(PWAVEHDR pWaveHdr, PBYTE pBuffer, DWORD bufferLen, DWORD dwFlg, DWORD dwLoops);
static void SetRecord(HRECORD hRecord);
static void CloseRecord(HRECORD hRecord);
static DWORD WINAPI myRecThread(void * pRecParam);

/** @function
********************************************************************************
<PRE>
函数名: isWaveSuccess()
功能: 检查wave函数调用的返回值，有错则弹出对话框
用法:
参数:
[IN] waveresult: wave函数调用的返回值
返回:
调用: 
主调函数: myRecThread()
</PRE>
*******************************************************************************/
static void isWaveSuccess(MMRESULT waveresult)
{
	switch (waveresult)
	{
	case MMSYSERR_INVALHANDLE:    //设备句柄无效
		MessageBox(NULL, "MMSYSERR_INVALHANDLE", szAppVersion,
			MB_ICONEXCLAMATION | MB_OK);
		break;

	case WAVERR_UNPREPARED:     //wave头未准备
		MessageBox(NULL, "WAVERR_UNPREPARED", szAppVersion,
			MB_ICONEXCLAMATION | MB_OK);
		break;
	default:	 break;
	}
}


/** @function
********************************************************************************
<PRE>
函数名: SetWaveform()
功能: 设置（填充）WAVEFORMATEX结构
用法:
参数:
[IN] pWaveform: WAVEFORMATEX结构指针
[IN] nChannels: 声道 [1（单声道）or 2（立体声）]
[IN] nSamplesPerSec: 采样频率
[IN] wBitsPerSample: 单样本比特数
返回:
调用:
主调函数: CreateRecord()
</PRE>
*******************************************************************************/
static void SetWaveform(PWAVEFORMATEX pWaveform, WORD nChannels, 
				 DWORD nSamplesPerSec, WORD wBitsPerSample)
{
	pWaveform->wFormatTag = WAVE_FORMAT_PCM;
	pWaveform->nChannels = nChannels;    //1
	pWaveform->nSamplesPerSec = nSamplesPerSec;    //SAMPLE_FREQUENCY
	pWaveform->nAvgBytesPerSec = nChannels * nSamplesPerSec;    //SAMPLE_FREQUENCY
	pWaveform->nBlockAlign = nChannels * wBitsPerSample / 8;    //1
	pWaveform->wBitsPerSample  = wBitsPerSample;    //8 
	pWaveform->cbSize = 0;
}

/** @function
********************************************************************************
<PRE>
函数名: SetPWaveHDR()
功能: 设置（填充）PWAVEHDR结构
用法:
参数:
[IN] pWaveHdr: WAVE头句柄指针
[IN] pBuffer: 录音/放音缓冲
[IN] bufferLen: 缓冲长度
[IN] dwFlg: 缓冲区状态标志
[IN] dwLoops: 声音重复播放次数
返回:
调用:
主调函数: SetRecord()
</PRE>
*******************************************************************************/
static void SetPWaveHDR(PWAVEHDR pWaveHdr, PBYTE pBuffer, 
				 DWORD bufferLen, DWORD dwFlg, DWORD dwLoops)
{
	pWaveHdr->lpData = pBuffer;
	pWaveHdr->dwBufferLength  = bufferLen;   //BUFFLEN;
	pWaveHdr->dwBytesRecorded = 0;
	pWaveHdr->dwUser = 0;
	pWaveHdr->dwFlags = dwFlg;   //0是默认？
	pWaveHdr->dwLoops =dwLoops;   //录音时取1
	pWaveHdr->lpNext  = NULL;
	pWaveHdr->reserved = 0;
}


/** @function
********************************************************************************
<PRE>
函数名: CreateRecord()
功能: 打开录音环境，初始化WAVEFORMATEX结构，初始化"录音写入"句柄指针
lhWaveIn，并创建录音线程。
用法:
参数:
返回:
非空: 初始化成功
NULL: 失败
调用: SetWaveform()
主调函数: MainDlg()
</PRE>
*******************************************************************************/
HRECORD CreateRecord(HWND hwnd)
{
	MYRECORD * myRecord = NULL;
	RECPARAM * pRecParam = NULL;    //录音线程输入参数  

    // 进入临界区：保证主线程与录音线程在创建录音线程时互斥
	EnterCriticalSection(&cs);     
	myRecord = (MYRECORD *)malloc(sizeof(MYRECORD));
	pRecParam = (RECPARAM *)malloc(sizeof(RECPARAM));
	if (!myRecord || !pRecParam)
	{
		MessageBox(NULL, "初始化myrecord失败！", szAppVersion,
			MB_ICONEXCLAMATION | MB_OK);
		return NULL;
	}

	//分配录音句柄
	myRecord->lhWaveIn = (LPHWAVEIN)malloc(sizeof(HWAVEIN));
	if (!myRecord->lhWaveIn)
	{
		MessageBox(NULL, "分配录音句柄失败！", szAppVersion,
			MB_ICONEXCLAMATION | MB_OK);
		return NULL;
	}

	//分配waveform结构
	myRecord->pWaveform = (PWAVEFORMATEX)malloc(sizeof(WAVEFORMATEX));
	if (!myRecord->pWaveform)
	{
		MessageBox(NULL, "初始化WAVEFORMATEX结构失败！", szAppVersion,
			MB_ICONEXCLAMATION | MB_OK);
		return NULL;
	}

	// Open waveform audio for input
	SetWaveform(myRecord->pWaveform, 1, SAMPLE_FREQUENCY, 8);

	pRecParam->hwnd = hwnd;
	pRecParam->myRecord = myRecord;
	my_voiceprint.g_hRecThread = CreateThread(NULL, 0, myRecThread, pRecParam, 0, &my_voiceprint.g_RecThreadID); 

	//产生MM_WIM_OPEN消息
	if (waveInOpen(myRecord->lhWaveIn, WAVE_MAPPER, myRecord->pWaveform, 
		my_voiceprint.g_RecThreadID, 0, CALLBACK_THREAD))
	{
		MessageBeep(MB_ICONEXCLAMATION);
		MessageBox(hwnd, szOpenError, szAppVersion,
			MB_ICONEXCLAMATION | MB_OK);
		return NULL;
	}

    LeaveCriticalSection(&cs);    // 离开临界区
	return((void *) myRecord);
}


/** @function
********************************************************************************
<PRE>
函数名: SetRecord()
功能: 设置录音环境并开始录音（分配并设置PWAVEHDR 结构、分配录音缓冲区
并加入录音队列）
用法: 在waveInOpen()触发的MM_WIM_OPEN消息中使用
参数:
[IN] hRecord: 录音实例
返回:
调用: SetPWaveHDR()
主调函数: myRecThread()
</PRE>
*******************************************************************************/
static void SetRecord(HRECORD hRecord)
{
	int  i;
	MYRECORD * myRecord = (MYRECORD *)hRecord;
	HWAVEIN hWaveIn = *(myRecord->lhWaveIn);

	// 分配录音缓冲和PWAVEHDR结构
	for (i = 0; i < BUFF_NUM; ++i)
	{		
		myRecord->pBuffer[i] = malloc(BUFFLEN);
		myRecord->pWaveHdr[i] = (PWAVEHDR)malloc(sizeof(WAVEHDR));
		if (!myRecord->pBuffer[i] || !myRecord->pWaveHdr[i])
		{
			break;
		}

		// 设置WAVEHDR并准备好录音
		SetPWaveHDR(myRecord->pWaveHdr[i], myRecord->pBuffer[i], BUFFLEN, 0, 1);
		waveInPrepareHeader(hWaveIn, myRecord->pWaveHdr[i], sizeof(WAVEHDR));
		waveInAddBuffer(hWaveIn, myRecord->pWaveHdr[i], sizeof(WAVEHDR));

	} //结束：for (i=0; i<BUFF_NUM; ++i)

	// 内存分配出错
	if (i < BUFF_NUM)
	{
		for (--i; i>=0; --i)
		{
			waveInUnprepareHeader(hWaveIn, myRecord->pWaveHdr[i], sizeof(WAVEHDR));
			free(myRecord->pBuffer[i]);
			free(myRecord->pWaveHdr[i]);
		}
		return;
	} // 结束：if (i<BUFF_NUM)

	//开始录音，缓冲区录满后发送MM_WIM_DATA消息
	waveInStart(hWaveIn);
}


/** @function
********************************************************************************
<PRE>
函数名: CloseRecord()
功能: 清空录音环境
用法: 在waveInClose()触发的MM_WIM_CLOSE消息中使用
参数:
[IN] hRecord: 录音实例
返回:
调用: 
主调函数: myRecThread()
</PRE>
*******************************************************************************/
static void CloseRecord(HRECORD hRecord)
{
	int i;
	MYRECORD * myRecord = (MYRECORD *)hRecord;
	HWAVEIN hWaveIn = *(myRecord->lhWaveIn);

	for (i=0; i<BUFF_NUM; ++i)
	{
		waveInUnprepareHeader(hWaveIn, myRecord->pWaveHdr[i], sizeof(WAVEHDR));
		free(myRecord->pBuffer[i]);
		free(myRecord->pWaveHdr[i]);
	}
	free(myRecord->pWaveform);
	free(myRecord->lhWaveIn);
	free(myRecord);
}


/** @function
********************************************************************************
<PRE>
函数名: myRecThread()
功能: 录音线程函数
用法:
参数:
[IN] pRecParam: 指向RECPARAM结构的指针
返回:
调用: SetRecord(), CloseRecord()
主调函数: CreateRecord()
</PRE>
*******************************************************************************/
static DWORD WINAPI myRecThread(void * pRecParam)
{
	MSG msg;
	HWND hwnd = ((RECPARAM *)pRecParam)->hwnd;
	MYRECORD * myRecord = ((RECPARAM *)pRecParam)->myRecord;
	BOOL  bEnding =  FALSE; 
	BYTE * pVoiceBuffer = NULL;

	int i;
	int frame_num = 0;
	int buff_num = g_build_buff_num;
	double temp = 0;


	while (GetMessage(&msg, 0, 0, 0))
	{ 
		switch(msg.message)
		{
		case  MM_WIM_OPEN:
			EnterCriticalSection(&cs);     //进入临界区
			SetRecord(myRecord);
			//bEnding =  FALSE; 
			LeaveCriticalSection(&cs);    //离开临界区
			break;

		case  MM_WIM_DATA:
			if (bEnding && myRecord)    //判断myRecord是为了避免重复waveInClose()
			{
				//产生MM_WIM_CLOSE消息
				EnterCriticalSection(&cs);     //进入临界区
				isWaveSuccess(waveInClose(*(myRecord->lhWaveIn)));
				CloseRecord(myRecord);
				myRecord = NULL;
				LeaveCriticalSection(&cs);    //离开临界区
				break;
			}

			if (!myRecord)
			{
				break;
			}

			pVoiceBuffer = ((PWAVEHDR)(msg.lParam))->lpData;
			//--------------------------------------------------------------------------------------------------
			// 提取声纹语音和识别语音
			//
			if (((PWAVEHDR)(msg.lParam))->dwBytesRecorded > 0 && g_buildGMM)
			{
				if (!my_voiceprint.voice_mdl.bValued && frame_num >= buff_num)
				{
					break;
				}
				else if (my_voiceprint.voice_mdl.bValued && frame_num >= IDENTIFY_BUFF_NUM)
				{
					break;
				}
				for (i = 0, temp = 0; i < BUFFLEN; ++i)
				{
					temp += abs((pVoiceBuffer[i] - 128));
				}
				if (temp > 1050)  // 静寂判断 1050
				{
					memcpy(my_voiceprint.voice_mdl.pVoice+(frame_num%5)*BUFFLEN, pVoiceBuffer, BUFFLEN);
					++frame_num;
				}
				if (frame_num > 0 && frame_num%5 == 0)
				{
					fwrite(my_voiceprint.voice_mdl.pVoice, sizeof(TBYTE), BUFFLEN * 5, my_voiceprint.voice_mdl.file_stream);
				}
				if ((!my_voiceprint.voice_mdl.bValued && frame_num==buff_num) || 
					(my_voiceprint.voice_mdl.bValued && frame_num==IDENTIFY_BUFF_NUM))
				{
					if (frame_num > 0 && frame_num%5 != 0)
					{
						fwrite(my_voiceprint.voice_mdl.pVoice, sizeof(TBYTE), (frame_num%5 )* BUFFLEN, my_voiceprint.voice_mdl.file_stream);
					}
					free(my_voiceprint.voice_mdl.pVoice);
					my_voiceprint.voice_mdl.pVoice = NULL;
					fclose(my_voiceprint.voice_mdl.file_stream);
					my_voiceprint.voice_mdl.file_stream = NULL;
					PostMessage(hwndMainDlg, WM_BUILD_GMM, 0, 0);	

					xprint(TEXT("语音收集完成"));
				}
			} // end: if (g_buildGMM)
			waveInAddBuffer(*(myRecord->lhWaveIn), (PWAVEHDR)(msg.lParam), sizeof(WAVEHDR));
			break;

		case MM_WIM_CLOSE:
			CloseHandle(my_voiceprint.g_hRecThread);
			my_voiceprint.g_RecThreadID = 0;
			free(pRecParam);
			/*SetWindowText(GetDlgItem(hwnd, IDC_BUILD_GMM), TEXT("提取声纹"));*/
			if (bExit)
			{
				bExit = FALSE;
				killThreading =FALSE;
				QuitMyWindow();
			}
			if (my_voiceprint.voice_mdl.file_stream)
			{
				fclose(my_voiceprint.voice_mdl.file_stream);
				my_voiceprint.voice_mdl.file_stream = NULL;
			}
			return msg.wParam;

		case WM_RECORD_END:
			if (bExit)
			{
				killThreading = TRUE;
			}
			bEnding = TRUE;
			// 使录音停止，并产生MM_WIM_DATA消息
			isWaveSuccess(waveInReset(*(myRecord->lhWaveIn)));
			break;
		}
	}

	free(pRecParam);
	return msg.wParam; 
}