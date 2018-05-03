#include "CharaParameterControl.h"

const int CharaParameterControl::MelCoefficient = 1125;                               // 计算mel频率的系数，简称mel系数 或是哟个2595
const int CharaParameterControl::MelFilterNumber = 24;                                // mel频率滤波器组的滤波器个数
const int CharaParameterControl::MelDegreeNumber = 13;                                // mel频率的阶数

double**  CharaParameterControl::MFCC_CharaParameter(unsigned long sampleRate)        // 求解MFCC特诊参数
{
	//1.拷贝数据，保存原先分帧后的数据
	this->frameFFTParameter =
		this->DistributionSpace(this->frameNumber, PretreatmentControlCommon::N);     // 分配FFT保存的内存

	for (unsigned long i = 0; i < this->frameNumber; ++i) {
		memcpy(this->frameFFTParameter[i], this->frameData[i],
			sizeof(double)* PretreatmentControlCommon::N);                            // 拷贝帧数据，预备后期计算FFT
	}

	//2.帧内求解快速傅里叶变换数据
	for (unsigned long i = 0; i < this->frameNumber; ++i) {
		double *space =
			this->FFT(this->frameFFTParameter[i], PretreatmentControlCommon::N);      // 保存下返回的地址，因为这有可能是新数据的地址
		if (space != NULL) {
			this->frameFFTParameter[i] = space;
		}
	}

	//3.求频谱的平方，得到谱线的能量
	double** spectrumEnergy =
		this->DistributionSpace(this->frameNumber, PretreatmentControlCommon::N);     // 为能量谱新建地址，但不需保存
	for (unsigned long i = 0; i < this->frameNumber; ++i) {
		for (int j = 0; j < PretreatmentControlCommon::N; ++j) {
			spectrumEnergy[i][j] = pow(this->frameFFTParameter[i][j], 2);             // 平方求谱线能量
		}
	}

	//4.将数据通过Mel滤波器组
	//  1)求最大/最小mel频率，及滤波器的中心间距
	double melFreMax = CharaParameterControl::MelCoefficient *
		log((long double)1 + (sampleRate / 2) / 700);                                 // 求最大mel频率
	double melFreMin = CharaParameterControl::MelCoefficient *
		log((long double)1);                                                          // 求最小mel频率
	double centerSpace = (melFreMax - melFreMin) /
		(CharaParameterControl::MelFilterNumber + 1);                                 // 求姐mel滤波器组中每个滤波器的中心距离

	//  2)求mel滤波器中每个滤波器的实际频率值
	double * melFrequency =
		this->DistributionSpace(CharaParameterControl::MelFilterNumber + 2);          // 为mel滤波器组的实际频率创建内存空间,但不需要保存
	for (int i = 0; i < CharaParameterControl::MelFilterNumber + 2; ++i) {
		melFrequency[i] =
			floor(((PretreatmentControlCommon::N + 1) * 700 *
			(exp((melFreMin + i * centerSpace) / CharaParameterControl::MelCoefficient) - 1))
			/ sampleRate);                                                            // 求解每个滤波器的实际频率
	}

	//  3)将线谱能量通过mel滤波器组
	double** melEnergy =
		this->DistributionSpace(this->frameNumber, CharaParameterControl::MelFilterNumber);                         //分配保存Mel特诊参数的内存
	double tempCoefficient = 0;
	for (unsigned long i = 0; i < this->frameNumber; ++i) {
		for (int j = 1; j <= CharaParameterControl::MelFilterNumber; ++j) {
			tempCoefficient = 0;
			for (int k = 0; k < PretreatmentControlCommon::N; ++k) {                  // 将数据频率与实际频率对应起来
				if (k >= melFrequency[j - 1] && k <= melFrequency[j]) {
					tempCoefficient = (k - melFrequency[j - 1]) * (melFrequency[j] - melFrequency[j - 1]);
				}
				else if (k >= melFrequency[j] && k <= melFrequency[j + 1]) {
					tempCoefficient = (melFrequency[j + 1] - k) * (melFrequency[j + 1] - melFrequency[j]);
				}
				melEnergy[i][j - 1] += this->frameFFTParameter[i][k] * tempCoefficient;
			}
		}
	}

	//5.求对数
	for (unsigned long i = 0; i < this->frameNumber; ++i) {
		for (int j = 0; j < CharaParameterControl::MelFilterNumber; ++j) {
			if (melEnergy[i][j] < 0) {                                                // 处理数据为负数的情况，以免出现无效数据
				melEnergy[i][j] = log(fabs(melEnergy[i][j])) * -1;
			}
			else {
				melEnergy[i][j] = log(melEnergy[i][j]);
			}
		}
	}

	//6.进行离散余弦变换
	this->frameMelParameter = this->DistributionSpace(this->frameNumber, CharaParameterControl::MelDegreeNumber);
	this->DCT(melEnergy, this->frameMelParameter, this->frameNumber,
		CharaParameterControl::MelFilterNumber, CharaParameterControl::MelDegreeNumber);

	//销毁内存
	this->DestorySpace(melEnergy, this->frameNumber);
	this->DestorySpace(melFrequency);
	this->DestorySpace(spectrumEnergy, this->frameNumber);

	return this->frameMelParameter;
}