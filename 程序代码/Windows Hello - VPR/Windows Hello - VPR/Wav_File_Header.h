#pragma once
#include <vector>
#include <iostream>
#include <windows.h>
using namespace std;

struct VoiceParagraph
{
	unsigned long begin;                                                     //语音段落开始点
	unsigned long end;                                                       //语音段落结束点
	unsigned long voiceLength;                                               //语音段落长度
	VoiceParagraph(unsigned long be, unsigned long en, unsigned long vo) {   //构造初始化
		begin = be;
		end = en;
		voiceLength = vo;
	}
};

class Wav_File
{
private:
	FILE *fp;                                                                //文件指针
	char *fileName;                                                          //文件名或地址

	char id_RIFF[5];                                                         //RIFF块标志
	unsigned long fileSize;                                                  //文件的总字节数
	char id_WAVE[5];                                                         //WAV标志

	char id_FMT[5];                                                          //格式块标志
	unsigned long formatLength;                                              //格式块长度，16为正常，18说明有附加信息
	short formatTag;                                                         //格式类别，值=1 表示编码方式为PCMu律编码
	short channelsNumber;                                                    //声道数
	unsigned long sampleRate;                                                //每秒的样本数
	unsigned long secondBytes;                                               //每秒数据的字节数
	short chunkAlign;                                                        //采样字节数
	short sampleBits;                                                        //采样位数
	short appendInformation;                                                 //附加信息，通过formatLength来判断

	char id_FACT[5];                                                         //附加块标志
	unsigned long appendLength;                                              //附加块长度
	unsigned long appendNone;                                                //未知

	char id_DATA[5];                                                         //附加块标志
	unsigned long dataSize;                                                  //数据部分字节数
	char *data;                                                              //数据部分

	long *dataTuple;                                                         //每个样本的数据
	unsigned long dataNumber;                                                //样本的数据个数
	long dataMax;                                                            //样本数据的最大值
	long dataMin;                                                            //样本数据的最小值

	bool Conversion_Tuple(void);                                             //将直接读取的数据转换为样本数据
	long MakeWord(long NumberA, long NumberB);                               //合并字节
public:
	Wav_File(void) {
		dataMax = 0;
		dataMin = 1000000;
	}
	Wav_File(FILE *f) {
		fp = f;
		::Wav_File();
	}
	void Give_FP(FILE *f) {
		fp = f;
	}
	bool Read_File(void);                                                    //读取文件
	unsigned long Get_SampleRate(void);                                      //获取采样频率
	short Get_ChunkAlign(void);                                              //获取样本字节数
	long Get_Data(unsigned long Number);                                     //获取某个位置上的数据
	unsigned long Get_dataNumber(void);                                      //获取样本数据个数
	void ShowData(void);                                                     //输出数据
	void SaveNewWav(unsigned long voiceNumber, vector<VoiceParagraph> voiceParagraph);           //保存去掉空白处的语音文件，参数1为段落的个数，参数2为各个语音段落的开始点与结束点信息
};