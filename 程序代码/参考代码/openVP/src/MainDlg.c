//////////////////////////////////////////////////////////////////////////
//Copyright(C) Dake，2010-05
//编程方式：win32 SDK C语言编程
//文件名：MainDlg.c
//描述：
//主要函数：
//版本：V1.1
//////////////////////////////////////////////////////////////////////////
#include "MainDlg.h"
#include "voiceprint.h"
#include "FileDlg.h"

static void VoicePrintIdentify();
static void OnInitDlg(HWND hwnd);
static void SetupDlg(HWND hwnd);
static void UpdateDlg(HWND hwnd, LPARAM lParam);
static BOOL OnBuildGMM(HWND hwnd);
static void OnClearBtn(HWND hwnd);
static void OnResetBtn(HWND hwnd);
static void OnStopVoice(HWND hwnd);
static void OnRecordBeg(HWND hwnd, LPARAM lParam);
static void OnRecordEnd(HWND hwnd);
static void OnBuildBtn(HWND hwnd);
static void OnIdentifyBtn(HWND hwnd);
static void OnExportGMMBtn(HWND hwnd);
static void OnImportGMMBtn(HWND hwnd);
static void DelVoiceFile(void);


BOOL CALLBACK MainDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{  
	switch (message)
	{
	case WM_INITDIALOG:
		OnInitDlg(hwnd);
		return TRUE;

	case WM_RECORD_BEG:
		OnRecordBeg(hwnd, lParam);
		return TRUE;

	case WM_RECORD_END:
		OnRecordEnd(hwnd);
		return TRUE;

	case WM_BUILD_GMM:  // 建立GMM及生成识别阈值
		// 先结束录音
		OnStopVoice(hwnd);
        OnBuildGMM(hwnd);
		DelVoiceFile();
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_VOICE_STOP:
			OnStopVoice(hwnd);
			DelVoiceFile();
			return TRUE;

		case IDC_CLEAR_TEXT:
			OnClearBtn(hwnd);
			return TRUE;

		case IDC_RESET:
			OnResetBtn(hwnd);
			return TRUE;

		case IDC_IMPORT_GMM:
			OnImportGMMBtn(hwnd);
			return TRUE;

		case IDC_EXPORT_GMM:
			OnExportGMMBtn(hwnd);
			return TRUE;

		case IDC_BUILD_GMM: // 提取声纹按钮
			UpdateDlg(hwnd, IDC_BUILD_GMM);
			OnBuildBtn(hwnd);
			return TRUE;

		case IDC_IDENTIFY: // 身份验证
			UpdateDlg(hwnd, IDC_IDENTIFY);
			OnIdentifyBtn(hwnd);
			return TRUE;

		case IDCANCEL:    // 关闭对话框
			g_buildGMM = FALSE;
			my_voiceprint.voice_mdl.gmm.f_FreeGMM(&my_voiceprint.voice_mdl.gmm);
			QuitMyWindow();
			return TRUE;
		} // switch (LOWORD(wParam))
		break;

	case WM_SYSCOMMAND:
		switch (LOWORD(wParam))
		{
		case SC_CLOSE:
			SendMessage(hwnd, WM_COMMAND, IDCANCEL, 0);
			return TRUE;
		}
		break;
	}

	return FALSE;
}


static BOOL OnBuildGMM(HWND hwnd)
{
	int buff_num = g_build_buff_num;


	MessageBox(hwnd, TEXT("语音已经采集！"), szAppVersion, 0);
	// 文件探测
	my_voiceprint.voice_mdl.voice_file.f_CheckFileSize(&my_voiceprint.voice_mdl.voice_file);
	if (my_voiceprint.voice_mdl.voice_file.file_size == -1)
	{
		MessageBox(hwnd, TEXT("声纹提取文件不存在！"), szAppVersion, 0);
		return FALSE;
	}
	else if ((!my_voiceprint.voice_mdl.bValued && my_voiceprint.voice_mdl.voice_file.file_size<BUFFLEN*buff_num) 
		|| (my_voiceprint.voice_mdl.bValued && my_voiceprint.voice_mdl.voice_file.file_size<BUFFLEN*IDENTIFY_BUFF_NUM))
	{
		MessageBox(hwnd, TEXT("语音数据过少！"), szAppVersion, 0);
		return FALSE;
	}

	if (my_voiceprint.voice_mdl.f_GetUsrGMM(&my_voiceprint, my_voiceprint.voice_mdl.bValued ? E_IDENTIFY : E_BUILD))
	{	
		if (!my_voiceprint.voice_mdl.bValued)
		{
			my_voiceprint.voice_mdl.bValued = TRUE;
			MessageBox(hwnd, TEXT("建模成功！"), szAppVersion, 0);
		}
		else
		{
			VoicePrintIdentify();
		}
	}
	else
	{
		MessageBox(hwnd, TEXT("建模失败！"), szAppVersion, 0);
	}

	return TRUE;
}

static void OnClearBtn(HWND hwnd)
{
	SetDlgItemText(hwnd, IDC_TEXTOUT, TEXT(""));
	SetDlgItemText(hwnd, IDC_GMM_NUM, TEXT(""));
	SetDlgItemText(hwnd, IDC_IDENTIFY_NUM, TEXT(""));
	SetDlgItemText(hwnd, IDC_M_NUM, TEXT(""));
}

static void OnInitDlg(HWND hwnd)
{
	// 初始化文件对话框
	my_voiceprint.voice_mdl.voice_file.f_FileInitialize(hwnd);
	SetWindowText(hwnd, TEXT("声纹识别"));
	SetupDlg(hwnd);
}

static void SetupDlg(HWND hwnd)
{
	SetDlgItemInt(hwnd, IDC_IDENTIFY_NUM, my_voiceprint.voice_mdl.percent, FALSE);
	SetDlgItemInt(hwnd, IDC_GMM_NUM, my_voiceprint.voice_mdl.frame_num, FALSE);
	SetDlgItemInt(hwnd, IDC_M_NUM, my_voiceprint.voice_mdl.m, FALSE);
}

static void UpdateDlg(HWND hwnd, LPARAM lParam)
{
	int m = 0;
	int percent = 0;
	int frame_num = 0;
	BOOL bFlag;

	percent = GetDlgItemInt(hwnd, IDC_IDENTIFY_NUM, &bFlag, FALSE);
	if (bFlag && percent > 0)
	{
		my_voiceprint.voice_mdl.percent = percent;
		if (lParam == IDC_IDENTIFY)
		{
			SetupDlg(hwnd);
			return;
		}
	}
	m = GetDlgItemInt(hwnd, IDC_M_NUM, &bFlag, FALSE);
	if (bFlag && m > 0)
	{
		my_voiceprint.voice_mdl.m = m;
	}
	frame_num = GetDlgItemInt(hwnd, IDC_GMM_NUM, &bFlag, FALSE);
	if (bFlag && frame_num > 0)
	{
		my_voiceprint.voice_mdl.frame_num = frame_num;
	}
	SetupDlg(hwnd);
	g_build_buff_num = (frame_num/2+50) / (BUFFLEN/FRAME_LEN);
}

static void OnResetBtn(HWND hwnd)
{
	my_voiceprint.g_hRecThread = NULL;
	my_voiceprint.g_RecThreadID = 0;
	g_buildGMM = FALSE;
	my_voiceprint.voice_mdl.bValued = FALSE;
	my_voiceprint.voice_mdl.gmm.f_FreeGMM(&my_voiceprint.voice_mdl.gmm);
	my_voiceprint.f_Init(&my_voiceprint);
	my_voiceprint.hwnd = hwnd;
	OnClearBtn(hwnd);
	SetupDlg(hwnd);
}

static void OnStopVoice(HWND hwnd)
{
	g_buildGMM = FALSE;
	SendMessage(hwnd, WM_RECORD_END, 0, 0);
	if (my_voiceprint.voice_mdl.bValued)
	{
		SetWindowText(GetDlgItem(hwnd, IDC_VOICE_STOP), TEXT("身份验证"));
		SetWindowLong(GetDlgItem(hwnd, IDC_VOICE_STOP), GWL_ID, IDC_IDENTIFY);
	}
	else
	{
		SetWindowText(GetDlgItem(hwnd, IDC_VOICE_STOP), TEXT("提取声纹"));
		SetWindowLong(GetDlgItem(hwnd, IDC_VOICE_STOP), GWL_ID, IDC_BUILD_GMM);	
	}
}

static void VoicePrintIdentify()
{
	int i;
	int temp_num = 0;
	double temp = 0;
	double percent = my_voiceprint.voice_mdl.percent / 100.0;
	HWND hwndPrint = NULL;
	TCHAR * res; // 识别结果字符串

	hwndPrint = GetDlgItem(my_voiceprint.hwnd, IDC_TEXTOUT);
	EditPrintf(hwndPrint, TEXT("声纹识别结果:\r\n"));
	for (i = 0; i < IDENTIFY_NUM; ++i)
	{
		temp = (1 + percent) * my_voiceprint.voice_mdl.value[i][0];
		EditPrintf(hwndPrint, TEXT("第%d个%d帧:  最大识别值: %lf\r\n识别值: %lf\r\n"), 
			i+1, my_voiceprint.rec_frame[i], temp, my_voiceprint.voice_mdl.value[i][1]);
		temp >= my_voiceprint.voice_mdl.value[i][1] ? ++temp_num : temp_num;
	}
	res = temp_num>(IDENTIFY_NUM-1)?TEXT("识别成功！"):TEXT("未识别！");
	EditPrintf(hwndPrint, TEXT("识别次数: %d, %s\r\n\r\n"), temp_num, res);
}

static void OnRecordBeg(HWND hwnd, LPARAM lParam)
{
	if (!my_voiceprint.myRecord && !my_voiceprint.g_RecThreadID)
	{
		my_voiceprint.myRecord = CreateRecord(hwnd);
		SetWindowText(GetDlgItem(hwnd, lParam), TEXT("停止"));
		SetWindowLong(GetDlgItem(hwnd, lParam), GWL_ID, IDC_VOICE_STOP);
	}
}

static void OnRecordEnd(HWND hwnd)
{
	if (my_voiceprint.myRecord && my_voiceprint.g_RecThreadID)
	{
		// 向录音线程发送结束消息
		PostThreadMessage(my_voiceprint.g_RecThreadID, WM_RECORD_END, 0, 0);
		my_voiceprint.myRecord = NULL;
	}
}

static void OnBuildBtn(HWND hwnd)
{
	if (!g_buildGMM && !my_voiceprint.voice_mdl.bValued)
	{
		/* 启动录音 */
		SendMessage(hwnd, WM_RECORD_BEG, 0, IDC_BUILD_GMM);
		MessageBox(hwnd, TEXT("建模开始\n开始录音..."), szAppVersion, 0);
		sprintf(my_voiceprint.voice_mdl.voice_file.szFileName, "C:\\建模%I64d.data", time(NULL));
		my_voiceprint.voice_mdl.file_stream = fopen(my_voiceprint.voice_mdl.voice_file.szFileName, "wb");
		if (!my_voiceprint.voice_mdl.file_stream) // 失败
		{
			MessageBox(hwnd, TEXT("创建转储文件失败"), szAppVersion, 0);
			exit(0);
		}
		my_voiceprint.voice_mdl.pVoice = (TBYTE *)malloc(BUFFLEN * 5);
		if (!my_voiceprint.voice_mdl.pVoice) // 失败
		{
			MessageBox(hwnd, TEXT("申请语音临时内存失败"), szAppVersion, 0);
			exit(0);
		}
		g_buildGMM = TRUE;
	}
	else if (my_voiceprint.voice_mdl.bValued) // 已经建模过	
	{
		MessageBox(hwnd, TEXT("已有声纹模型，请导出后重置！"), szAppVersion, 0);
	}
}

static void OnIdentifyBtn(HWND hwnd)
{
	if (!g_buildGMM && !my_voiceprint.voice_mdl.bValued)
	{
		MessageBox(hwnd, TEXT("请先提取或导入声纹！"), szAppVersion, 0);
	}
	else if (g_buildGMM && !my_voiceprint.voice_mdl.bValued)
	{
		MessageBox(hwnd, TEXT("正在提取声纹！"), szAppVersion, 0);
	}
	else  if (!g_buildGMM && my_voiceprint.voice_mdl.bValued)
	{
		SendMessage(hwnd, WM_RECORD_BEG, 0, IDC_IDENTIFY);
		MessageBox(hwnd, TEXT("识别开始\n 开始录音..."), szAppVersion, 0);
		sprintf(my_voiceprint.voice_mdl.voice_file.szFileName, "C:\\识别%I64d.data", time(NULL));
		my_voiceprint.voice_mdl.file_stream = fopen(my_voiceprint.voice_mdl.voice_file.szFileName, "wb");
		if (!my_voiceprint.voice_mdl.file_stream) // 失败
		{
			MessageBox(hwnd, TEXT("创建转储文件失败"), szAppVersion, 0);
			exit(0);
		}
		my_voiceprint.voice_mdl.pVoice = (TBYTE *)malloc(BUFFLEN * 5);
		if (!my_voiceprint.voice_mdl.pVoice) // 失败
		{
			MessageBox(hwnd, TEXT("申请语音临时内存失败"), szAppVersion, 0);
			exit(0);
		}
		g_buildGMM = TRUE;
	}
}

static void OnExportGMMBtn(HWND hwnd)
{
	int i;
	FILE * fsave = NULL;
	VOICE_MODEL * p_data = NULL;
	FILE_CLASS save_file = {FileClassInit};

	if (!my_voiceprint.voice_mdl.bValued)
	{
		MessageBox(hwnd, TEXT("尚无声纹模型！"), szAppVersion, 0);
		return;
	}

	save_file.f_Init(&save_file);
	save_file.f_FileInitialize(hwnd);
	save_file.f_FileSaveDlg(hwnd, &save_file);
	fsave = fopen(save_file.szFileName, "wb");
	if (!fsave)
	{
		MessageBox(hwnd, TEXT("创建文件失败！"), szAppVersion, 0);
		return;
	}
	p_data = &my_voiceprint.voice_mdl;

	// 写入GMM模型数据
	fwrite(&p_data->frame_num, sizeof(p_data->frame_num), 1, fsave);
	fwrite(&p_data->m, sizeof(p_data->m), 1, fsave);
	fwrite(&p_data->percent, sizeof(p_data->percent), 1, fsave);
	fwrite(p_data->gmm.p, sizeof(p_data->gmm.p[0]), p_data->m, fsave);
	for (i = 0; i < p_data->m; ++i)
	{
		fwrite(p_data->gmm.u[i], sizeof(p_data->gmm.u[0][0]), D, fsave);
	}
	for (i = 0; i < p_data->m; ++i)
	{
		fwrite(p_data->gmm.cMatrix[i], sizeof(p_data->gmm.cMatrix[0][0]), D, fsave);
	}

	// 写入识别阈值
	for (i = 0; i < IDENTIFY_NUM; ++i)
	{
		fwrite(&p_data->value[i][0], sizeof(p_data->value[0][0]), 1, fsave);
	}
	fclose(fsave);
	MessageBox(hwnd, TEXT("保存成功！"), szAppVersion, 0);
}


static void OnImportGMMBtn(HWND hwnd)
{
	int i;
	FILE * f_open = NULL;
	VOICE_MODEL * p_data = NULL;
	FILE_CLASS open_file = {FileClassInit};

	open_file.f_Init(&open_file);
	open_file.f_FileInitialize(hwnd);
	open_file.f_FileOpenDlg(hwnd, &open_file);
	open_file.f_CheckFileSize(&open_file);
	if (open_file.file_size == -1)
	{
		MessageBox(hwnd, TEXT("找不到文件！"), szAppVersion, 0);
		return;
	}
	//else if (open_file.file_size < 2044)  // 2044是当M=6，D=20时的GMM保存文件大小
	//{
	//	MessageBox(hwnd, TEXT("文件已损坏！"), szAppVersion, 0);
	//	return;
	//}
	f_open = fopen(open_file.szFileName, "rb");
	if (!f_open)
	{
		MessageBox(hwnd, TEXT("打开文件失败！"), szAppVersion, 0);
		return;
	}
	p_data = &my_voiceprint.voice_mdl;
	p_data->gmm.f_FreeGMM(&p_data->gmm);

	// 读入GMM模型数据
	fread(&p_data->frame_num, sizeof(p_data->frame_num), 1, f_open);
	fread(&p_data->m, sizeof(p_data->m), 1, f_open);
	fread(&p_data->percent, sizeof(p_data->percent), 1, f_open);
	p_data->gmm.f_Init(&p_data->gmm, p_data->m);
	p_data->gmm.f_CallocGMM(&p_data->gmm);
	fread(p_data->gmm.p, sizeof(p_data->gmm.p[0]), p_data->m, f_open);
	for (i = 0; i < p_data->m; ++i)
	{
		fread(p_data->gmm.u[i], sizeof(p_data->gmm.u[0][0]), D, f_open);
	}
	for (i = 0; i < p_data->m; ++i)
	{
		fread(p_data->gmm.cMatrix[i], sizeof(p_data->gmm.cMatrix[0][0]), D, f_open);
	}

	// 读入识别阈值
	for (i = 0; i < IDENTIFY_NUM; ++i)
	{
		fread(&p_data->value[i][0], sizeof(p_data->value[0][0]), 1, f_open);
	}
	fclose(f_open);
	p_data->bValued = TRUE;
	//-------------------------------------------------------------------------------------------------
	// 刷新界面显示
	//
	OnClearBtn(hwnd);
	PrintThresholdValue(hwnd);
	SetupDlg(hwnd);
	//MessageBox(hwnd, TEXT("导入成功！"), szAppVersion, 0);
}


/* 删除临时语音转储文件 */
static void DelVoiceFile(void)
{
	// 文件探测
	my_voiceprint.voice_mdl.voice_file.f_CheckFileSize(&my_voiceprint.voice_mdl.voice_file);
	if (my_voiceprint.voice_mdl.voice_file.file_size == -1)
	{
		return;
	}
	if (my_voiceprint.voice_mdl.file_stream)
	{
		fclose(my_voiceprint.voice_mdl.file_stream);
	}
	if (remove(my_voiceprint.voice_mdl.voice_file.szFileName))
	{
		xprint(TEXT("删除文件失败:%s"), my_voiceprint.voice_mdl.voice_file.szFileName);
	}
}