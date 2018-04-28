#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")                                         //不显示控制台

#include <io.h>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include "LogSystem.h"
#include "Model_GMM.h"
#include "Model_KMeans.h"
#include "CChineseCode.h"
#include "WaveRecorder.h"
#include "WavData_CharaParameter.h"

string getFileName(string path);                                                                                     //将某个路径转换为某个文件名
void writePassword(const char* password);
void getFiles(string path, vector<string>& files);                                                                   //获取path文件夹下的所有文件名
bool recognitionMethods(vector<string> files, double *libProbability, int countMax, int gmmNumber, int methods);     //通过不同的方式进行判断

LogSystem *p_logSystem;                                                                                              //用于处理Log相关

int main()
{
	p_logSystem = new LogSystem();                                                                                   //初始化日志系统
	p_logSystem->linkState = p_logSystem->initSocket();
	p_logSystem->beginSystem();                                                                                      //开启线程发送消息

	p_logSystem->sendMessage("<Type>\n");                                                                            //发送类型消息
	p_logSystem->sendMessage("Windows Hello\n");
	p_logSystem->writeMessage("==============================================\n");

	//Todo 录音
	//LogSystem send message
	if (p_logSystem->linkState) {
		p_logSystem->sendMessage("<Message>\n");
		p_logSystem->sendMessage("TIP : System begin to start recorder. \n");
	}
	p_logSystem->writeMessage("TIP : System begin to start recorder. \n");

	cout << "TIP : System begin to start recorder. " << endl;
	//count down
	PlaySound(TEXT("resourse\\countdown03.wav"), NULL, SND_SYNC | SND_FILENAME);
	PlaySound(TEXT("resourse\\countdown02.wav"), NULL, SND_SYNC | SND_FILENAME);
	PlaySound(TEXT("resourse\\countdown01.wav"), NULL, SND_SYNC | SND_FILENAME);
	PlaySound(TEXT("resourse\\countdown00.wav"), NULL, SND_SYNC | SND_FILENAME);

	WaveRecorder waveRecorder;
	waveRecorder.set_FileName("tempRecord.wav");
	waveRecorder.Start();
	Sleep(5500);
	waveRecorder.Stop();
	waveRecorder.Reset();


	FILE *fp = NULL;
	if ((fp = fopen("tempRecord.wav", "rb")) == NULL) {                                                              //打开语音文件
		cout << "ERROR : File open failed !" << endl;
		//LogSystem send message
		if (p_logSystem->linkState) {
			p_logSystem->sendMessage("<Message>\n");
			p_logSystem->sendMessage("ERROR : File open failed !\n");
		}
		p_logSystem->writeMessage("ERROR : File open failed !\n");
		PlaySound(TEXT("resourse\\failed.wav"), NULL, SND_ASYNC | SND_FILENAME);
		exit(-1);
	}

	//Todo 初始化语音文件类 读取语音文件数据
	//LogSystem send message
	if (p_logSystem->linkState) {
		p_logSystem->sendMessage("<Message>\n");
		p_logSystem->sendMessage("TIP : Init voice file, read voice data !\n");
	}
	p_logSystem->writeMessage("TIP : Init voice file, read voice data !\n");

	WavFile_Initial *wavFile = new WavFile_Initial(fp);                                                              //读取语音文件数据
	fclose(fp);
	for (unsigned long i = 0; i < wavFile->Get_voiceNumber(); ++i) {
		wavFile->Pre_emphasis(wavFile->Get_dataVoicePoint(i), wavFile->Get_WavFileData());                           //对可用范围内的数据进行预加重
	}

	//Todo 初始化特征参数类 计算语音数据特征参数
	//LogSystem send message
	if (p_logSystem->linkState) {
		p_logSystem->sendMessage("<Message>\n");
		p_logSystem->sendMessage("TIP : Init charaparameter !\n");
	}
	p_logSystem->writeMessage("TIP : Init charaparameter !\n");

	double *dataSpace = NULL;

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
	//LogSystem send message
	if (p_logSystem->linkState) {
		p_logSystem->sendMessage("<Message>\n");
		p_logSystem->sendMessage("TIP : Caculater MFCC parameter !\n");
	}
	p_logSystem->writeMessage("TIP : Caculater MFCC parameter !\n");

	charaParameter->MFCC_CharaParameter(sampleRate);                                                                 //计算MFCC特征参数

	/******************************识别示例******************************/
	//LogSystem send message
	if (p_logSystem->linkState) {
		p_logSystem->sendMessage("<Message>\n");
		p_logSystem->sendMessage("TIP : Begin recognition !\n");
	}
	p_logSystem->writeMessage("TIP : Begin recognition !\n");

	char szModuleFilePath[MAX_PATH];
	int n = GetModuleFileNameA(0, szModuleFilePath, MAX_PATH);               //获得当前执行文件的路径
	szModuleFilePath[strrchr(szModuleFilePath, '\\') - szModuleFilePath + 1] = 0;      //将最后一个"\\"后的字符置为0

	int index = 0;
	char filePath[MAX_PATH];
	for (int i = 0; i < (int)strlen(szModuleFilePath); ++i) {               //补全//
		filePath[index++] = szModuleFilePath[i];
		if (szModuleFilePath[i] == '\\') {
			filePath[index++] = '\\';
		}
	}
	filePath[index++] = 0;                                                   //末尾归零

	char path[MAX_PATH];
	strcpy_s(path, filePath);
	strcat_s(path, "voiceLib");

	vector<string> files;
	getFiles(path, files);                                                   //获取文件夹内的所有文件名（路径名）


	//Todo 初始化已有库的GMM模型
	//LogSystem send message
	if (p_logSystem->linkState) {
		p_logSystem->sendMessage("<Message>\n");
		p_logSystem->sendMessage("TIP : Init library gmm model !\n");
	}
	p_logSystem->writeMessage("TIP : Init library gmm model !\n");

	int gmmNumber = files.size();
	GMM **gmmLib = new GMM*[gmmNumber];
	ifstream *gmmFiles = new ifstream[gmmNumber];
	for (int i = 0; i < gmmNumber; ++i) {
		gmmLib[i] = new GMM(CharaParameter::MelDegreeNumber, GMM::SGMNumber);
		gmmFiles[i].open(files[i]);
		assert(gmmFiles[i]);
		gmmFiles[i] >> *gmmLib[i];
		gmmFiles[i].close();
	}

	//Todo 识别计算
	//LogSystem send message
	if (p_logSystem->linkState) {
		p_logSystem->sendMessage("<Message>\n");
		p_logSystem->sendMessage("TIP : Begin reservation ... !\n");
	}
	p_logSystem->writeMessage("TIP : Begin reservation ... !\n");

	cout << "TIP : Begin reservation ..." << endl;
	double *libProbability = new double[gmmNumber];
	cout << "Frame number is " << charaParameter->Get_frameNumber() << endl;
	for (int i = 0; i < gmmNumber; ++i) {
		libProbability[i] = 0;
		for (unsigned long j = 0; j < charaParameter->Get_frameNumber(); ++j) {                                      //计算当前GMM下，目标特征参数集在GMM模型下的概率密度
			double tempData = gmmLib[i]->GetProbability(charaParameter->Get_frameMelParameter(j));                   //获取GMM的数值
			if (tempData > 0) {                                                                                      //取对数操作
				tempData = log10(tempData);
			}
			libProbability[i] += tempData;
		}
	}

	int countMax = 0;
	char tempData[24];
	char resultName[1024] = {}, resultData[1024] = {};
	cout << "TIP : Probability data is ";
	for (int i = 0; i < gmmNumber; ++i) {
		cout << libProbability[i] << "\t";
		if (libProbability[i] > libProbability[countMax]) {
			countMax = i;
		}

		if (i != gmmNumber - 1) {
			strcat_s(resultName, getFileName(files[i]).data());
			strcat_s(resultName, "|");
			sprintf(tempData, "%lf", libProbability[i]);
			strcat_s(resultData, tempData);
			strcat_s(resultData, "|");
		}
		else {
			strcat_s(resultName, getFileName(files[i]).data());
			sprintf(tempData, "%lf", libProbability[i]);
			strcat_s(resultData, tempData);
			strcat_s(resultName, "\n");
			strcat_s(resultData, "\n");
		}

	}
	cout << endl;

	//LogSystem send message
	if (p_logSystem->linkState) {
		p_logSystem->sendMessage("<Result>\n");
		p_logSystem->sendMessage(resultName);
		p_logSystem->sendMessage(resultData);
	}
	p_logSystem->writeMessage(resultName);
	p_logSystem->writeMessage(resultData);


	int method = 1;
	ReadConfig *readConfig = new ReadConfig;                                                                         //打开文件读取
	bool isSuccess = readConfig->ReadFile();                                                                         //读取文件
	if (isSuccess) {
		method = atoi(readConfig->getMethod().c_str());
	}
	delete readConfig;
	if (recognitionMethods(files, libProbability, countMax, gmmNumber, method)) {
		cout << "TIP : Recognition successfully ! - Login..." << endl;

		//LogSystem send message
		if (p_logSystem->linkState) {
			p_logSystem->sendMessage("<Message>\n");
			p_logSystem->sendMessage("TIP : Recognition successfully ! - Login...!\n");
		}
		p_logSystem->writeMessage("TIP : Recognition successfully ! - Login...!\n");

		ReadConfig *readConfig = new ReadConfig;                                                                     //打开文件读取
		bool isSuccess = readConfig->ReadFile();                                                                     //读取文件
		if (isSuccess) {
			writePassword(readConfig->getPassword().data());
		}
		delete readConfig;

		string str;
		//CChineseCode::GB2312ToUTF_8(str, password, strlen(password));
		
		p_logSystem->sendMessage("<File>\n");
		Sleep(500);
		p_logSystem->sendFile("tempRecord.wav");
		//p_logSystem->sendMessage("<Finish>\n");
		PlaySound(TEXT("resourse\\success.wav"), NULL, SND_ASYNC | SND_FILENAME);
	}
	else {
		cout << "ERROR : User Unknow !" << endl;
		//LogSystem send message
		if (p_logSystem->linkState) {
			p_logSystem->sendMessage("<Message>\n");
			p_logSystem->sendMessage("ERROR : User Unknow ! \n");
			p_logSystem->sendMessage("<Message>\n");
			p_logSystem->sendMessage("ERROR : 对不起，您没有权限登陆 ! ! \n");
		}
		p_logSystem->writeMessage("ERROR : User Unknow ! \n");
		p_logSystem->writeMessage("ERROR : 对不起，您没有权限登陆 ! \n");

		p_logSystem->sendMessage("<File>\n");
		Sleep(500);
		p_logSystem->sendFile("tempRecord.wav");

		PlaySound(TEXT("resourse\\failed.wav"), NULL, SND_ASYNC | SND_FILENAME);
		MessageBoxA(NULL, "对不起，您没有权限登陆 !", "错误", MB_ICONHAND);
	}

	// 备份录音数据
	char* fileName = new char[256];
	memset(fileName, 0, 256);
	time_t date = time(0);
	sprintf(fileName, "temp\\%d%d%d%d%d%d.wav", localtime(&date)->tm_year,
		localtime(&date)->tm_mon, localtime(&date)->tm_yday,
		localtime(&date)->tm_hour, localtime(&date)->tm_min,
		localtime(&date)->tm_sec);                                                                               //格式化文件名

	CopyFileA("tempRecord.wav", fileName, FALSE);
	delete fileName;

	remove("tempRecord.wav");
	return 0;
}

string getFileName(string path)                                                                                      //将某个路径转换为某个文件名
{
	return path.substr(path.rfind('\\') + 1, path.size() - path.rfind('\\') - 1);
}

void writePassword(const char* password)
{
	//Sleep(5000);
	HWND hWnd = GetForegroundWindow();                                                                               //获得当前激活的窗口句柄
	DWORD SelfThreadId = GetCurrentThreadId();                                                                       //获取本身的线程ID
	DWORD ForeThreadId = GetWindowThreadProcessId(hWnd, NULL);                                                       //根据窗口句柄获取线程ID
	AttachThreadInput(ForeThreadId, SelfThreadId, true);                                                             //附加线程
	hWnd = GetFocus();                                                                                               //获取具有输入焦点的窗口句柄
	AttachThreadInput(ForeThreadId, SelfThreadId, false);                                                            //取消附加的线程
	for (int i = 0; i < (int)strlen(password); ++i) {
		SendMessage(hWnd, WM_CHAR, WPARAM(password[i]), 0);                                                          //发送一个字消息
	}
	SendMessage(hWnd, WM_KEYDOWN, VK_RETURN, 0);
}

void getFiles(string path, vector<string>& files)                                                                    //获取path文件夹下的所有文件名
{
	long hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
		do {
			if ((fileinfo.attrib & _A_SUBDIR)) {                                                                     //判断是否为文件夹
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
			}
			else {
				files.push_back(p.assign(path).append("\\\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

bool recognitionMethods(vector<string> files, double *libProbability, int countMax, int gmmNumber, int methods)
{
	if (methods == 0) {                                                                                              //通过比较最大值文件是否为me.txt的方式来获取结果
		//LogSystem send message
		if (p_logSystem->linkState) {
			p_logSystem->sendMessage("<Message>\n");
			p_logSystem->sendMessage("TIP : Use max probability to recognition .\n");
		}
		p_logSystem->writeMessage("TIP : Use max probability to recognition .\n");

		if (countMax < (int) files.size()) {
			if (strcmp(getFileName(files[countMax]).data(), "me.txt") == 0) {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}
	else {                                                                                                           //通过比较数据是否在设置的门限之间的方式来判断
		//LogSystem send message
		if (p_logSystem->linkState) {
			p_logSystem->sendMessage("<Message>\n");
			p_logSystem->sendMessage("TIP : Use probability max and min to recognition .\n");
		}
		p_logSystem->writeMessage("TIP : Use probability max and min to recognition .\n");

		double threshold = 0;
		double probability = 0;
		double probabilityMax = 0, probabilityMin = 0;

		ReadConfig *readConfig = new ReadConfig;                                                                     //打开文件读取
		bool isSuccess = readConfig->ReadFile();                                                                     //读取文件
		if (isSuccess) {
			threshold = atof(readConfig->getThreshold().c_str());
			probability = atof(readConfig->getProbability().c_str());
		}
		delete readConfig;

		if (threshold >= 1) {
			threshold /= 100;
		}
		probabilityMax = probability + fabs(probability) * threshold;
		probabilityMin = probability - fabs(probability) * threshold;

		int index = -1;
		for (int i = 0; i < gmmNumber; ++i) {
			if (i < (int) files.size()) {
				if (strcmp(getFileName(files[i]).data(), "me.txt") == 0) {
					index = i;
					break;
				}
			}
		}

		if (index == -1) {
			// can't find me.txt file
			return false;
		}

		//LogSystem send message
		char tempStr[256];
		sprintf(tempStr, "TIP : Probability max (%lf) and min (%lf) to recognition .\n", probabilityMax, probabilityMin);
		if (p_logSystem->linkState) {
			p_logSystem->sendMessage("<Message>\n");
			p_logSystem->sendMessage(tempStr);
		}
		p_logSystem->writeMessage(tempStr);

		if (libProbability[index] >= probabilityMin && libProbability[index] <= probabilityMax) {
			return true;
		} 
		else {
			return false;
		}
	}
}