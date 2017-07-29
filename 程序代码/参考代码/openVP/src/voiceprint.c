//////////////////////////////////////////////////////////////////////////
//Copyright(C) Dake，2010-06
//编程方式：win32 SDK C语言编程
//文件名：voiceprint.c
//描述：
//主要函数：
//版本：
//////////////////////////////////////////////////////////////////////////
#include "voiceprint.h"

VOICE_PRINT my_voiceprint = {VoicePrintInit};

void VoiceModelInit(VOICE_MODEL * this)
{
	int i;
	int j;

	for (i = 0; i < IDENTIFY_NUM; ++i)
	{
		for (j = 0; j < 2; ++j)
		{
			this->value[i][j] = 0;
		}
	}
	this->gmm.f_Init = InitGMMClass;
	this->gmm.f_Init(&this->gmm, M);
	this->percent = PERCENT;
	this->frame_num = GOOD_FRAME_NUM;
	this->m = M;
	this->bValued = FALSE;
	this->file_stream = NULL;
	this->pVoice = NULL;
	this->voice_file.f_Init = FileClassInit;
	this->voice_file.f_Init(&this->voice_file);
	this->f_GetUsrGMM = GetUsrGMM;
}


void VoicePrintInit(VOICE_PRINT * this)
{
	int i;

	this->voice_mdl.f_Init = VoiceModelInit;
	this->voice_mdl.f_Init(&this->voice_mdl);
	this->hwnd = NULL;
	this->myRecord = NULL;
	this->g_hRecThread = NULL;
	this->g_RecThreadID = 0;
	for (i = 0; i < IDENTIFY_NUM; ++i)
	{
		/*this->rec_frame[i] = IDENTIFY_FRAME_NUM / (IDENTIFY_NUM - i);*/
		this->rec_frame[i] = IDENTIFY_FRAME_NUM / IDENTIFY_NUM;
	}
}

// mod取E_BUILD是建模，取E_IDENTIFY是识别
BOOL GetUsrGMM(VOICE_PRINT * pUsr, BUILD_MODE mod)
{
	int i;
	FILE * fi = NULL;
	int frame_num;
	int buff_num;
	int buff_size;
	BOOL bRes;
	TBYTE * pRawVoice = NULL;   //指向原始语音数据
	double ** MFCC = NULL;
	double temp;

	switch (mod)
	{
	case E_BUILD: // 建模模式
		frame_num = pUsr->voice_mdl.frame_num;
		buff_num = g_build_buff_num;
		break;
		
	case E_IDENTIFY: // 识别模式
		frame_num = IDENTIFY_FRAME_NUM;
		buff_num = IDENTIFY_BUFF_NUM;
		break;

	default:	
		break;
	}

	buff_size = buff_num * BUFFLEN;
	fi = fopen(pUsr->voice_mdl.voice_file.szFileName, "rb");
	if (!fi)
	{
		xprint(TEXT("打开声纹数据文件出错"));
		return FALSE;
	}
	pRawVoice = (TBYTE *)malloc(buff_size);
	if (!pRawVoice)
	{
		xprint(TEXT("分配读文件缓冲失败"));
		fclose(fi);
		return FALSE;
	}
    fread(pRawVoice, sizeof(TBYTE), buff_size, fi);
	fclose(fi);
	MFCC = (double **)malloc(frame_num * sizeof(double *));
	for (i = 0; i < frame_num; ++i)
	{
		MFCC[i] = (double *)malloc(D * sizeof(double));
	}
	if (voiceToMFCC(pRawVoice, buff_num * BUFFLEN, MFCC, frame_num))
	{
		free(pRawVoice);
		if (mod == E_BUILD)
		{
			xprint(TEXT("训练参数: %d %d"), pUsr->voice_mdl.frame_num, pUsr->voice_mdl.m);
			if (!GMMs(MFCC, &pUsr->voice_mdl.gmm, pUsr->voice_mdl.frame_num, pUsr->voice_mdl.m))
			{
				MessageBox(pUsr->hwnd, "GMMs失败！", szAppVersion, 0);
				goto RETURN_FALSE;
			}
		}
// TODO 判断GMM_identify返回值
		for (i = 0; i < IDENTIFY_NUM; ++i)
		{
			bRes = GMM_identify(&MFCC[i*(IDENTIFY_FRAME_NUM / IDENTIFY_NUM)], &temp, &(pUsr->voice_mdl.gmm), pUsr->rec_frame[i], pUsr->voice_mdl.m);
			if (bRes)
			{
				pUsr->voice_mdl.value[i][mod] = fabs(temp);
			}
			else
			{				
				MessageBox(pUsr->hwnd, "GMM_identify失败！", szAppVersion, 0);
RETURN_FALSE:
				for (i = 0; i < frame_num; ++i)
				{
					free(MFCC[i]);
				}
				free(MFCC);
				return FALSE;
			}
		}
		if (mod == E_BUILD) // 打印模型阈值
		{
			PrintThresholdValue(pUsr->hwnd);
		}
		for (i = 0; i < frame_num; ++i)
		{
			free(MFCC[i]);
		}
		free(MFCC);	
	}
	else
	{
		MessageBox(pUsr->hwnd, TEXT("MFCC失败！"), szAppVersion, 0);
		for (i = 0; i < frame_num; ++i)
		{
			free(MFCC[i]);
		}
		free(MFCC);
		free(pRawVoice);
		return FALSE;
	}
	return TRUE;
}

void PrintThresholdValue(HWND hwnd)
{
	int i;
	HWND hwndPrint;

	hwndPrint = GetDlgItem(hwnd, IDC_TEXTOUT);
	EditPrintf(hwndPrint, TEXT("声纹识别阈值:\r\n"));
	for (i = 0; i < IDENTIFY_NUM; ++i)
	{
		EditPrintf(hwndPrint, TEXT("第%d个%d帧:  %lf\r\n"), i+1, my_voiceprint.rec_frame[i], my_voiceprint.voice_mdl.value[i][0]);
	}
}