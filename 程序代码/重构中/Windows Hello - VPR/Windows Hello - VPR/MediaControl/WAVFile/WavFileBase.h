#pragma once
#include <vector>
#include "../Common/MediaFile.h"
#include "../Common/CommonType.h"

typedef struct WaveChunk                                                              // RIFF块结构
{
	char RIFF[4];
	unsigned int fileLength;                                                          // 文件长度
	char WAVE[4];
	WaveChunk();
} WaveChunk;

typedef struct FormatChunk                                                            // 格式块结构
{
	char FMT[4];
	unsigned int fChunkLength;                                                        // 格式块长度
	unsigned short formatCategory;                                                    // 格式类别
	unsigned short channelNumber;                                                     // 声道数
	unsigned int sampleFrequency;                                                     // 采样频率
	unsigned int transferRate;                                                        // 数据传送速率
	unsigned short sampleBytes;                                                       // 样本字节数
	unsigned short sampleBits;                                                        // 样本位数

	unsigned short extraInfo;                                                         // 附加信息
	FormatChunk();
} FormatChunk;

typedef struct FactChunk                                                              // 附加块结构
{
	char FACT[4];
	unsigned int eChunkLength;                                                        // 块长度
	unsigned int eChunk;
	FactChunk();
} FactChunk;

typedef struct DataChunk                                                              // 数据块结构
{
	char DATA[4];
	unsigned int dataLength;                                                          // 数据长度
	DataType *dataList;                                                               // 数据列表
	DataChunk();
} DataChunk;

class WavFile : public MediaFile
{
private:
	bool isWAV;
	WaveChunk waveChunk;                                                              // RIFF块
	FormatChunk formatChunk;                                                          // 格式块
	FactChunk *factChunk;                                                             // 附加块
	DataChunk dataChunk;                                                              // 数据块

	void readWAV(FILE *fp);
	void readWAV(ifstream &fin);
public:
	WavFile(void);
	WavFile(FILE *fp);
	WavFile(ifstream &fin);
	~WavFile(void);

	bool isWAVE(void);
	virtual unsigned short getChannelNumber(void);                                    // 获取声道数
	virtual unsigned int getSampleFrequency(void);                                    // 获取采样频率
	virtual unsigned short getSampleBytes(void);                                      // 获取采样位数
	virtual unsigned int getDataNumber(void);                                         // 获取数据数量

	virtual DataType* getData(void);
	virtual int getData(const unsigned int index);

	virtual void setData(const unsigned int index, int dataSample);

	virtual void writeWAV(FILE *fp);
	virtual void writeWAV(ofstream &fout);
};
