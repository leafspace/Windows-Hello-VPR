﻿#include "../PretreatmentControl/PretreatmentControlCommon.h"

class CharaParameterControl
{
private:
	unsigned long frameNumber;                                                        // 帧数量
	double **frameData;                                                               // 帧数据
	double **frameFFTParameter;                                                       // 帧数据通过快速傅里叶变换后的数据
	double **frameMelParameter;                                                       // FFT数据通过Mel频率滤波器组后的数据

	// 底层内存操作
	double *DistributionSpace(const unsigned long col);                               // 分配一维数组空间
	double **DistributionSpace(const unsigned long row, const unsigned long col);     // 分配二维数组空间
	void DestorySpace(double* space);                                                 // 销毁一维数组空间
	void DestorySpace(double** space, const unsigned long row);                       // 销毁二位数组空间

	// 傅里叶变换操作
	unsigned long fftNumber;                                                          // 傅里叶变换后的数据的个数
	double* FFT(double * const data, const unsigned long dataNumber);                 // 快速离散傅立叶变换，无虚部 (严重警告，因为可能出现扩展内存的情况，所以必须返回新地址)
	bool IFFT(double * const data, const unsigned long dataNumber);                   // 快速离散逆傅立叶变换，无虚部
	bool DCT(double ** const dataEngrgy, double ** const dataRet, unsigned long row, const unsigned long col, const int degree);        //离散余弦变换
public:
	static const int MelCoefficient;                                                  // 计算mel频率的系数，简称mel系数
	static const int MelFilterNumber;                                                 // mel频率滤波器组的滤波器个数
	static const int MelDegreeNumber;                                                 // mel频率的阶数

	CharaParameterControl(unsigned long frameNumber) {
		this->frameNumber = frameNumber;
		this->frameData = new double*[this->frameNumber];                             // 分配指针数组的空间
		this->frameFFTParameter = NULL;
		this->frameMelParameter = NULL;
	}
	~CharaParameterControl(void) {
		this->DestorySpace(this->frameMelParameter, this->frameNumber);
		this->DestorySpace(this->frameFFTParameter, this->frameNumber);
		this->DestorySpace(this->frameData, this->frameNumber);
	}

	bool Push_data(const unsigned long index, double * const frame);                  // 初始化特征参数类使用，将index帧的数据存放如类内
	unsigned long Get_frameNumber();                                                  // 获取帧数量
	double* Get_frameMelParameter(const unsigned long row);                           // 获取Mel特征参数中的第Row行
	double Get_frameMelParameter(const unsigned long row, const unsigned long col);   // 获取Mel特征参数中的[row][col]

	// 特征参数求解列表
	double** MFCC_CharaParameter(unsigned long sampleRate);                           // 求解MFCC特诊参数
};