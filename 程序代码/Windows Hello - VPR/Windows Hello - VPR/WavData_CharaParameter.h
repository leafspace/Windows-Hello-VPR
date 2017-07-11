#include "WavFile_Initial.h"

class CharaParameter
{
private:
	unsigned long frameNumber;                                               //帧数量
	double **frameData;                                                      //帧数据

	//底层内存操作
	double *DistributionSpace(unsigned long col);                            //分配一维数组空间
	double **DistributionSpace(unsigned long row, unsigned long col);        //分配二维数组空间
	void DestorySpace(double* space);                                        //销毁一维数组空间
	void DestorySpace(double** space, unsigned long row);                    //销毁二位数组空间

	//傅里叶变换操作
	unsigned long fftNumber;                                                 //傅里叶变换后的数据的个数
	double* FFT(double *data, unsigned long dataNumber);                     //快速离散傅立叶变换，无虚部 (严重警告，因为可能出现扩展内存的情况，所以必须返回新地址)
	bool IFFT(double *data, unsigned long dataNumber);                       //快速离散逆傅立叶变换，无虚部
public:
	CharaParameter(unsigned long frameNumber) {
		this->frameNumber = frameNumber;
		this->frameData = new double* [this->frameNumber];                   //分配指针数组的空间
	}
	
	bool Push_data(unsigned long index, double *frame);                      //初始化特征参数类使用，将index帧的数据存放如类内

	//特征参数求解列表
	
};