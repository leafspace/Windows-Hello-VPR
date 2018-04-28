#pragma once
#include <iostream>
#include <windows.h>
using namespace std;

class MediaFile
{
public:
	virtual unsigned short getChannelNumber();
	virtual unsigned int getSampleFrequency();
	virtual unsigned short getSampleBytes();
	virtual unsigned int getDataNumber();
	virtual int getData(const unsigned int index);

	virtual void setData(const unsigned int index, int dataSample);

	virtual void writeWAV(FILE *fp);
	virtual void writeWAV(ofstream &fout);
};