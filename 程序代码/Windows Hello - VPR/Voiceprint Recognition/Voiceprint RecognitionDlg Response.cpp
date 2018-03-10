#include "stdafx.h"
#include "VPR/Model_GMM.h"
#include "VPR/Model_KMeans.h"
#include "Voiceprint RecognitionDlg.h"
#include "VPR/WavData_CharaParameter.h"

WaveRecorder waveRecorder;                                                   //全局录音对象
char* fileName;                                                              //文件对比之用
string fileName_t;                                                           //文件对比之用
string playpath;                                                             //保存需要播放的语音的路径
double* mfccData;                                                            //用于保存当前语音训练出的数据
CharaParameter* charaParameter;                                              //用于保存当前语音训练出的参数

void CVoiceprintRecognitionDlg::CompoundFile(vector<FILESTRUCT>& fileLib, int flag)    //用于将txt信息与当前文件夹下内容相结合
{
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
	if (flag == 0) {                                                         //按照要求连接文件夹
		strcat_s(path, "wavLib");
	}
	else if (flag == 1) {
		strcat_s(path, "voiceLib");
	}

	vector<string> files;
	getFiles(path, files);                                                   //获取文件夹内的所有文件名（路径名）

	strcat_s(path, "\\\\info.list");                                         //指定文件夹的读取文件
	ifstream in(path);
	fileLib.clear();
	readList(in, fileLib);                                                   //读取list文件内容
	in.close();

	vector<FILESTRUCT> newLib;
	for (int i = 0, j = 0; i < (int)files.size(); ++i) {
		string fileName = files[i];
		if (strcmp(getFileName(fileName).data(), "info.list") == 0) {        //如果当前文件的文件名为info.list说明其为配置文件
			continue;
		}
		for (j = 0; j < (int)fileLib.size(); ++j) {
			if (strcmp(getFileName(fileName).data(), fileLib[j].fileName.data()) == 0) {         //如果当前文件名与列表名相匹配，则说明这是一个不变的列表项
				newLib.push_back(fileLib[j]);
				break;
			}
		}
		if (j == fileLib.size()) {
			FILESTRUCT item;
			item.fileName = getFileName(fileName).data();
			if (flag == 0) {
				if (::fileName != NULL && (strcmp(::fileName_t.data(), fileName.data()) == 0)) { //如果录音文件路径与当前这个未知的文件路径相同
					CString str;
					GetDlgItem(IDC_EDIT1)->GetWindowText(str);               //获取所属人名
					string tempstr = CStringA(str);
					if (tempstr.size() == 0) {                               //所属人名没写
						item.peopleName = "未知";
					}
					else {
						item.peopleName = tempstr;
					}
				}
				else {
					item.peopleName = "未知";
				}
			}
			else if (flag == 1) {
				item.peopleName = "未知";
			}
			newLib.push_back(item);
		}
	}

	ofstream wavOut(path);
	writeList(wavOut, newLib);                                               //重新写入数据
	wavOut.close();

	fileLib.clear();
	fileLib = newLib;
}

int CVoiceprintRecognitionDlg::GetItemSelect(int index)                      //获取某个listControl当前选中项的行号
{
	int count = 0;
	switch (index)
	{
	case 0: count = this->listCtrl_1.GetItemCount(); break;
	case 1: count = this->listCtrl_2.GetItemCount(); break;
	default: break;
	}

	for (int i = 0; i < count; ++i) {
		switch (index)
		{
		case 0:
			if (this->listCtrl_1.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED) {
				return i;
			}
			break;
		case 1:
			if (this->listCtrl_2.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED) {
				return i;
			}
			break;
		default: break;
		}
	}
	return -1;
}

bool CVoiceprintRecognitionDlg::OnButton1_record(char* fileName)             //开启录音线程
{
	::fileName = fileName;
	pthread_attr_t attr;                                                     //线程属性结构体，创建线程时加入的参数  
	pthread_attr_init(&attr);                                                //初始化
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);             //是设置你想要指定线程属性参数
	int ret = pthread_create(&thread_recordID, &attr, record, (void*)&fileName);
	if (ret != 0) {
		MessageBoxA(NULL, "ERROR : Can't create thread !", "ERROR", MB_ICONHAND);
		return false;
	}
	return true;
}

bool CVoiceprintRecognitionDlg::OnButton1_cancel()                           //结束录音
{
	::waveRecorder.Stop();
	::waveRecorder.Reset();
	pthread_cancel(this->thread_recordID);
	return true;
}

bool CVoiceprintRecognitionDlg::OnButton4_refresh()                          //录音文件刷新
{
	CString str_f, str_p;
	listCtrl_1.DeleteAllItems();
	for (int i = 0; i < (int) this->wavLib.size(); ++i) {
		FILESTRUCT item = this->wavLib[i];
		str_f = item.fileName.c_str();
		str_p = item.peopleName.c_str();
		listCtrl_1.InsertItem(i, str_f);                                     //设置列表文件名信息
		listCtrl_1.SetItemText(i, 1, str_p);                                 //设置列表用户信息
	}
	return true;
}

bool CVoiceprintRecognitionDlg::OnButton5_refresh()                          //模型文件刷新
{
	CString str_f, str_p;
	listCtrl_2.DeleteAllItems();
	for (int i = 0; i < (int) this->voiceLib.size(); ++i) {
		FILESTRUCT item = this->voiceLib[i];
		str_f = item.fileName.c_str();
		str_p = item.peopleName.c_str();
		listCtrl_2.InsertItem(i, str_f);                                     //设置列表文件名信息
		listCtrl_2.SetItemText(i, 1, str_p);;                                //设置列表用户信息
	}
	return true;
}

void* record(void* args)                                                     //录音线程
{
	::waveRecorder.set_FileName((char*)fileName);
	::waveRecorder.Start();
	delete fileName;
	return NULL;
}

void* player(void* args)                                                     //放音线程
{
	PlaySound((CString)(::playpath.data()), NULL, SND_FILENAME | SND_ASYNC);
	return NULL;
}

string getFileName(string path)                                              //将某个路径转换为某个文件名
{
	return path.substr(path.rfind('\\') + 1, path.size() - path.rfind('\\') - 1);
}

void getFiles(string path, vector<string>& files)                            //获取path文件夹下的所有文件名
{
	long hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
		do {
			if ((fileinfo.attrib & _A_SUBDIR)) {                              //判断是否为文件夹
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

void readList(ifstream& in, vector<FILESTRUCT>& list)                        //读取文件的内容到list中
{
	char buffer[512];
	string str_f, str_p;
	char fileName[256], peopleName[256];
	if (in.is_open()) {
		while (!in.eof()) {
			in.getline(buffer, 512);
			if (strlen(buffer) == 0) {                                       //防止到了最后一行只是一个换行还重复读取
				continue;
			}
			sscanf(buffer, "%s %s", &fileName, &peopleName);                 //格式化字符串
			CChineseCode::UTF_8ToGB2312(str_f, fileName, strlen(fileName));
			CChineseCode::UTF_8ToGB2312(str_p, peopleName, strlen(peopleName));
			FILESTRUCT item(str_f, str_p);
			list.push_back(item);
			memset(buffer, 0, 512);
		}
	}
}

void writeList(ofstream& out, vector<FILESTRUCT>& list)                      //将list文件内容写入数据流
{
	string str_f, str_p;
	if (out.is_open()) {
		for (int i = 0; i < (int)list.size(); ++i) {
			CChineseCode::GB2312ToUTF_8(str_f, (char*)list[i].fileName.data(), list[i].fileName.length());
			CChineseCode::GB2312ToUTF_8(str_p, (char*)list[i].peopleName.data(), list[i].peopleName.length());
			out << str_f.data() << "\t" << str_p.data() << endl;
		}
	}
}

bool extractParameter(string wavfilePath)                                    //训练目标路径的语音文件的特征参数
{
	FILE *fp;
	if ((fp = fopen(wavfilePath.data(), "rb")) == NULL) {                    //打开语音文件
		//LogSystem send message
		if (p_logSystem->linkState) {
			p_logSystem->sendMessage("<Message>\n");
			p_logSystem->sendMessage("ERROR : Voice file open failed ! \n");
		}
		p_logSystem->writeMessage("ERROR : Voice file open failed ! \n");
		return false;
	}

	//Todo 初始化语音文件类 读取语音文件数据
	//LogSystem send message
	if (p_logSystem->linkState) {
		p_logSystem->sendMessage("<Message>\n");
		p_logSystem->sendMessage("Info : Read voice file data ! \n");
	}
	p_logSystem->writeMessage("Info : Read voice file data ! \n");

	WavFile_Initial *wavFile = new WavFile_Initial(fp);                      //读取语音文件数据
	fclose(fp);
	for (unsigned long i = 0; i < wavFile->Get_voiceNumber(); ++i) {
		wavFile->Pre_emphasis(wavFile->Get_dataVoicePoint(i), wavFile->Get_WavFileData());       //对可用范围内的数据进行预加重
	}

	//Todo 初始化特征参数类 计算语音数据特征参数
	//LogSystem send message
	if (p_logSystem->linkState) {
		p_logSystem->sendMessage("<Message>\n");
		p_logSystem->sendMessage("Info : Initial characteristic parameters are calculated ! \n");
	}
	p_logSystem->writeMessage("Info : Initial characteristic parameters are calculated ! \n");

	double *dataSpace = NULL;

	CharaParameter *charaParameter = new CharaParameter(wavFile->Get_frameNumber());             //初始化特诊参数类
	for (unsigned long i = 1; i <= wavFile->Get_frameNumber(); ++i) {        //逐帧遍历
		dataSpace = new double[WavFile_Initial::N];                          //新建帧数据空间
		memset(dataSpace, 0, sizeof(double) * WavFile_Initial::N);
		wavFile->Frame_Data(wavFile->Get_WavFileData(), i, dataSpace, WavFile_Initial::N);       //分帧并加窗
		charaParameter->Push_data(i, dataSpace);                             //将分帧完成的数据保存进特征参数备用
	}

	unsigned long sampleRate = wavFile->Get_SampleRate();
	delete wavFile;

	//Todo 计算MFCC参数
	//LogSystem send message
	if (p_logSystem->linkState) {
		p_logSystem->sendMessage("<Message>\n");
		p_logSystem->sendMessage("Info : Initial MFCC parameters are calculated ! \n");
	}
	p_logSystem->writeMessage("Info : Initial MFCC parameters are calculated ! \n");

	charaParameter->MFCC_CharaParameter(sampleRate);                         //计算MFCC特征参数

	//Todo 初始化Kmeans数据
	//LogSystem send message
	if (p_logSystem->linkState) {
		p_logSystem->sendMessage("<Message>\n");
		p_logSystem->sendMessage("Info : Initial K-means data ! \n");
	}
	p_logSystem->writeMessage("Info : Initial K-means data ! \n");

	::mfccData = new double[charaParameter->Get_frameNumber() * CharaParameter::MelDegreeNumber];
	for (unsigned long i = 0; i < charaParameter->Get_frameNumber(); ++i) {
		memcpy(&::mfccData[i * CharaParameter::MelDegreeNumber],
			charaParameter->Get_frameMelParameter(i), sizeof(double) * CharaParameter::MelDegreeNumber);             //拷贝mfcc数据到一段连续的存储空间中备用
	}

	::charaParameter = charaParameter;
	return true;
}

bool trainingWAV(string wavfilePath)                                         //训练wav文件，将当前语音计算出gmm模型，再将模型数据放入模型中，计算出概率值
{
	bool success = extractParameter(wavfilePath);
	if (!success) {
		return false;
	}

	//Todo 开始Kmeans聚类操作
	KMeans* kmeans = new KMeans(CharaParameter::MelDegreeNumber, KMeans::ClusterNumber);         //使用阶数跟簇数初始化Kmeans类
	int* labels = new int[::charaParameter->Get_frameNumber()];
	kmeans->SetInitMode(KMeans::InitUniform);                                //设置数据的初始化方法
	kmeans->Cluster(::mfccData, ::charaParameter->Get_frameNumber(), labels);//开始聚类

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
	gmm->Train(::mfccData, ::charaParameter->Get_frameNumber());             //GMM训练数据

	double probability = 0;
	for (unsigned long i = 0; i < charaParameter->Get_frameNumber(); ++i) {
		double tempData = gmm->GetProbability(charaParameter->Get_frameMelParameter(i));
		if (tempData > 0) {
			tempData = log10(tempData);
		}
		probability += tempData;
	}
	delete gmm;

	//Todo 将probability数据写入到文件中
	ofstream out;
    out.open("info.conf", ios::app);
	out << endl;
    out << "<probability>" << probability << "</probability>";
	out.close();

	return true;
}

bool trainingWAV(string wavfilePath, string gmmfilePath)                     //训练wav文件
{
	bool success = extractParameter(wavfilePath);
	if (!success) {
		return false;
	}

	//Todo 开始Kmeans聚类操作
	KMeans* kmeans = new KMeans(CharaParameter::MelDegreeNumber, KMeans::ClusterNumber);         //使用阶数跟簇数初始化Kmeans类
	int* labels = new int[::charaParameter->Get_frameNumber()];
	kmeans->SetInitMode(KMeans::InitUniform);                                //设置数据的初始化方法
	kmeans->Cluster(::mfccData, ::charaParameter->Get_frameNumber(), labels);//开始聚类

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
	gmm->Train(::mfccData, ::charaParameter->Get_frameNumber());             //GMM训练数据

	//Todo save GMM to file
	ofstream gmm_file(gmmfilePath.data());
	assert(gmm_file);
	gmm_file << *gmm;
	gmm_file.close();

	delete gmm;

	return true;
}

int voiceprintRecognition(string rootPath, vector<FILESTRUCT> voiceLib)      //声纹识别
{
	char filePath[MAX_PATH];
	GMM **gmmLib = new GMM*[voiceLib.size()];
	ifstream *gmm_file = new ifstream[voiceLib.size()];
	for (int i = 0; i < (int)voiceLib.size(); ++i) {
		strcpy_s(filePath, rootPath.data());
		strcat_s(filePath, voiceLib[i].fileName.data());
		gmm_file[i].open(filePath);
		assert(gmm_file[i]);
		gmmLib[i] = new GMM(CharaParameter::MelDegreeNumber, GMM::SGMNumber);
		gmm_file[i] >> *gmmLib[i];
		gmm_file[i].close();
	}

	//Todo 识别计算
	cout << "TIP : Begin reservation ..." << endl;
	double *libProbability = new double[voiceLib.size()];
	for (int i = 0; i < (int)voiceLib.size(); ++i) {
		libProbability[i] = 0;
		for (unsigned long j = 0; j < charaParameter->Get_frameNumber(); ++j) {                                      //计算当前GMM下，目标特征参数集在GMM模型下的概率密度
			double tempData = gmmLib[i]->GetProbability(charaParameter->Get_frameMelParameter(j));                   //获取GMM的数值
			if (tempData > 0) {                                                                                      //取对数操作
				tempData = log10(tempData);
			}
			libProbability[i] += tempData;
		}
	}

	char tempData[24];
	char resultName[1024] = {}, resultData[1024] = {};
	cout << "TIP : Probability data is ";
	for (int i = 0; i < (int)voiceLib.size(); ++i) {
		cout << libProbability[i] << "\t";
		if (i != voiceLib.size() - 1) {
			strcat_s(resultName, voiceLib[i].peopleName.data());
			strcat_s(resultName, "|");
			sprintf(tempData, "%lf", libProbability[i]);
			strcat_s(resultData, tempData);
			strcat_s(resultData, "|");
		}
		else {
			strcat_s(resultName, voiceLib[i].peopleName.data());
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

	int countMax = 0;
	for (int i = 1; i < (int)voiceLib.size(); ++i) {
		if (libProbability[i] > libProbability[countMax]) {
			countMax = i;
		}
	}
	return countMax;
}