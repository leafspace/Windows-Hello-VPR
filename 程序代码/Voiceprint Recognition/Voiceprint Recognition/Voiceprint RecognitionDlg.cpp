
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

