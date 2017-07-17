#include "WavData_CharaParameter.h"

double* CharaParameter::DistributionSpace(unsigned long col)                 //分配一维数组空间
{
	double *tempspace = new double[col];
	if (tempspace == NULL) {
		throw invalid_argument("ERROR : Memory failure !");
		return false;
	} else {
		for (unsigned long i = 0; i < col; ++i) {
			tempspace[i] = 0;
		}
	}
	return tempspace;
}

double** CharaParameter::DistributionSpace(unsigned long row, unsigned long col)       //分配二维数组空间
{
	double **tempspace = new double* [row];
	if (tempspace == NULL) {
		throw invalid_argument("ERROR : Memory failure !");
		return false;
	}
	for (unsigned long i = 0; i < row; ++i) {
		tempspace[i] = this->DistributionSpace(col);
		if (tempspace[i] == NULL) {
			throw invalid_argument("ERROR : Memory failure !");
			return false;
		}
	}
	return tempspace;
}

void CharaParameter::DestorySpace(double *space)                             //销毁一维数组空间
{
	delete space;
}

void CharaParameter::DestorySpace(double **space, unsigned long row)         //销毁二位数组空间
{
	for (unsigned long i = 0; i < row; ++i) {
		delete space[i];
	}
	delete space;
}

void CharaParameter::ShowDataValue(bool showOnTerminal)                      //显示求值过程中的数据并保存在文件中
{
	/*
	cout << "TIP : This is orgin data :" << endl;
	for (unsigned long i = 0; i < this->frameNumber; ++i) {
		for (int j = 0; j < WavFile_Initial::N; ++j) {
			cout << this->frameData[i][j] << "\t";
		}
		cout << endl;
	}
	cout << endl;

	cout << "TIP : This is FFT past data :" << endl;
	for (unsigned long i = 0; i < this->frameNumber; ++i) {
		for (int j = 0; j < WavFile_Initial::N; ++j) {
			cout << this->frameFFTParameter[i][j] << "\t";
		}
		cout << endl;
	}
	cout << endl;
	*/

	cout << "TIP : This is Mel data :" << endl;
	for (unsigned long i = 0; i < this->frameNumber; ++i) {
		for (int j = 0; j < CharaParameter::MelDegreeNumber; ++j) {
			cout << this->frameMelParameter[i][j] << "\t";
		}
		cout << endl;
	}
}

double* CharaParameter::FFT(double *data, unsigned long dataNumber)          //快速离散傅立叶变换，无虚部
{
	short power = 0;
	for (int i = dataNumber - 1; i > 0; ++power, i /= 2);

	fftNumber = (unsigned long)pow((float)2, (int)power);
	double *dataFFT = DistributionSpace(fftNumber);
	for (unsigned long i = 0; i < dataNumber; i++) {
		dataFFT[i] = data[i];                                                //初始化快速傅立叶变换数据
	}
	double *W = DistributionSpace(fftNumber / 2);                            //计算旋转因子
	for (unsigned long i = 0; i < fftNumber / 2; i++) {
		W[i] = cos(2 * i * WavFile_Initial::PI / fftNumber);
	}
	unsigned long ulGroupLength = 1;										 //段的长度
	unsigned long ulHalfLength = 0;											 //段长度的一半
	unsigned long ulGroupCount = 0;											 //段的数量
	double cw, c1, c2;
	for (short b = 0; b < power; b++)                                        //计算FFT
	{
		ulHalfLength = ulGroupLength;
		ulGroupLength *= 2;
		for (unsigned long j = 0; j < fftNumber; j += ulGroupLength)
		{
			for (unsigned long k = 0; k < ulHalfLength; k++)
			{
				cw = W[k * fftNumber / ulGroupLength], dataFFT[j + k + ulHalfLength];
				c1 = dataFFT[j + k] + cw;
				c2 = dataFFT[j + k] - cw;
				dataFFT[j + k] = c1;
				dataFFT[j + k + ulHalfLength] = c2;
			}
		}
	}
	free(W);
	if (fftNumber == dataNumber) {                                           //如何FFT运算时个数正好，没有扩展，则需要拷贝数据
		for (unsigned long i = 0; i < dataNumber; ++i) {
			data[i] = dataFFT[i];
		}
		free(dataFFT);
		return NULL;
	}
	else {                                                                   //否则需要返回新的数据地址
		free(data);
		return dataFFT;
	}
}

bool CharaParameter::IFFT(double *data, unsigned long dataNumber)             //快速离散逆傅立叶变换，无虚部
{
	short power = 0;
	double *dataIFFT;
	for (unsigned long i = dataNumber - 1; i > 0; ++power, i /= 2);          //计算次幂
	for (unsigned long i = 1, j = dataNumber / 2; i < (unsigned long)WavFile_Initial::N - 1; ++i) {        //雷德算法重排位置
		if (i < j) {
			double temp = data[i];
			data[i] = data[j];
			data[j] = temp;
		}
		unsigned long k = dataNumber / 2;
		while (k <= j) {
			j = j - k;
			k = k / 2;
		}
		j = j + k;
	}
	dataIFFT = DistributionSpace(dataNumber);
	for (unsigned long i = 0; i < dataNumber; ++i) {                         //逆操作
		dataIFFT[i] = data[i] / dataNumber;
	}
	for (short p = 0; p < power; ++p) {                                      //IFFT计算
		for (unsigned long i = 0; i < pow((long double)2, (long double)p); ++i) {
			double cw = i*pow((long double)2, (long double)power - (p + 1)); //计算旋转因子
			for (unsigned long j = i; j < dataNumber - 1; j += (unsigned long)pow((long double)2, (long double)(p + 1))) {
				unsigned long index = j + (unsigned long)pow((long double)2, (long double)p);
				cw = dataIFFT[index] * cos(2 * WavFile_Initial::PI * cw / dataNumber);
				dataIFFT[index] = dataIFFT[j] - cw;
				dataIFFT[j] = dataIFFT[j] + cw;
			}
		}
	}
	for (unsigned long i = 0; i < dataNumber; ++i) {                         //拷贝数据到自身
		data[i] = dataIFFT[i];
	}
	free(dataIFFT);
	return true;
}

bool CharaParameter::DCT(double **dataEngrgy, double **dataRet, unsigned long row, unsigned long col, int degree)        //离散余弦变换
{
	for (unsigned long i = 0; i < row; ++i) {
		for (int j = 0; j < degree; ++j) {
			for (unsigned long k = 0; k < col; ++k) {
				dataRet[i][j] += dataEngrgy[i][k] * cos(WavFile_Initial::PI * j * (0.5 + k) / (col)); //计算公式
			}
		}
	}
	return true;
}

bool CharaParameter::Push_data(unsigned long index, double *frame) {         //初始化特征参数类使用，将index帧的数据存放如类内
	if (index < 1 || index > frameNumber) {
		return false;
	}

	this->frameData[index - 1] = frame;
	return true;
}
