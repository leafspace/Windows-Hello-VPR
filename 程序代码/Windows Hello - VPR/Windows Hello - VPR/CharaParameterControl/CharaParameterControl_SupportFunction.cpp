#include "CharaParameterControl.h"

double* CharaParameterControl::DistributionSpace(const unsigned long col)             // 分配一维数组空间
{
	double *tempspace = new double[col];
	if (tempspace == NULL) {
		return NULL;
	}
	else {
		for (unsigned long i = 0; i < col; ++i) {
			tempspace[i] = 0;
		}
	}
	return tempspace;
}

double** CharaParameterControl::DistributionSpace(const unsigned long row, const unsigned long col)                 // 分配二维数组空间
{
	double **tempspace = new double*[row];
	if (tempspace == NULL) {
		return NULL;
	}
	for (unsigned long i = 0; i < row; ++i) {
		tempspace[i] = this->DistributionSpace(col);
		if (tempspace[i] == NULL) {
			this->DestorySpace(*tempspace);
			return NULL;
		}
	}
	return tempspace;
}

void CharaParameterControl::DestorySpace(double *space)                               // 销毁一维数组空间
{
	delete space;
	space = NULL;
}

void CharaParameterControl::DestorySpace(double **space, const unsigned long row)     // 销毁二位数组空间
{
	for (unsigned long i = 0; i < row; ++i) {
		this->DestorySpace(space[i]);
	}
	delete space;
	space = NULL;
}

double* CharaParameterControl::FFT(double * const data, const unsigned long dataNumber)                             // 快速离散傅立叶变换，无虚部
{
	short power = 0;
	for (int i = dataNumber - 1; i > 0; ++power, i /= 2);

	this->fftNumber = (unsigned long)pow((float)2, (int)power);
	double *dataFFT = DistributionSpace(this->fftNumber);
	for (unsigned long i = 0; i < dataNumber; i++) {
		dataFFT[i] = data[i];                                                         // 初始化快速傅立叶变换数据
	}
	double *rotationFactor = DistributionSpace(this->fftNumber / 2);                  // 计算旋转因子
	for (unsigned long i = 0; i < this->fftNumber / 2; i++) {
		rotationFactor[i] = cos(2 * i * PretreatmentControlCommon::PI / this->fftNumber);
	}
	unsigned long ulGroupLength = 1;										          // 段的长度
	unsigned long ulHalfLength = 0;											          // 段长度的一半
	unsigned long ulGroupCount = 0;					                                  // 段的数量
	double cw, c1, c2;
	for (short b = 0; b < power; b++)                                                 // 计算FFT
	{
		ulHalfLength = ulGroupLength;
		ulGroupLength *= 2;
		for (unsigned long j = 0; j < this->fftNumber; j += ulGroupLength)
		{
			for (unsigned long k = 0; k < ulHalfLength; k++)
			{
				cw = rotationFactor[k * this->fftNumber / ulGroupLength], dataFFT[j + k + ulHalfLength];
				c1 = dataFFT[j + k] + cw;
				c2 = dataFFT[j + k] - cw;
				dataFFT[j + k] = c1;
				dataFFT[j + k + ulHalfLength] = c2;
			}
		}
	}
	free(rotationFactor);
	if (this->fftNumber == dataNumber) {                                              // 如何FFT运算时个数正好，没有扩展，则需要拷贝数据
		for (unsigned long i = 0; i < dataNumber; ++i) {
			data[i] = dataFFT[i];
		}
		free(dataFFT);
		return NULL;
	}
	else {                                                                            // 否则需要返回新的数据地址
		free(data);
		return dataFFT;
	}
}

bool CharaParameterControl::IFFT(double * const data, const unsigned long dataNumber) // 快速离散逆傅立叶变换，无虚部
{
	short power = 0;
	double *dataIFFT;
	for (unsigned long i = dataNumber - 1; i > 0; ++power, i /= 2);                   // 计算次幂
	for (unsigned long i = 1, j = dataNumber / 2; i < (unsigned long)PretreatmentControlCommon::N - 1; ++i) {       //雷德算法重排位置
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
	for (unsigned long i = 0; i < dataNumber; ++i) {                                  // 逆操作
		dataIFFT[i] = data[i] / dataNumber;
	}
	for (short p = 0; p < power; ++p) {                                               // IFFT计算
		for (unsigned long i = 0; i < pow((long double)2, (long double)p); ++i) {
			double cw = i*pow((long double)2, (long double)power - (p + 1));          // 计算旋转因子
			for (unsigned long j = i; j < dataNumber - 1; j += (unsigned long)pow((long double)2, (long double)(p + 1))) {
				unsigned long index = j + (unsigned long)pow((long double)2, (long double)p);
				cw = dataIFFT[index] * cos(2 * PretreatmentControlCommon::PI * cw / dataNumber);
				dataIFFT[index] = dataIFFT[j] - cw;
				dataIFFT[j] = dataIFFT[j] + cw;
			}
		}
	}
	for (unsigned long i = 0; i < dataNumber; ++i) {                                  // 拷贝数据到自身
		data[i] = dataIFFT[i];
	}
	free(dataIFFT);
	return true;
}

bool CharaParameterControl::DCT(double ** const dataEngrgy, double ** const dataRet, const unsigned long row, const unsigned long col, int degree)          //离散余弦变换
{
	for (unsigned long i = 0; i < row; ++i) {
		for (int j = 0; j < degree; ++j) {
			for (unsigned long k = 0; k < col; ++k) {
				dataRet[i][j] += dataEngrgy[i][k] * cos(PretreatmentControlCommon::PI * j * (0.5 + k) / (col));     // 计算公式
			}
		}
	}
	return true;
}

bool CharaParameterControl::Push_data(const unsigned long index, double * const frame)// 初始化特征参数类使用，将index帧的数据存放如类内
{
	if (index < 1 || index > this->frameNumber) {
		return false;
	}

	this->frameData[index - 1] = frame;
	return true;
}


unsigned long CharaParameterControl::Get_frameNumber()                                // 获取帧数量
{
	return this->frameNumber;
}

double* CharaParameterControl::Get_frameMelParameter(const unsigned long row)         // 获取Mel特征参数中的第Row行
{
	if (row < 0 || row >= this->frameNumber) {
		return NULL;
	}

	return this->frameMelParameter[row];
}

double CharaParameterControl::Get_frameMelParameter(const unsigned long row, const unsigned long col)               // 获取Mel特征参数中的[row][col]
{
	if (row < 0 || row >= this->frameNumber ||
		col < 0 || col >= (unsigned long)CharaParameterControl::MelDegreeNumber) {
		return NULL;
	}

	return this->frameMelParameter[row][col];
}
