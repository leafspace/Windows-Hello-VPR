#include "../stdafx.h"
#include "WAVStruct.h"

WaveChunk::WaveChunk()
{
	memset(this->RIFF, 0, 4);
	this->fileLength = 0;
	memset(this->WAVE, 0, 4);
}

FormatChunk::FormatChunk()
{
	memset(this->FMT, 0, 4);
	this->fChunkLength = 0;
	this->formatCategory = 0;
	this->channelNumber = 0;
	this->sampleFrequency = 0;
	this->transferRate = 0;
	this->sampleBytes = 0;
	this->sampleBits = 0;

	this->extraInfo = 0;
}

FactChunk::FactChunk()
{
	memset(this->FACT, 0, 4);
	this->eChunkLength = 0;
	this->eChunk = 0;
}

DataChunk::DataChunk()
{
	memset(this->DATA, 0, 4);
	this->dataLength = 0;
	this->dataList = NULL;
}

WAV::WAV()
{
	this->isWAV = true;
	this->factChunk = NULL;
}

WAV::WAV(FILE *fp)
{
	WAV();
	this->readWAV(fp);
}

WAV::WAV(ifstream &fin)
{
	WAV();
	this->readWAV(fin);

}

WAV::~WAV()
{
	if (factChunk != NULL) {
		delete factChunk;
		factChunk = NULL;
	}
}

void WAV::readWAV(FILE *fp)
{
	fread(this->waveChunk.RIFF, sizeof(char), 4, fp);                       // 读取'RIFF'
	fread(&this->waveChunk.fileLength, sizeof(unsigned int), 1, fp);        // 读取文件的大小
	fread(this->waveChunk.WAVE, sizeof(char), 4, fp);                       // 读取'WAVE'

	fread(this->formatChunk.FMT, sizeof(char), 4, fp);                      // 读取'FMT'
	fread(&this->formatChunk.fChunkLength, sizeof(unsigned int), 1, fp);    // 读取块大小
	fread(&this->formatChunk.formatCategory, sizeof(unsigned short), 1, fp);// 读取文件标签
	fread(&this->formatChunk.channelNumber, sizeof(unsigned short), 1, fp); // 读取声道数
	fread(&this->formatChunk.sampleFrequency, sizeof(unsigned int), 1, fp); // 读取采样频率
	fread(&this->formatChunk.transferRate, sizeof(unsigned int), 1, fp);    // 读取数据传送速率
	fread(&this->formatChunk.sampleBytes, sizeof(unsigned short), 1, fp);   // 读取样本字节数
	fread(&this->formatChunk.sampleBits, sizeof(unsigned short), 1, fp);    // 读取样本位数
	if (this->formatChunk.fChunkLength > 16) {                              // 含有附加块的情况
		fread(&this->formatChunk.extraInfo, sizeof(unsigned short), 1, fp); // 读取附加信息
		this->factChunk = new FactChunk();
		fread(this->factChunk->FACT, sizeof(char), 4, fp);                  // 读取'FACT'
		fread(&this->factChunk->eChunkLength, sizeof(unsigned int), 1, fp); // 读取块大小
		fread(&this->factChunk->eChunk, sizeof(unsigned int), 1, fp);       // 读取附加块
	}

	DataType::setSizeFlag(this->getSampleBytes());

	fread(this->dataChunk.DATA, sizeof(char), 4, fp);                       // 读取'DATA'
	fread(&this->dataChunk.dataLength, sizeof(unsigned int), 1, fp);        // 读取数据大小
	double dataNumber = this->dataChunk.dataLength / this->getSampleBytes();
	if (dataNumber != this->getDataNumber()
		|| this->dataChunk.dataLength > this->waveChunk.fileLength) {       // 数值跟设置不吻合
		this->isWAV = false;
		return;
	}
	this->dataChunk.dataList = new DataType[(unsigned int)dataNumber];
	unsigned int tempNumber = 0;
	for (unsigned int i = 0; i < dataNumber; ++i) {
		fread(&tempNumber, this->getSampleBytes(), 1, fp);                  //读取数据
		this->dataChunk.dataList[i].setValue(tempNumber);
	}
}

void WAV::readWAV(ifstream &fin)
{
	fin.read(this->waveChunk.RIFF, sizeof(char) * 4);
	fin.read(this->waveChunk.RIFF, sizeof(char) * 4);                       // 读取'RIFF'
	fin.read(reinterpret_cast<char*>(this->waveChunk.fileLength), sizeof(unsigned int) * 1);              // 读取文件的大小
	fin.read(this->waveChunk.WAVE, sizeof(char) * 4);                       // 读取'WAVE'

	fin.read(this->formatChunk.FMT, sizeof(char) * 4);                      // 读取'FMT'
	fin.read(reinterpret_cast<char*>(this->formatChunk.fChunkLength), sizeof(unsigned int) * 1);          // 读取块大小
	fin.read(reinterpret_cast<char*>(this->formatChunk.formatCategory), sizeof(unsigned short) * 1);      // 读取文件标签
	fin.read(reinterpret_cast<char*>(this->formatChunk.channelNumber), sizeof(unsigned short) * 1);       // 读取声道数
	fin.read(reinterpret_cast<char*>(this->formatChunk.sampleFrequency), sizeof(unsigned int) * 1);       // 读取采样频率
	fin.read(reinterpret_cast<char*>(this->formatChunk.transferRate), sizeof(unsigned int) * 1);          // 读取数据传送速率
	fin.read(reinterpret_cast<char*>(this->formatChunk.sampleBytes), sizeof(unsigned short) * 1);         // 读取样本字节数
	fin.read(reinterpret_cast<char*>(this->formatChunk.sampleBits), sizeof(unsigned short) * 1);          // 读取样本位数
	if (this->formatChunk.fChunkLength > 16) {                              // 含有附加块的情况
		fin.read(reinterpret_cast<char*>(this->formatChunk.extraInfo), sizeof(unsigned short) * 1);       // 读取附加信息
		this->factChunk = new FactChunk();
		fin.read(this->factChunk->FACT, sizeof(char) * 4);                  // 读取'FACT'
		fin.read(reinterpret_cast<char*>(this->factChunk->eChunkLength), sizeof(unsigned int) * 1);       // 读取块大小
		fin.read(reinterpret_cast<char*>(this->factChunk->eChunk), sizeof(unsigned int) * 1);             // 读取附加块
	}

	DataType::setSizeFlag(this->getSampleBytes());

	fin.read(this->dataChunk.DATA, sizeof(char) * 4);                       // 读取'DATA'
	fin.read(reinterpret_cast<char*>(this->dataChunk.dataLength), sizeof(unsigned int) * 1);              // 读取数据大小
	double dataNumber = this->dataChunk.dataLength / this->getSampleBytes();
	if (dataNumber != this->getDataNumber()
		|| this->dataChunk.dataLength > this->waveChunk.fileLength) {       // 数值跟设置不吻合
		this->isWAV = false;
		return;
	}
	this->dataChunk.dataList = new DataType[(unsigned int)dataNumber];
	unsigned int tempNumber = 0;
	for (unsigned int i = 0; i < dataNumber; ++i) {
		fin.read(reinterpret_cast<char*>(&tempNumber), this->getSampleBytes());
		this->dataChunk.dataList[i].setValue(tempNumber);
	}
}

bool WAV::isWAVE()
{
	do {
		if (!strstr(_strupr(this->waveChunk.RIFF), "RIFF")) {
			this->isWAV = false;
		}

		if (!strstr(_strupr(this->waveChunk.WAVE), "WAVE")) {
			this->isWAV = false;
		}

		if (!strstr(_strupr(this->formatChunk.FMT), "FMT")) {
			this->isWAV = false;
		}

		if (this->factChunk) {
			if (!strstr(_strupr(this->factChunk->FACT), "FACT")) {
				this->isWAV = false;
			}
		}

		if (!strstr(_strupr(this->dataChunk.DATA), "DATA")) {
			this->isWAV = false;
		}
	} while (0);
	return this->isWAV;
}

unsigned short WAV::getChannelNumber()
{
	return this->formatChunk.channelNumber;
}

unsigned int WAV::getSampleFrequency()
{
	return this->formatChunk.sampleFrequency;
}

unsigned short WAV::getSampleBytes()
{
	return this->formatChunk.sampleBytes / this->formatChunk.channelNumber;
}

unsigned int WAV::getDataNumber()
{
	return (unsigned int)(this->dataChunk.dataLength / this->getSampleBytes());
}

int WAV::getData(const unsigned int index)
{
	if (index >= this->getDataNumber()) {
		return 0;
	}
	return this->dataChunk.dataList[index];
}

void WAV::setData(const unsigned int index, int dataSample)
{
	if (index <= this->getDataNumber()) {
		this->dataChunk.dataList[index].setValue(dataSample);
	}
}

void WAV::writeWAV(FILE *fp)
{
	fwrite(this->waveChunk.RIFF, sizeof(char), 4, fp);                      // 写入'RIFF'
	fwrite(&this->waveChunk.fileLength, sizeof(unsigned int), 1, fp);       // 写入文件的大小
	fwrite(this->waveChunk.WAVE, sizeof(char), 4, fp);                      // 写入'WAVE'

	fwrite(this->formatChunk.FMT, sizeof(char), 4, fp);                     // 写入'FMT'
	fwrite(&this->formatChunk.fChunkLength, sizeof(unsigned int), 1, fp);   // 写入块大小
	fwrite(&this->formatChunk.formatCategory, sizeof(unsigned short), 1, fp);                             // 写入文件标签
	fwrite(&this->formatChunk.channelNumber, sizeof(unsigned short), 1, fp);// 写入声道数
	fwrite(&this->formatChunk.sampleFrequency, sizeof(unsigned int), 1, fp);// 写入采样频率
	fwrite(&this->formatChunk.transferRate, sizeof(unsigned int), 1, fp);   // 写入数据传送速率
	fwrite(&this->formatChunk.sampleBytes, sizeof(unsigned short), 1, fp);  // 写入样本字节数
	fwrite(&this->formatChunk.sampleBits, sizeof(unsigned short), 1, fp);   // 写入样本位数
	if (this->formatChunk.fChunkLength > 16) {                              // 含有附加块的情况
		fwrite(&this->formatChunk.extraInfo, sizeof(unsigned short), 1, fp);// 写入附加信息
		fwrite(this->factChunk->FACT, sizeof(char), 4, fp);                 // 写入'FACT'
		fwrite(&this->factChunk->eChunkLength, sizeof(unsigned int), 1, fp);// 写入块大小
		fwrite(&this->factChunk->eChunk, sizeof(unsigned int), 1, fp);      // 写入附加块
	}

	fwrite(this->dataChunk.DATA, sizeof(char), 4, fp);                      // 写入'DATA'
	fwrite(&this->dataChunk.dataLength, sizeof(unsigned int), 1, fp);       // 写入数据大小
	for (unsigned int i = 0, tempNumber = 0; i < this->getDataNumber(); ++i) {
		tempNumber = this->dataChunk.dataList[i];
		fwrite(&tempNumber, this->getSampleBytes(), 1, fp);// 写入数据
	}
}

void WAV::writeWAV(ofstream &fout)
{
	fout.write(this->waveChunk.RIFF, sizeof(char) * 4);
	fout.write(this->waveChunk.RIFF, sizeof(char) * 4);                     // 写入'RIFF'
	fout.write(reinterpret_cast<char*>(this->waveChunk.fileLength), sizeof(unsigned int) * 1);              // 写入文件的大小
	fout.write(this->waveChunk.WAVE, sizeof(char) * 4);                     // 写入'WAVE'

	fout.write(this->formatChunk.FMT, sizeof(char) * 4);                    // 写入'FMT'
	fout.write(reinterpret_cast<char*>(this->formatChunk.fChunkLength), sizeof(unsigned int) * 1);          // 写入块大小
	fout.write(reinterpret_cast<char*>(this->formatChunk.formatCategory), sizeof(unsigned short) * 1);      // 写入文件标签
	fout.write(reinterpret_cast<char*>(this->formatChunk.channelNumber), sizeof(unsigned short) * 1);       // 写入声道数
	fout.write(reinterpret_cast<char*>(this->formatChunk.sampleFrequency), sizeof(unsigned int) * 1);       // 写入采样频率
	fout.write(reinterpret_cast<char*>(this->formatChunk.transferRate), sizeof(unsigned int) * 1);          // 写入数据传送速率
	fout.write(reinterpret_cast<char*>(this->formatChunk.sampleBytes), sizeof(unsigned short) * 1);         // 写入样本字节数
	fout.write(reinterpret_cast<char*>(this->formatChunk.sampleBits), sizeof(unsigned short) * 1);          // 写入样本位数
	if (this->formatChunk.fChunkLength > 16) {                              // 含有附加块的情况
		fout.write(reinterpret_cast<char*>(this->formatChunk.extraInfo), sizeof(unsigned short) * 1);       // 写入附加信息
		fout.write(this->factChunk->FACT, sizeof(char) * 4);                // 写入'FACT'
		fout.write(reinterpret_cast<char*>(this->factChunk->eChunkLength), sizeof(unsigned int) * 1);       // 写入块大小
		fout.write(reinterpret_cast<char*>(this->factChunk->eChunk), sizeof(unsigned int) * 1);             // 写入附加块
	}

	fout.write(this->dataChunk.DATA, sizeof(char) * 4);                     // 写入'DATA'
	fout.write(reinterpret_cast<char*>(this->dataChunk.dataLength), sizeof(unsigned int) * 1);              // 写入数据大小
	for (unsigned int i = 0, tempNumber = 0; i < this->getDataNumber(); ++i) {
		tempNumber = this->dataChunk.dataList[i];
		fout.write(reinterpret_cast<char*>(&tempNumber), this->getSampleBytes());
	}
}