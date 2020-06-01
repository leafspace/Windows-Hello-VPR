
// Voiceprint RecognitionDlg.h : 头文件
//

#pragma once
#include <io.h>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include "resource.h"
#include "EncodeSolution/CChineseCode.h"
#include "RecordSolution/WaveRecorder.h"
#include "LogSolution/LogSystem.h"
#include "FlashSolution/Shockwaveflash.h"
#include "VPRSolution/WavData_CharaParameter.h"

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

class CVoiceprintRecognitionDlg : public CDialogEx
{
public:
	CVoiceprintRecognitionDlg(CWnd* pParent = NULL);
	enum { IDD = IDD_VOICEPRINTRECOGNITION_DIALOG };
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonRecord();                                  //录音
	afx_msg void OnBnClickedButtonTraining();                                //训练
	afx_msg void OnBnClickedButtonRecognition();                             //识别
	afx_msg void OnBnClickedButtonRefreshVoiceFile();                        //刷新
	afx_msg void OnBnClickedButtonRefreshVoiceLib();                         //刷新
	afx_msg void OnBnClickedButtonTraningData();
	afx_msg void OnNMDblclkListVoiceFile(NMHDR* pNMHDR, LRESULT* pResult);

private:
	CShockwaveflash m_flashCtrl;
	CListCtrl m_listCtrl_VoiceFile;
	CListCtrl m_listCtrl_VoiceLib;

	pthread_t thread_recordID;                                               //录音线程ID
	pthread_t thread_playerID;                                               //播放线程ID

	vector<FILESTRUCT> voiceLib;
	vector<FILESTRUCT> wavLib;

	bool flagRecord = false;                                                 //用于标识当前状态，如果flag=true则表示当前处于录音状态 flag=false则表示当前属于空白状态

	void CompoundFile(vector<FILESTRUCT>& fileLib, int flag);                //用于将txt信息与当前文件夹下内容相结合
	int GetItemSelect(int index);                                            //获取某个listControl当前选中项的行号

public:
	void OnLoadXActiveFlash(void);											 //加载Flash动画
	void OnLoadTableHead(void);
	bool OnButton_record(char* fileName);                                    //开启录音线程
	bool OnButton_cancel();                                                  //结束录音
};

extern WaveRecorder waveRecorder;                                            //全局录音对象
extern char* fileName;                                                       //文件对比之用
extern string fileName_t;                                                    //文件对比之用
extern string playpath;                                                      //保存需要播放的语音的路径
extern double* mfccData;                                                     //用于保存当前语音训练出的数据
extern CharaParameter* charaParameter;                                       //用于保存当前语音训练出的参数

void* record(void* args);                                                    //录音线程
void* player(void* args);                                                    //放音线程
string getFileName(string path);                                             //将某个路径转换为某个文件名
void getFiles(string path, vector<string>& files);                           //获取path文件夹下的所有文件名
void readList(ifstream& in, vector<FILESTRUCT>& list);                       //读取文件的内容到list中
void writeList(ofstream& out, vector<FILESTRUCT>& list);                     //将list文件内容写入数据流

bool extractParameter(string wavfilePath);                                   //训练目标路径的语音文件的特征参数
bool trainingWAV(string wavfilePath);                                        //训练wav文件，将当前语音计算出gmm模型，再将模型数据放入模型中，计算出概率值
bool trainingWAV(string wavfilePath, string gmmfilePath);                    //训练wav文件
int voiceprintRecognition(string rootPath, vector<FILESTRUCT> voiceLib);     //声纹识别