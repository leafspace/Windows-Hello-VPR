#include "WavData_CharaParameter.h"

bool CharaParameter::Push_data(unsigned long index, double *frame) {         //初始化特征参数类使用，将index帧的数据存放如类内
	if (index < 1 || index > frameNumber) {
		return false;
	}

	this->frameData[index - 1] = frame;
	return true;
}