#include "WavFile_Initial.h"

class CharaParameter
{
private:
	unsigned long frameNumber;                                               //帧数量
	double **frameData;                                                      //帧数据

public:
	CharaParameter(unsigned long frameNumber) {
		this->frameNumber = frameNumber;
		this->frameData = new double* [this->frameNumber];                   //分配指针数组的空间
	}
	
	bool Push_data(unsigned long index, double *frame);                      //初始化特征参数类使用，将index帧的数据存放如类内
};