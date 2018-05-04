#pragma once

#include <iostream>
using namespace std;

class MediaFile
{
public:
	virtual unsigned short getChannelNumber(void);
	virtual unsigned int getSampleFrequency(void);
	virtual unsigned short getSampleBytes(void);
	virtual unsigned int getDataNumber(void);

	virtual double* getData(void);
	virtual double getData(const unsigned int index);
	virtual void setData(const unsigned int index, int dataSample);

	virtual void writeWAV(FILE *fp);
	virtual void writeWAV(ofstream &fout);

	virtual void showData(void);
};