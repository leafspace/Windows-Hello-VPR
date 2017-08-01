#include "WavData_CharaParameter.h"
#include "Model_KMeans.h"
#include "Model_GMM.h"
#include <fstream>

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
	
	/*
	//所有的数据分帧加窗
	CharaParameter *charaParameter = new CharaParameter(wavFile->Get_frameNumber(wavFile->Get_dataNumber()));        //初始化特诊参数类
	for (unsigned long i = 1; i <= wavFile->Get_frameNumber(wavFile->Get_dataNumber()); ++i) {                       //逐帧遍历
		dataSpace = new double[WavFile_Initial::N];                                                                  //新建帧数据空间
		memset(dataSpace, 0, sizeof(double) * WavFile_Initial::N);
		wavFile->Frame_Data(wavFile->Get_WavFileData(), wavFile->Get_dataNumber(), i, dataSpace, WavFile_Initial::N);//分帧并加窗
		charaParameter->Push_data(i, dataSpace);                                                                     //将分帧完成的数据保存进特征参数备用
	}
	*/
	
	CharaParameter *charaParameter = new CharaParameter(wavFile->Get_frameNumber());                                 //初始化特诊参数类
	for (unsigned long i = 1; i <= wavFile->Get_frameNumber(); ++i) {                                                //逐帧遍历
		dataSpace = new double[WavFile_Initial::N];                                                                  //新建帧数据空间
		memset(dataSpace, 0, sizeof(double) * WavFile_Initial::N);
		wavFile->Frame_Data(wavFile->Get_WavFileData(), i, dataSpace, WavFile_Initial::N);                           //分帧并加窗
		charaParameter->Push_data(i, dataSpace);                                                                     //将分帧完成的数据保存进特征参数备用
	}
	
	unsigned long sampleRate = wavFile->Get_SampleRate();
	delete wavFile;

	//Todo 计算MFCC参数
	charaParameter->MFCC_CharaParameter(sampleRate);                                                                 //计算MFCC特征参数


	//Todo 初始化Kmeans数据
	dataSpace = new double[charaParameter->Get_frameNumber() * CharaParameter::MelDegreeNumber];
	for (unsigned long i = 0; i < charaParameter->Get_frameNumber(); ++i) {
		memcpy(&dataSpace[i * CharaParameter::MelDegreeNumber], 
			charaParameter->Get_frameMelParameter(i), sizeof(double) * CharaParameter::MelDegreeNumber);             //拷贝mfcc数据到一段连续的存储空间中备用
	}

	//Todo 开始Kmeans聚类操作
	KMeans* kmeans = new KMeans(CharaParameter::MelDegreeNumber, KMeans::ClusterNumber);                             //使用阶数跟簇数初始化Kmeans类
	int* labels = new int[charaParameter->Get_frameNumber()];
	kmeans->SetInitMode(KMeans::InitUniform);                                                                        //设置数据的初始化方法
	kmeans->Cluster(dataSpace, charaParameter->Get_frameNumber(), labels);                                           //开始聚类

	//Todo  初始化GMM数据
	double **test_data = new double*[KMeans::ClusterNumber];
	for (int i = 0; i < KMeans::ClusterNumber; ++i) {
		test_data[i] = new double[CharaParameter::MelDegreeNumber];
		double *tempSpace = kmeans->GetMean(i);
		for (int j = 0; j < CharaParameter::MelDegreeNumber; ++j) {
			test_data[i][j] = tempSpace[j];
		}
	}

	delete[]labels;
	delete kmeans;

	//Todo GMM训练数据
	GMM *gmm = new GMM(CharaParameter::MelDegreeNumber, GMM::SGMNumber);
	gmm->Train(dataSpace, charaParameter->Get_frameNumber());                                                        //GMM训练数据

	//Todo save GMM to file
	ofstream gmm_file("voiceLib\\gmm.txt");
	assert(gmm_file);
	gmm_file << *gmm;
	gmm_file.close();

	delete gmm;

	/******************************识别示例******************************/

	//Todo 初始化已有库的GMM模型
	GMM **gmmLib = new GMM*[2];
	gmmLib[0] = new GMM(CharaParameter::MelDegreeNumber, GMM::SGMNumber);
	gmmLib[1] = new GMM(CharaParameter::MelDegreeNumber, GMM::SGMNumber);

	ifstream gmm_file_ms("voiceLib\\ms-gmm(180123456).txt");
	ifstream gmm_file_xf("voiceLib\\xf-gmm(180123456).txt");
	assert(gmm_file_ms);
	assert(gmm_file_xf);

	gmm_file_ms >> *gmmLib[0];
	gmm_file_xf >> *gmmLib[1];
	gmm_file_ms.close();
	gmm_file_xf.close();




	return 0;
}