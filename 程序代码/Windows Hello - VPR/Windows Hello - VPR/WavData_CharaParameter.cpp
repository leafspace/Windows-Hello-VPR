#include "WavData_CharaParameter.h"

double**  WavData_CharaParameter::MFCC_CharamParameter()
{
    //拷贝数据，保存原先分帧后的数据
    double** frameFFTParameter = new double[this->frameNumber][WavFile_Initial::N];

    for (unsigned long i = 0; i < this->frameNumber; ++i) {
        for (unsigned long j = 0; j < WavFile_Initial::N; ++j) {
            frameFFTParameter[i][j] = this->frameData[i][j];
        }
    }

    //帧内求解快速傅里叶变换数据
    for (unsigned long i = 0; i < this->frameNumber; ++i) {
        double *space = this->FFT(frameFFTParameter[i], WavFile_Initial::N); //保存下返回的地址，因为这有可能是新数据的地址
		if (space != NULL) {
			frameFFTParameter[i] = space;
		}
    }

    this->frameFFTParameter = frameFFTParameter;

}