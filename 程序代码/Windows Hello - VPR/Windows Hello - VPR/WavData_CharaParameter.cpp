#include "WavData_CharaParameter.h"

double**  CharaParameter::MFCC_CharaParameter()
{
	//拷贝数据，保存原先分帧后的数据
	this->frameFFTParameter = 
		this->DistributionSpace(this->frameNumber, WavFile_Initial::N);      //分配FFT保存的内存

    for (unsigned long i = 0; i < this->frameNumber; ++i) {
		memcpy(this->frameFFTParameter[i], this->frameData[i], 
			sizeof(double) * WavFile_Initial::N);                            //拷贝帧数据，预备后期计算FFT
    }

    //帧内求解快速傅里叶变换数据
    for (unsigned long i = 0; i < this->frameNumber; ++i) {
        double *space = 
			this->FFT(this->frameFFTParameter[i], WavFile_Initial::N);       //保存下返回的地址，因为这有可能是新数据的地址
		if (space != NULL) {
			this->frameFFTParameter[i] = space;
		}
    }

	//求频谱的平方，得到谱线的能量
	double** spectrumEnergy = 
		this->DistributionSpace(this->frameNumber, WavFile_Initial::N);      //为能量谱新建地址，但不需保存
	for (unsigned long i = 0; i < this->frameNumber; ++i) {
		for (int j = 0; j < WavFile_Initial::N; ++j) {
			spectrumEnergy[i][j] = pow(this->frameFFTParameter[i][j], 2);    //平方求谱线能量
		}
	}

	//将数据通过Mel滤波器组



	return spectrumEnergy;
}