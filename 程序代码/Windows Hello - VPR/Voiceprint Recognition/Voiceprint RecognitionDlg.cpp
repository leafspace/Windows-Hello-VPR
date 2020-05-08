
// Voiceprint RecognitionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Voiceprint Recognition.h"
#include "Voiceprint RecognitionDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LogSystem *g_pLogSystem = NULL;                                                      //用于处理Log相关

//======================================================================================
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
	enum { IDD = IDD_ABOUTBOX };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
protected:
	DECLARE_MESSAGE_MAP()
};
CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

//======================================================================================
CVoiceprintRecognitionDlg::CVoiceprintRecognitionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVoiceprintRecognitionDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVoiceprintRecognitionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SHOCKWAVEFLASH, m_flashCtrl);
	DDX_Control(pDX, IDC_LIST_VOICEFILE, m_listCtrl_VoiceFile);
	DDX_Control(pDX, IDC_LIST_VOICELIB, m_listCtrl_VoiceLib);
}

BEGIN_MESSAGE_MAP(CVoiceprintRecognitionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_VOICEFILE, &CVoiceprintRecognitionDlg::OnNMDblclkListVoiceFile)
	ON_BN_CLICKED(IDC_BUTTON_RECORD, &CVoiceprintRecognitionDlg::OnBnClickedButtonRecord)
	ON_BN_CLICKED(IDC_BUTTON_TRAINING, &CVoiceprintRecognitionDlg::OnBnClickedButtonTraining)
	ON_BN_CLICKED(IDC_BUTTON_RECOGNITION, &CVoiceprintRecognitionDlg::OnBnClickedButtonRecognition)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH_VOICEFILE, &CVoiceprintRecognitionDlg::OnBnClickedButtonRefreshVoiceFile)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH_VOICELIB, &CVoiceprintRecognitionDlg::OnBnClickedButtonRefreshVoiceLib)
	ON_BN_CLICKED(IDC_BUTTON_TRANING_DATA, &CVoiceprintRecognitionDlg::OnBnClickedButtonTraningData)
END_MESSAGE_MAP()


// CVoiceprintRecognitionDlg 消息处理程序

BOOL CVoiceprintRecognitionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。
	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


	// TODO: 在此添加额外的初始化代码
	this->flagRecord = false;

	this->OnLoadXActiveFlash();
	this->OnLoadTableHead();

	this->OnBnClickedButtonRefreshVoiceFile();
	this->OnBnClickedButtonRefreshVoiceLib();

	if (g_pLogSystem != NULL)
	{
		delete g_pLogSystem;
		g_pLogSystem = NULL;
	}

	g_pLogSystem = new LogSystem();                                           //初始化日志系统
	g_pLogSystem->linkState = g_pLogSystem->initSocket();
	g_pLogSystem->beginSystem();                                              //开启线程发送消息

	g_pLogSystem->sendMessage("<Type>\n");                                    //发送类型消息
	g_pLogSystem->sendMessage("Windows Hello Client\n");
	g_pLogSystem->writeMessage("==============================================\n");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CVoiceprintRecognitionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CVoiceprintRecognitionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CVoiceprintRecognitionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVoiceprintRecognitionDlg::OnBnClickedButtonRecord()
{
	// TODO: 在此添加控件通知处理程序代码
	if (flagRecord) {
		//LogSystem send message
		if (g_pLogSystem->linkState) {
			g_pLogSystem->sendMessage("<Message>\n");
			g_pLogSystem->sendMessage("Info : Stop record ! \n");
		}
		g_pLogSystem->writeMessage("Info : Stop record ! \n");

		this->OnButton_cancel();
		this->m_flashCtrl.Stop();
		this->flagRecord = false;
		this->OnBnClickedButtonRefreshVoiceFile();
		this->OnBnClickedButtonRefreshVoiceLib();
		SetDlgItemText(IDC_BUTTON_RECORD, _T("录音"));
		SetDlgItemText(IDC_EDIT_RECORD_PEOPLE, _T(""));
	}
	else {
		//LogSystem send message
		if (g_pLogSystem->linkState) {
			g_pLogSystem->sendMessage("<Message>\n");
			g_pLogSystem->sendMessage("Info : Begin record ! \n");
		}
		g_pLogSystem->writeMessage("Info : Begin record ! \n");

		//弹出窗口并显示
		CFileDialog opendlg(FALSE, _T("*.wav"), _T("*.wav"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("所有文件(*.wav*;)|*.wav*||"), NULL);   //打开文件选择框
		if (opendlg.DoModal() == IDOK)
		{
			CString fileName = opendlg.GetPathName();                        //获取选择的文件名
			int nameLen = WideCharToMultiByte(CP_ACP, 0, fileName, -1, NULL, 0, NULL, NULL);
			char *fileNameChar = new char[nameLen + 1];
			WideCharToMultiByte(CP_ACP, 0, fileName, -1, fileNameChar, nameLen, NULL, NULL);     //将CString转为char*
			char *fileNameTemp = new char[nameLen + 20];

			int index = 0;                                                   //用于保存新文件名长度
			for (int i = 0; i < nameLen + 1; ++i) {                          //处理'\'为'\\'，若文件中的路径分隔符为'\'则无法准确定位
				fileNameTemp[index++] = fileNameChar[i];
				if (fileNameChar[i] == '\\') {
					fileNameTemp[index++] = '\\';
				}
			}
			fileNameTemp[index] = 0;
			if (fileNameTemp[index - 1] != 'v' || fileNameTemp[index - 2] != 'a' ||
				fileNameTemp[index - 3] != 'w' || fileNameTemp[index - 4] != '.') {
				fileNameTemp[index] = '.';
				fileNameTemp[index + 1] = 'w';
				fileNameTemp[index + 2] = 'a';
				fileNameTemp[index + 3] = 'v';
				fileNameTemp[index + 4] = 0;
			}

			::fileName_t = fileNameTemp;

			bool bSuccess = this->OnButton_record(fileNameTemp);                  //赋文件名给线程操作函数
			delete fileNameChar;
			//delete fileNameTemp;                                           //交给初始化完录音后清空

			if (bSuccess) {
				SetDlgItemText(IDC_BUTTON_RECORD, _T("停止"));
				this->m_flashCtrl.Play();
				this->flagRecord = true;
			}
			else
			{
				g_pLogSystem->writeMessage("ERROR : Can't record ! \n");
			}
		}
		else
		{
			g_pLogSystem->writeMessage("Info : End record ! \n");
			g_pLogSystem->writeMessage("Info : Use not choose output file name ! \n");
		}
	}
}


void CVoiceprintRecognitionDlg::OnBnClickedButtonTraining()
{
	// TODO: 在此添加控件通知处理程序代码
	//LogSystem send message
	if (g_pLogSystem->linkState) {
		g_pLogSystem->sendMessage("<Message>\n");
		g_pLogSystem->sendMessage("Info : Begin traning user voice ! \n");
	}
	g_pLogSystem->writeMessage("Info : Begin traning user voice ! \n");

	int selectIndex = this->GetItemSelect(0);
	FILESTRUCT selectItem = this->wavLib[selectIndex];

	if (strcmp(selectItem.peopleName.data(), "未知") == 0 ||
		strcmp(selectItem.peopleName.data(), "unknow") == 0) {
		MessageBoxA(NULL, "未知的用户语音无法训练进入训练库", "错误", MB_ICONHAND);
		//LogSystem send message
		if (g_pLogSystem->linkState) {
			g_pLogSystem->sendMessage("<Message>\n");
			g_pLogSystem->sendMessage("ERROR : Unknow person voice can't into the libraly ! \n");
		}
		g_pLogSystem->writeMessage("ERROR : Unknow person voice can't into the libraly ! \n");
		return;
	}

	char szModuleFilePath[MAX_PATH];
	int n = GetModuleFileNameA(0, szModuleFilePath, MAX_PATH);               //获得当前执行文件的路径
	szModuleFilePath[strrchr(szModuleFilePath, '\\') - szModuleFilePath + 1] = 0;      //将最后一个"\\"后的字符置为0

	int index = 0;
	char filePath[MAX_PATH];
	for (int i = 0; i < (int)strlen(szModuleFilePath); ++i) {               //补全//
		filePath[index++] = szModuleFilePath[i];
		if (szModuleFilePath[i] == '\\') {
			filePath[index++] = '\\';
		}
	}
	filePath[index++] = 0;                                                   //末尾归零

	char wavfilePath[MAX_PATH], gmmfilePath[MAX_PATH];
	strcpy_s(wavfilePath, filePath);
	strcpy_s(gmmfilePath, filePath);
	strcat_s(wavfilePath, "wavLib\\\\");
	strcat_s(gmmfilePath, "trainLib\\\\");
	strcat_s(wavfilePath, selectItem.fileName.data());
	strcat_s(gmmfilePath, selectItem.peopleName.data());
	strcat_s(gmmfilePath, "-gmm(-).txt");

	if (::charaParameter != NULL) {
		delete ::charaParameter;
	}
	trainingWAV(wavfilePath, gmmfilePath);

	char infofilePath[MAX_PATH];
	strcpy_s(infofilePath, filePath);
	strcat_s(infofilePath, "voiceLib\\\\");
	strcat_s(infofilePath, "info.list");
	ofstream out(infofilePath, ios::app);
	if (out.is_open()) {
		string str_f, str_p;
		CChineseCode::GB2312ToUTF_8(str_f, (char*)getFileName(gmmfilePath).data(), getFileName(gmmfilePath).length());
		CChineseCode::GB2312ToUTF_8(str_p, (char*)selectItem.peopleName.data(), selectItem.peopleName.length());
		out << str_f.data() << "\t" << str_p.data() << endl;
	}
	out.close();

	this->OnBnClickedButtonRefreshVoiceLib();
}


void CVoiceprintRecognitionDlg::OnBnClickedButtonRecognition()
{
	// TODO: 在此添加控件通知处理程序代码
	char szModuleFilePath[MAX_PATH];
	int n = GetModuleFileNameA(0, szModuleFilePath, MAX_PATH);               //获得当前执行文件的路径
	szModuleFilePath[strrchr(szModuleFilePath, '\\') - szModuleFilePath + 1] = 0;      //将最后一个"\\"后的字符置为0

	int index = 0;
	char filePath[MAX_PATH];
	for (int i = 0; i < (int)strlen(szModuleFilePath); ++i) {               //补全//
		filePath[index++] = szModuleFilePath[i];
		if (szModuleFilePath[i] == '\\') {
			filePath[index++] = '\\';
		}
	}
	filePath[index++] = 0;
	char gmmfilePath[MAX_PATH];
	strcpy_s(gmmfilePath, filePath);
	strcat_s(gmmfilePath, "voiceLib\\\\");

	this->OnBnClickedButtonTraining();                                              //先训练目标数据

	int selectIndex = this->GetItemSelect(0);
	FILESTRUCT selectItem = this->wavLib[selectIndex];

	if (strcmp(selectItem.peopleName.data(), "未知") == 0 ||
		strcmp(selectItem.peopleName.data(), "unknow") == 0) {
		return;
	}

	//LogSystem send message
	if (g_pLogSystem->linkState) {
		g_pLogSystem->sendMessage("<Message>\n");
		g_pLogSystem->sendMessage("Info : Begin recognition ! \n");
	}
	g_pLogSystem->writeMessage("Info : Begin recognition ! \n");

	int countMax = voiceprintRecognition(gmmfilePath, this->voiceLib);

	char VPR_result[256] = "系统识别结果-说话人为：";
	strcat_s(VPR_result, this->voiceLib[countMax].peopleName.data());
	MessageBoxA(NULL, VPR_result, "信息", MB_ICONASTERISK);

	strcat_s(VPR_result, "\n");
	//LogSystem send message
	if (g_pLogSystem->linkState) {
		g_pLogSystem->sendMessage("<Message>\n");
		g_pLogSystem->sendMessage(VPR_result);
	}
	g_pLogSystem->writeMessage(VPR_result);
}


void CVoiceprintRecognitionDlg::OnBnClickedButtonRefreshVoiceFile()
{
	// TODO: 在此添加控件通知处理程序代码
	this->CompoundFile(this->wavLib, 0);
	

	CString str_f, str_p;
	m_listCtrl_VoiceFile.DeleteAllItems();
	for (int i = 0; i < (int)this->wavLib.size(); ++i) {
		FILESTRUCT item = this->wavLib[i];
		str_f = item.fileName.c_str();
		str_p = item.peopleName.c_str();
		m_listCtrl_VoiceFile.InsertItem(i, str_f);                                     //设置列表文件名信息
		m_listCtrl_VoiceFile.SetItemText(i, 1, str_p);                                 //设置列表用户信息
	}

}


void CVoiceprintRecognitionDlg::OnBnClickedButtonRefreshVoiceLib()
{
	// TODO: 在此添加控件通知处理程序代码
	this->CompoundFile(this->voiceLib, 1);
	
	CString str_f, str_p;
	m_listCtrl_VoiceLib.DeleteAllItems();
	for (int i = 0; i < (int)this->voiceLib.size(); ++i) {
		FILESTRUCT item = this->voiceLib[i];
		str_f = item.fileName.c_str();
		str_p = item.peopleName.c_str();
		m_listCtrl_VoiceLib.InsertItem(i, str_f);                                     //设置列表文件名信息
		m_listCtrl_VoiceLib.SetItemText(i, 1, str_p);;                                //设置列表用户信息
	}
}


void CVoiceprintRecognitionDlg::OnBnClickedButtonTraningData()
{
	// TODO: 在此添加控件通知处理程序代码
	//LogSystem send message
	if (g_pLogSystem->linkState) {
		g_pLogSystem->sendMessage("<Message>\n");
		g_pLogSystem->sendMessage("Info : Begin traning probalily voice ! \n");
	}
	g_pLogSystem->writeMessage("Info : Begin traning probalily voice ! \n");

	int selectIndex = this->GetItemSelect(0);
	FILESTRUCT selectItem = this->wavLib[selectIndex];

	char szModuleFilePath[MAX_PATH];
	int n = GetModuleFileNameA(0, szModuleFilePath, MAX_PATH);               //获得当前执行文件的路径
	szModuleFilePath[strrchr(szModuleFilePath, '\\') - szModuleFilePath + 1] = 0;      //将最后一个"\\"后的字符置为0

	int index = 0;
	char filePath[MAX_PATH];
	for (int i = 0; i < (int)strlen(szModuleFilePath); ++i) {               //补全//
		filePath[index++] = szModuleFilePath[i];
		if (szModuleFilePath[i] == '\\') {
			filePath[index++] = '\\';
		}
	}
	filePath[index++] = 0;                                                   //末尾归零

	char wavfilePath[MAX_PATH], gmmfilePath[MAX_PATH];
	strcpy_s(wavfilePath, filePath);
	strcpy_s(gmmfilePath, filePath);
	strcat_s(wavfilePath, "wavLib\\\\");
	strcat_s(gmmfilePath, "voiceLib\\\\");
	strcat_s(wavfilePath, selectItem.fileName.data());
	strcat_s(gmmfilePath, selectItem.peopleName.data());
	strcat_s(gmmfilePath, "-gmm(-).txt");

	if (::charaParameter != NULL) {
		delete ::charaParameter;
	}
	trainingWAV(wavfilePath);

	MessageBoxA(NULL, "TIP : Traning probalily data OK !", "TIP", MB_ICONASTERISK);
}


void CVoiceprintRecognitionDlg::OnNMDblclkListVoiceFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// TODO: 在此添加控件通知处理程序代码
	int selectIndex = this->GetItemSelect(0);
	if (selectIndex != -1) {
		FILESTRUCT selectItem = this->wavLib[selectIndex];
		char szModuleFilePath[MAX_PATH];
		int n = GetModuleFileNameA(0, szModuleFilePath, MAX_PATH);               //获得当前执行文件的路径
		szModuleFilePath[strrchr(szModuleFilePath, '\\') - szModuleFilePath + 1] = 0;      //将最后一个"\\"后的字符置为0

		int index = 0;
		char filePath[MAX_PATH];
		for (int i = 0; i < (int)strlen(szModuleFilePath); ++i) {               //补全//
			filePath[index++] = szModuleFilePath[i];
			if (szModuleFilePath[i] == '\\') {
				filePath[index++] = '\\';
			}
		}
		filePath[index++] = 0;
		char wavfilePath[MAX_PATH];
		strcpy_s(wavfilePath, filePath);
		strcat_s(wavfilePath, "wavLib\\\\");
		strcat_s(wavfilePath, selectItem.fileName.data());
		::playpath = wavfilePath;

		pthread_attr_t attr;                                                     //线程属性结构体，创建线程时加入的参数  
		pthread_attr_init(&attr);                                                //初始化
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);             //是设置你想要指定线程属性参数
		int ret = pthread_create(&thread_playerID, &attr, player, (void*)&fileName);
		if (ret != 0) {
			MessageBoxA(NULL, "ERROR : Can't Paly Sound !", "ERROR", MB_ICONHAND);
		}
	}

	*pResult = 0;
}

void CVoiceprintRecognitionDlg::OnLoadXActiveFlash(void)
{
	CRect rectCtrl(32, 45, 217, 200);
	m_flashCtrl.MoveWindow(&rectCtrl, true);
	
	TCHAR strCurDrt[500] = { 0 };
	int nLen = ::GetCurrentDirectory(500, strCurDrt);
	if (strCurDrt[nLen] != '\\') {
		strCurDrt[nLen++] = '\\';
		strCurDrt[nLen++] = 0;
	}

	CString strFileName = strCurDrt;
	strFileName += "flash.swf";
	this->m_flashCtrl.LoadMovie(0, strFileName);
	this->m_flashCtrl.Play();
	SetWindowPos(&this->m_flashCtrl, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void CVoiceprintRecognitionDlg::OnLoadTableHead(void)
{
	CRect rect;
	this->m_listCtrl_VoiceFile.GetHeaderCtrl()->EnableWindow(false);                   //固定标题不被移动
	m_listCtrl_VoiceFile.GetClientRect(&rect);                                         //获取编程语言列表视图控件的位置和大小
	m_listCtrl_VoiceFile.SetExtendedStyle(m_listCtrl_VoiceFile.GetExtendedStyle()
		| LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);                          //为列表视图控件添加全行选中和栅格风格

	m_listCtrl_VoiceFile.InsertColumn(0, _T("文件名"), LVCFMT_CENTER, rect.Width() / 2, 0);
	m_listCtrl_VoiceFile.InsertColumn(1, _T("录音人"), LVCFMT_CENTER, rect.Width() / 2, 1);

	this->m_listCtrl_VoiceLib.GetHeaderCtrl()->EnableWindow(false);                   //固定标题不被移动
	m_listCtrl_VoiceLib.GetClientRect(&rect);                                         //获取编程语言列表视图控件的位置和大小
	m_listCtrl_VoiceLib.SetExtendedStyle(m_listCtrl_VoiceLib.GetExtendedStyle()
		| LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);                          //为列表视图控件添加全行选中和栅格风格

	m_listCtrl_VoiceLib.InsertColumn(0, _T("文件名"), LVCFMT_CENTER, rect.Width() / 2, 0);
	m_listCtrl_VoiceLib.InsertColumn(1, _T("所属人"), LVCFMT_CENTER, rect.Width() / 2, 1);
}


bool CVoiceprintRecognitionDlg::OnButton_record(char* fileName)             //开启录音线程
{
	::fileName = fileName;
	pthread_attr_t attr;                                                     //线程属性结构体，创建线程时加入的参数  
	pthread_attr_init(&attr);                                                //初始化
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);             //是设置你想要指定线程属性参数
	int ret = pthread_create(&thread_recordID, &attr, record, (void*)&fileName);
	if (ret != 0) {
		MessageBoxA(NULL, "ERROR : Can't create thread !", "ERROR", MB_ICONHAND);
		return false;
	}
	return true;
}

bool CVoiceprintRecognitionDlg::OnButton_cancel()                           //结束录音
{
	::waveRecorder.Stop();
	::waveRecorder.Reset();
	pthread_cancel(this->thread_recordID);
	return true;
}