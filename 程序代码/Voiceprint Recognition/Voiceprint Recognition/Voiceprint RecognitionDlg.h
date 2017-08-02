
// Voiceprint RecognitionDlg.h : 头文件
//

#pragma once
#include <pthread.h>
#include "resource.h"
#include "WaveRecorder.h"
#include "Shockwaveflash.h"

// CVoiceprintRecognitionDlg 对话框
class CVoiceprintRecognitionDlg : public CDialogEx
{
// 构造
public:
	CVoiceprintRecognitionDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_VOICEPRINTRECOGNITION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CShockwaveflash flashshow;
	CListCtrl listCtrl_1;
	CListCtrl listCtrl_2;
	CButton buttonCtrl_1;

	pthread_t thread_recordID;                                               //录音线程ID

	bool flagRecord;                                                         //用于标识当前状态，如果flag=true则表示当前处于录音状态 flag=false则表示当前属于空白状态

	bool OnButton1_record(char* fileName);                                   //开启录音线程
	bool OnButton1_cancel();                                                 //结束录音

public:
	afx_msg void OnBnClickedButton1();                                       //录音
	afx_msg void OnBnClickedButton2();                                       //训练
	afx_msg void OnBnClickedButton3();                                       //识别
	afx_msg void OnBnClickedButton4();                                       //刷新
	afx_msg void OnBnClickedButton5();                                       //刷新
};

extern WaveRecorder waveRecorder;
extern char* fileName;

void* record(void* args);                                                    //录音线程
