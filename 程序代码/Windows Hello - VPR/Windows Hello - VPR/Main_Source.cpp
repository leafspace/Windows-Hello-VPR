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

void Gmm_Train()
{
		double data[] = {
		0.0, 0.2, 0.4,
		0.3, 0.2, 0.4,
		0.4, 0.2, 0.4,
		0.5, 0.2, 0.4,
		5.0, 5.2, 8.4,
		6.0, 5.2, 7.4,
		4.0, 5.2, 4.4,
		10.3, 10.4, 10.5,
		10.1, 10.6, 10.7,
		11.3, 10.2, 10.9
	};

	const int size = 10; //Number of samples
	const int dim = 3;   //Dimension of feature
	const int cluster_num = 4; //Cluster number

	KMeans* kmeans = new KMeans(dim, cluster_num);
	int* labels = new int[size];
	kmeans->SetInitMode(KMeans::InitUniform);
	kmeans->Cluster(data, size, labels);

	printf("Clustering result by k-meams:\n");
	for (int i = 0; i < size; ++i)
	{
		printf("%f, %f, %f belongs to %d cluster\n", data[i*dim + 0], data[i*dim + 1], data[i*dim + 2], labels[i]);
	}

	double test_data[4][3];
	for (int i = 0; i < 4; ++i) {
		double *temp = kmeans->GetMean(i);
		for (int j = 0; j < 3; ++j) {
			test_data[i][j] = temp[j];
		}
	}

	delete[]labels;
	delete kmeans;

	GMM *gmm = new GMM(dim, 3); //GMM has 3 SGM
	gmm->Train(data, size);     //Training GMM

	printf("\nTest GMM:\n");
	for (int i = 0; i < 4; ++i)
	{
		printf("The Probability of %f, %f, %f  is %f \n", test_data[i][0], test_data[i][1], test_data[i][2], gmm->GetProbability(test_data[i]));
	}

	//save GMM to file
	ofstream gmm_file("gmm.txt");
	assert(gmm_file);
	gmm_file << *gmm;
	gmm_file.close();

	delete gmm;
}