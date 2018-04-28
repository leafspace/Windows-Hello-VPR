#pragma once
#include <cmath>
#include <float.h>
#include "../MediaControl/Common/CommonStruct.h"
#include "../MediaControl/WAVFile/WavFileBase.h"

#define MUTEPARAGRAPH		0                                               // 静音段
#define INTERIMPARAGRAPH	1                                               // 过渡段
#define VOICEPARAGRAPH		2                                               // 语音段
#define OVERPARAGRAPH		3                                               // 语音结束

class PretreatmentControlCommon
{
private:
	static const unsigned long minSilence;                                  // 最短静音长度
	static const unsigned long minVoiceLength;                              // 最短语音长度

	MediaFile *mediaFile;
	double energyHigh;                                                      // 短时帧能量高门限
	double energyLow;                                                       // 短时帧能量低门限
	double zcrHigh;                                                         // 短时过零率高门限
	double zcrLow;                                                          // 短时过零率低门限

	vector<double> dataEnergy;                                              // 保存短时帧能量，个数为dataNunber-N
	vector<double> dataZCR;                                                 // 保存短时过零率

	double maxEnergy;                                                       // 用于保存最大的短时帧能量
	double minEnergy;                                                       // 用于保存最小的短时帧能量
	double maxZCR;                                                          // 用于保存最大的短时过零率
	double minZCR;                                                          // 用于保存最小的短时过零率

	unsigned long voiceNumber;                                              // 语音段落个数
	vector<VoiceParagraph> voiceParagraph;                                  // 保存这个语音文件中所有要处理的语音段落

	double Hamming_window(const double data);                               // 汉明窗函数
	short Sign_Function(const double data);                                 // 求短时过零率的辅助符号函数
	bool Frame_Energy(void);                                                // 用于求短时帧能量
	bool Frame_ZCR(void);                                                   // 用于求短时过零率
	bool Frame_EnergyZcr(void);                                             // 用于同时求取短时帧能量与短时过零率
public:
	static const int N;                                                     // 表示每个窗的帧长
	static const int FrameShift;                                            // 窗函数的帧移
	static const double PI;                                                 // 数学圆周率PI
	static const double preCoefficient;                                     // 预加重系数

	PretreatmentControlCommon(MediaFile *mediaFile);
	~PretreatmentControlCommon(void);

	vector<double> Get_DataEnergy(void);                                    // 获取短时帧能量的数据
	vector<double> Get_DataZCR(void);                                       // 获取短时过零率的数据
	double Get_maxEnergy(void);                                             // 获取最大短时帧能量
	double Get_minEnergy(void);                                             // 获取最小短时帧能量
	double Get_maxZCR(void);                                                // 获取最大短时过零率
	double Get_minZCR(void);                                                // 获取最小短时过零率
	double Get_dataEZNumber(void);                                          // 获取能量过零率的个数
	double Get_DataEnergy(const unsigned long Number);                      // 依据序号找到对应的短时帧能量
	double Get_DataZCR(const unsigned long Number);                         // 依据序号找到对应的短时过零率
	int Get_WindowLength(void);                                             // 获取帧长（窗的大小）
	unsigned long Get_voiceNumber(void);                                    // 获取语音段落个数
	unsigned long Get_frameNumber(void);                                    // 获取端点检测后每个段落的帧数和
	unsigned long Get_frameNumber(const double dataSize);                   // 计算长度内的帧数
	unsigned long Get_frameNumber(const VoiceParagraph voiceParagraph);     // 获取某个语音段落的帧数
	VoiceParagraph Get_dataVoicePoint(const unsigned long Number);          // 获取某个语音段落
	void Pre_emphasis(VoiceParagraph voiceParagraph, double *dataDouble);   // 对一个段落内的数据进行预加重处理 （注：预加重处理可以安放在分帧前，也可安放在分帧后）
	bool Frame_Data(double *data, unsigned long index, double* dataSpace, int dataSpaceSize);                       // 获取端点检测后第index帧的分帧加窗操作
	bool Frame_Data(double *data, double dataSize, unsigned long index, double* dataSpace, int dataSpaceSize);      // 对部分数据进行分帧加窗操作
	bool Endpoint_Detection(void);                                          // 端点检测函数
};
