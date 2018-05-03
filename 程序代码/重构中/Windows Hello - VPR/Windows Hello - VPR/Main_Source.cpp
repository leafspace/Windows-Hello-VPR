#include <fstream>
#include <iostream>
#include "MediaControl/Common/MediaFile.h"
#include "MediaControl/WAVFile/WavFileBase.h"
#include "CharaParameterControl/CharaParameterControl.h"
#include "PretreatmentControl/PretreatmentControlCommon.h"
#include "OperationalModelControl/GaussianMixtureModel/Model_GMM.h"
#include "OperationalModelControl/GaussianMixtureModel/Model_KMeans.h"

using namespace std;
int main()
{
	// Todo 初始化语音文件类 读取语音文件数据
	FILE *fp;
	if ((fp = fopen("微软默认.wav", "rb")) == NULL) {                                                                // 打开语音文件
		cout << "ERROR : File open failed !" << endl;
		exit(-1);
	}

	MediaFile *wavFile = new WavFile(fp);                                                                           // 读取语音文件数据
	fclose(fp);

	// Todo 预处理
	PretreatmentControlCommon *pretreatmentControl = new PretreatmentControlCommon(wavFile);
	pretreatmentControl->Endpoint_Detection();
	for (unsigned long i = 0; i < pretreatmentControl->Get_voiceNumber(); ++i) {
		pretreatmentControl->Pre_emphasis(pretreatmentControl->Get_dataVoicePoint(i), 
			pretreatmentControl->Get_MediaFileData());                                                              // 对可用范围内的数据进行预加重
	}

	// Todo 初始化特征参数类 计算语音数据特征参数
	double *dataSpace = NULL;

	/*
	//所有的数据分帧加窗
	CharaParameterControl *charaParameter = new CharaParameterControl(pretreatmentControl->Get_frameNumber(wavFile->getDataNumber()));  // 初始化特诊参数类
	for (unsigned long i = 1; i <= pretreatmentControl->Get_frameNumber(wavFile->getDataNumber()); ++i) {           // 逐帧遍历
		dataSpace = new double[PretreatmentControlCommon::N];                                                       // 新建帧数据空间
		memset(dataSpace, 0, sizeof(double) * PretreatmentControlCommon::N);
		pretreatmentControl->Frame_Data(pretreatmentControl->Get_MediaFileData(), wavFile->getDataNumber(), i, dataSpace, PretreatmentControlCommon::N);    // 分帧并加窗
		charaParameter->Push_data(i, dataSpace);                                                                    // 将分帧完成的数据保存进特征参数备用
	}
	*/

	CharaParameterControl *charaParameter = new CharaParameterControl(pretreatmentControl->Get_frameNumber());      // 初始化特诊参数类
	for (unsigned long i = 1; i <= pretreatmentControl->Get_frameNumber(); ++i) {                                   // 逐帧遍历
		dataSpace = new double[PretreatmentControlCommon::N];                                                       // 新建帧数据空间
		memset(dataSpace, 0, sizeof(double) * PretreatmentControlCommon::N);
		pretreatmentControl->Frame_Data(pretreatmentControl->Get_MediaFileData(), i, dataSpace, PretreatmentControlCommon::N);          // 分帧并加窗
		charaParameter->Push_data(i, dataSpace);                                                                    // 将分帧完成的数据保存进特征参数备用
	}

	unsigned long sampleFrequency = wavFile->getSampleFrequency();
	delete wavFile;

	// Todo 计算MFCC参数
	charaParameter->MFCC_CharaParameter(sampleFrequency);                                                           // 计算MFCC特征参数

	/******************************训练示例******************************/

	// Todo 初始化Kmeans数据
	dataSpace = new double[charaParameter->Get_frameNumber() * CharaParameterControl::MelDegreeNumber];
	for (unsigned long i = 0; i < charaParameter->Get_frameNumber(); ++i) {
		memcpy(&dataSpace[i * CharaParameterControl::MelDegreeNumber], 
			charaParameter->Get_frameMelParameter(i), sizeof(double) * CharaParameterControl::MelDegreeNumber);     // 拷贝mfcc数据到一段连续的存储空间中备用
	}

	// Todo 开始Kmeans聚类操作
	KMeans* kmeans = new KMeans(CharaParameterControl::MelDegreeNumber, KMeans::ClusterNumber);                     // 使用阶数跟簇数初始化Kmeans类
	int* labels = new int[charaParameter->Get_frameNumber()];
	kmeans->SetInitMode(KMeans::InitUniform);                                                                       // 设置数据的初始化方法
	kmeans->Cluster(dataSpace, charaParameter->Get_frameNumber(), labels);                                          // 开始聚类

	// Todo  初始化GMM数据
	double **test_data = new double*[KMeans::ClusterNumber];
	for (int i = 0; i < KMeans::ClusterNumber; ++i) {
		test_data[i] = new double[CharaParameterControl::MelDegreeNumber];
		double *tempSpace = kmeans->GetMean(i);
		for (int j = 0; j < CharaParameterControl::MelDegreeNumber; ++j) {
			test_data[i][j] = tempSpace[j];
		}
	}

	delete[]labels;
	delete kmeans;

	// Todo GMM训练数据
	GMM *gmm = new GMM(CharaParameterControl::MelDegreeNumber, GMM::SGMNumber);
	gmm->Train(dataSpace, charaParameter->Get_frameNumber());                                                       // GMM训练数据

	// Todo save GMM to file
	ofstream gmm_file("voiceLib\\gmm.txt");
	assert(gmm_file);
	gmm_file << *gmm;
	gmm_file.close();

	delete gmm;

	/******************************识别示例******************************/

	// Todo 初始化已有库的GMM模型
	int gmmNumber = 5;
	GMM **gmmLib = new GMM*[gmmNumber];
	double *gmmWeight = new double[gmmNumber];
	for (int i = 0; i < gmmNumber; ++i) {
		gmmLib[i] = new GMM(CharaParameterControl::MelDegreeNumber, GMM::SGMNumber);
	}
	gmmWeight[0] = 165;
	gmmWeight[1] = 380;
	gmmWeight[2] = 171;
	gmmWeight[3] = 187;
	gmmWeight[4] = 177;

	ifstream gmm_file_1("voiceLib\\男-wangzhe(我们需要帮助).txt");
	ifstream gmm_file_2("voiceLib\\男-zhanglifei(我们需要帮助).txt");
	ifstream gmm_file_3("voiceLib\\男-zhaozuoxiang(我们需要帮助).txt");
	ifstream gmm_file_4("voiceLib\\女-liuchang(我们需要帮助).txt");
	ifstream gmm_file_5("voiceLib\\女-zhaoquanyin(我们需要帮助).txt");
	assert(gmm_file_1);
	assert(gmm_file_2);
	assert(gmm_file_3);
	assert(gmm_file_4);
	assert(gmm_file_5);

	gmm_file_1 >> *gmmLib[0];
	gmm_file_2 >> *gmmLib[1];
	gmm_file_3 >> *gmmLib[2];
	gmm_file_4 >> *gmmLib[3];
	gmm_file_5 >> *gmmLib[4];
	gmm_file_1.close();
	gmm_file_2.close();
	gmm_file_3.close();
	gmm_file_4.close();
	gmm_file_5.close();

	// Todo 识别计算
	double *libProbability = new double[gmmNumber];
	cout << "Frame number is " << charaParameter->Get_frameNumber() << endl;
	for (int i = 0; i < gmmNumber; ++i) {
		libProbability[i] = 0;
		for (unsigned long j = 0; j < charaParameter->Get_frameNumber(); ++j) {                                     // 计算当前GMM下，目标特征参数集在GMM模型下的概率密度
			double tempData = gmmLib[i]->GetProbability(charaParameter->Get_frameMelParameter(j));                  // 获取GMM的数值
			if (tempData > 0) {                                                                                     // 取对数操作
				tempData = log10(tempData);
			}
			/*
			if (j == 0) {
				cout << tempData << " ";
				if (tempData < 0) {
					cout << log10(fabs(tempData));
				}
			}
			*/
			libProbability[i] += tempData;
		}
		//cout << endl;
	}

	// Todo 对数据长度进行权值计算
	/*
	double sumFrameNumber = 0;
	for (int i = 0; i < gmmNumber; ++i) {
		sumFrameNumber +=gmmWeight[i];
	}
	for (int i = 0; i < gmmNumber; ++i) {                                                                           // 求解数据量的比值
		gmmWeight[i] /= sumFrameNumber;
	}

	int probabilityMax = 0, probabilityMin = 0;
	for (int i = 0; i < gmmNumber; ++i) {                                                                           // 寻找最大最小概率
		if (libProbability[i] > libProbability[probabilityMax]) {
			probabilityMax = i;
		}

		if (libProbability[i] < libProbability[probabilityMin]) {
			probabilityMin = i;
		}
	}

	double probabilityDValue = libProbability[probabilityMax] - libProbability[probabilityMin];
	for (int i = 0; i < gmmNumber; ++i) {
		gmmWeight[i] *= probabilityDValue;                                                                          // 计算补充值
		libProbability[i] += gmmWeight[i];                                                                          // 填充补充值
	}
	*/


	//Todo 男女分开识别
	int countMax = 0;
	/*
	int beginScope = 0, endScope = 4;
	while (beginScope < gmmNumber && libProbability[beginScope] == 0) { beginScope++; }                             // 找到第一个有用的男生模板
	while (endScope > -1 && libProbability[endScope] == 0) { endScope--; }                                          // 找到第一个有用的女生模板

	if (beginScope >= endScope) {
		cout << "Can't find useful model !" << endl;
		return 0;
	}

	if (libProbability[beginScope] > libProbability[endScope]) {
		beginScope = 0;
		endScope   = 3;
		cout << "This is a man." << endl;
	} else {
		beginScope = 3;
		endScope   = 5;
		cout << "This is a women." << endl;
	}
	*/
	cout << "TIP : Probability data is ";
	/*
	while (beginScope < gmmNumber && libProbability[beginScope] == 0) { beginScope++; }
	for (int i = beginScope, countMax = beginScope; i < endScope; ++i) {
		cout << libProbability[i] << "\t";
		if (libProbability[i] > libProbability[countMax] && libProbability[i] != 0) {
			countMax = i;
		}
	}
	*/
	for (int i = 0; i < 5; ++i) {
		cout << libProbability[i] << "\t";
		if (libProbability[i] > libProbability[countMax]) {
			countMax = i;
		}
	}
	cout << endl;

	switch (countMax)
	{
	case 0 : cout << "Reservation is wangzhe." << endl;      break;
	case 1 : cout << "Reservation is zhanglifei." << endl;   break;
	case 2 : cout << "Reservation is zhaozuoxiang." << endl; break;
	case 3 : cout << "Reservation is liuchang." << endl;     break;
	case 4 : cout << "Reservation is zhaoquanyin." << endl;  break;
	default: cout << "Can't find result." << endl; break;
	}
	return 0;
}