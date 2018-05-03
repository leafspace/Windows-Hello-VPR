#pragma once
#include <fstream>
#include "CommonType.h"

class MediaFile
{
public:
	virtual unsigned short getChannelNumber(void);
	virtual unsigned int getSampleFrequency(void);
	virtual unsigned short getSampleBytes(void);
	virtual unsigned int getDataNumber(void);

	virtual DataType* getData(void);
	virtual int getData(const unsigned int index);
	virtual void setData(const unsigned int index, int dataSample);

	virtual void writeWAV(FILE *fp);
	virtual void writeWAV(ofstream &fout);
};