#include "MediaFile.h"

unsigned short MediaFile::getChannelNumber(void) { return 0; }
unsigned int MediaFile::getSampleFrequency(void) { return 0; }
unsigned short MediaFile::getSampleBytes(void) { return 0; }
unsigned int MediaFile::getDataNumber(void) { return 0; }

double* MediaFile::getData(void) { return NULL; }
double MediaFile::getData(const unsigned int index) { return 0; }
void MediaFile::setData(const unsigned int index, int dataSample) {}

void MediaFile::writeWAV(FILE *fp) {}
void MediaFile::writeWAV(ofstream &fout) {}

void MediaFile::showData(void) {}