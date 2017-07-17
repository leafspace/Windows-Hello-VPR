#include "WavData_CharaParameter.h"

int main()
{

	FILE *fp;
	if ((fp = fopen("微软默认.wav", "rb")) == NULL) {                                                                 //打开语音文件
		cout << "ERROR : File open failed !" << endl;
		exit(-1);
	}

	//Todo 初始化语音文件类 读取语音文件数据
	WavFile_Initial *wavFile = new WavFile_Initial(fp);                                                              //读取语音文件数据
	fclose(fp);
	for (unsigned long i = 0; i < wavFile->Get_voiceNumber(); ++i) {
		wavFile->Pre_emphasis(wavFile->Get_dataVoicePoint(i), wavFile->Get_WavFileData());                           //对可用范围内的数据进行预加重
	}

	//Todo 初始化特征参数类 计算语音数据特征参数
	double *dataSpace = NULL;
	CharaParameter *charaParameter = new CharaParameter(wavFile->Get_frameNumber(wavFile->Get_dataNumber()));        //初始化特诊参数类
	for (unsigned long i = 1; i <= wavFile->Get_frameNumber(wavFile->Get_dataNumber()); ++i) {                       //逐帧遍历
		dataSpace = new double[WavFile_Initial::N];                                                                  //新建帧数据空间
		wavFile->Frame_Data(wavFile->Get_WavFileData(), wavFile->Get_dataNumber(), i, dataSpace, WavFile_Initial::N);//分帧并加窗
		charaParameter->Push_data(i, dataSpace);                                                                     //将分帧完成的数据保存进特征参数备用
	}
	unsigned long sampleRate = wavFile->Get_SampleRate();
	delete wavFile;

	charaParameter->MFCC_CharaParameter(sampleRate);                                                                 //计算MFCC特征参数
	return 0;
}