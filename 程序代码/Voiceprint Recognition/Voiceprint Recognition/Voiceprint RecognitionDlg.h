
// Voiceprint RecognitionDlg.h : 头文件
//

#pragma once
#include <io.h>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include "resource.h"
#include "CChineseCode.h"
#include "WaveRecorder.h"
#include "Shockwaveflash.h"
#include "VPR/WavData_CharaParameter.h"

struct FILESTRUCT;

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

	vector<FILESTRUCT> voiceLib;
	vector<FILESTRUCT> wavLib;

	bool flagRecord;                                                         //用于标识当前状态，如果flag=true则表示当前处于录音状态 flag=false则表示当前属于空白状态

	void CompoundFile(vector<FILESTRUCT>& fileLib, int flag);                //用于将txt信息与当前文件夹下内容相结合
	int GetItemSelect(int index);                                            //获取某个listControl当前选中项的行号

	bool OnButton1_record(char* fileName);                                   //开启录音线程
	bool OnButton1_cancel();                                                 //结束录音

	bool OnButton4_refresh();                                                //录音文件刷新
	bool OnButton5_refresh();                                                //模型文件刷新

public:
	afx_msg void OnBnClickedButton1();                                       //录音
	afx_msg void OnBnClickedButton2();                                       //训练
	afx_msg void OnBnClickedButton3();                                       //识别
	afx_msg void OnBnClickedButton4();                                       //刷新
	afx_msg void OnBnClickedButton5();                                       //刷新
};

typedef struct FILESTRUCT
{
	string fileName;
	string peopleName;
	FILESTRUCT() {}
	FILESTRUCT(string fileName, string peopleName) {
		this->fileName = fileName;
		this->peopleName = peopleName;
	}
}FILESTRUCT;

extern WaveRecorder waveRecorder;                                            //全局录音对象
extern char* fileName;                                                       //文件对比之用
extern string fileName_t;                                                    //文件对比之用
extern double* mfccData;                                                     //用于保存当前语音训练出的数据
extern CharaParameter* charaParameter;                                       //用于保存当前语音训练出的参数

void* record(void* args);                                                    //录音线程
string getFileName(string path);                                             //将某个路径转换为某个文件名
void getFiles(string path, vector<string>& files);                           //获取path文件夹下的所有文件名
void readList(ifstream& in, vector<FILESTRUCT>& list);                       //读取文件的内容到list中
void writeList(ofstream& out, vector<FILESTRUCT>& list);                     //将list文件内容写入数据流

bool extractParameter(string wavfilePath);                                   //训练目标路径的语音文件的特征参数
bool trainingWAV(string wavfilePath, string gmmfilePath);                    //训练wav文件
int voiceprintRecognition(string rootPath, vector<FILESTRUCT> voiceLib);     //声纹识别