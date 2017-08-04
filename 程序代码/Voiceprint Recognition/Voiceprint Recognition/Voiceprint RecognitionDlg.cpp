
// Voiceprint RecognitionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Voiceprint Recognition.h"
#include "Voiceprint RecognitionDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CVoiceprintRecognitionDlg 对话框




CVoiceprintRecognitionDlg::CVoiceprintRecognitionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVoiceprintRecognitionDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVoiceprintRecognitionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SHOCKWAVEFLASH1, flashshow);
	DDX_Control(pDX, IDC_LIST1, listCtrl_1);
	DDX_Control(pDX, IDC_LIST2, listCtrl_2);
	DDX_Control(pDX, IDC_BUTTON1, buttonCtrl_1);
}

BEGIN_MESSAGE_MAP(CVoiceprintRecognitionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CVoiceprintRecognitionDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CVoiceprintRecognitionDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CVoiceprintRecognitionDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CVoiceprintRecognitionDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CVoiceprintRecognitionDlg::OnBnClickedButton5)
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
	CRect rectCtrl;
	GetDlgItem(IDC_SHOCKWAVEFLASH1)->GetWindowRect(&rectCtrl);
	rectCtrl.left = 35;
	rectCtrl.right = 217;
	rectCtrl.top = 45;
	rectCtrl.bottom = 200;
	flashshow.MoveWindow(&rectCtrl, true);
	TCHAR strCurDrt[500];
	int nLen = ::GetCurrentDirectory(500, strCurDrt);
	if (strCurDrt[nLen] != '\\') {
		strCurDrt[nLen++] = '\\';
		strCurDrt[nLen] = '\0';
	}
	CString strFileName = strCurDrt;
	strFileName += "flash.swf";
	this->flashshow.LoadMovie(0, strFileName);
	this->flashshow.Play();
	this->flashshow.Stop();
	SetWindowPos(&this->flashshow,0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);


	CRect rect;
	this->listCtrl_1.GetHeaderCtrl()->EnableWindow(false);                   //固定标题不被移动
	listCtrl_1.GetClientRect(&rect);                                         //获取编程语言列表视图控件的位置和大小
	listCtrl_1.SetExtendedStyle(listCtrl_1.GetExtendedStyle()
		| LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);                          //为列表视图控件添加全行选中和栅格风格

	listCtrl_1.InsertColumn(0, _T("文件名"), LVCFMT_CENTER, rect.Width() / 2, 0);
	listCtrl_1.InsertColumn(1, _T("录音人"), LVCFMT_CENTER, rect.Width() / 2, 1);

	this->listCtrl_2.GetHeaderCtrl()->EnableWindow(false);                   //固定标题不被移动
	listCtrl_2.GetClientRect(&rect);                                         //获取编程语言列表视图控件的位置和大小
	listCtrl_2.SetExtendedStyle(listCtrl_2.GetExtendedStyle()
		| LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);                          //为列表视图控件添加全行选中和栅格风格

	listCtrl_2.InsertColumn(0, _T("文件名"), LVCFMT_CENTER, rect.Width() / 2, 0);
	listCtrl_2.InsertColumn(1, _T("所属人"), LVCFMT_CENTER, rect.Width() / 2, 1);


	this->flagRecord = false;

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

void CVoiceprintRecognitionDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	if (flagRecord) {
		this->OnButton1_cancel();
		this->flashshow.Stop();
		this->flagRecord = false;
		this->OnBnClickedButton4();
		this->OnBnClickedButton5();
		SetDlgItemText(IDC_BUTTON1, (CString)"录音");
		SetDlgItemText(IDC_EDIT1, (CString)"");
	} else {
		bool success = false;
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
				fileNameTemp[index - 3] != 'w' || fileNameTemp[index - 4] != '.' ) {
					fileNameTemp[index    ] = '.';
					fileNameTemp[index + 1] = 'w';
					fileNameTemp[index + 2] = 'a';
					fileNameTemp[index + 3] = 'v';
					fileNameTemp[index + 4] = 0;
			}

			::fileName_t = fileNameTemp;

			success = this->OnButton1_record(fileNameTemp);                  //赋文件名给线程操作函数
			delete fileNameChar;
			//delete fileNameTemp;                                           //交给初始化完录音后清空
		}
		if (success) {
			SetDlgItemText(IDC_BUTTON1, (CString)"停止");
			this->flashshow.Play();
			this->flagRecord = true;
		}
	}
}


void CVoiceprintRecognitionDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	int selectIndex = this->GetItemSelect(0);
	FILESTRUCT selectItem = this->wavLib[selectIndex];

	char szModuleFilePath[MAX_PATH];
	int n = GetModuleFileNameA(0, szModuleFilePath, MAX_PATH);               //获得当前执行文件的路径
	szModuleFilePath[strrchr(szModuleFilePath, '\\') - szModuleFilePath + 1] = 0;      //将最后一个"\\"后的字符置为0
	
	int index = 0;
	char filePath[MAX_PATH];
	for (int i = 0; i < (int) strlen(szModuleFilePath); ++i) {               //补全//
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

	trainingWAV(wavfilePath, gmmfilePath);

	char infofilePath[MAX_PATH];
	strcpy_s(infofilePath, filePath);
	strcat_s(infofilePath, "voiceLib\\\\");
	strcat_s(infofilePath, "info.list");
	ofstream out(infofilePath, ios::app);
	if (out.is_open()) {
		string str_f, str_p;
		CChineseCode::GB2312ToUTF_8(str_f, (char*) getFileName(gmmfilePath).data(), getFileName(gmmfilePath).length());
		CChineseCode::GB2312ToUTF_8(str_p, (char*) selectItem.peopleName.data(), selectItem.peopleName.length());
		out << str_f.data() << "\t" << str_p.data() << endl;
	}
	out.close();

	this->OnBnClickedButton5();
}


void CVoiceprintRecognitionDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	int selectIndex = this->GetItemSelect(1);
}


void CVoiceprintRecognitionDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	this->CompoundFile(this->wavLib, 0);
	this->OnButton4_refresh();
}


void CVoiceprintRecognitionDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	this->CompoundFile(this->voiceLib, 1);
	this->OnButton5_refresh();
}
