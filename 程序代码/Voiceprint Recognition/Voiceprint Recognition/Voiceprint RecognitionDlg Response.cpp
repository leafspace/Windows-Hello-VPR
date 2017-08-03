#include "stdafx.h"
#include "VPR/WavData_CharaParameter.h"
#include "VPR/Model_KMeans.h"
#include "VPR/Model_GMM.h"
#include "Voiceprint RecognitionDlg.h"

WaveRecorder waveRecorder;
char* fileName;
string fileName_t;

void CVoiceprintRecognitionDlg::CompoundFile(vector<FILESTRUCT>& fileLib, int flag)    //用于将txt信息与当前文件夹下内容相结合
{
	char szModuleFilePath[MAX_PATH];
	int n = GetModuleFileNameA(0, szModuleFilePath, MAX_PATH);               //获得当前执行文件的路径
	szModuleFilePath[strrchr(szModuleFilePath, '\\') - szModuleFilePath + 1] = 0;      //将最后一个"\\"后的字符置为0
	
	int index = 0;
	char filePath[MAX_PATH];
	for (int i = 0; i < (int) strlen(szModuleFilePath); ++i) {               //补全//
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
	} else if (flag == 1) {
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
	for (int i = 0, j = 0; i < (int) files.size(); ++i) {
		string fileName = files[i];
		if (strcmp(getFileName(fileName).data(), "info.list") == 0) {        //如果当前文件的文件名为info.list说明其为配置文件
			continue;
		}
		for (j = 0; j < (int) fileLib.size(); ++j) {
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
					} else {
						item.peopleName = tempstr;
					}
				} else {
					item.peopleName = "未知";
				}
			} else if (flag == 1) {
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
	case 0 : count = this->listCtrl_1.GetItemCount(); break;
	case 1 : count = this->listCtrl_2.GetItemCount(); break;
	default : break;
	}

	for (int i = 0; i < count; ++i) {
		switch (index)
		{
		case 0 : 
			if (this->listCtrl_1.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED) {
				return i;
			}
			break;
		case 1 : 
			if (this->listCtrl_2.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED) {
				return i;
			}
			break;
		default : break;
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
    if(ret != 0) {
		MessageBoxA(NULL, "ERROR : Can't create thread !", "ERROR", MB_ICONHAND);
		return false;
	}
	return true;
}

bool CVoiceprintRecognitionDlg::OnButton1_cancel()                           //结束录音
{
	waveRecorder.Stop();
	waveRecorder.Reset();
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
	waveRecorder.set_FileName((char*)fileName);
	waveRecorder.Start();
	delete fileName;
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
    if((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
		do {
            if((fileinfo.attrib & _A_SUBDIR)) {                              //判断是否为文件夹
				if(strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);  
				}
            } else {
				files.push_back(p.assign(path).append("\\\\").append(fileinfo.name));
			}
        } while(_findnext(hFile, &fileinfo) == 0);  
		_findclose(hFile);
	}
}

void readList(ifstream& in, vector<FILESTRUCT>& list)                        //读取文件的内容到list中
{
	char buffer[256];
	string str_f, str_p;
	char fileName[256], peopleName[256];
	if (in.is_open()) {
		while (!in.eof()) {
			in.getline(buffer, 256);
			if (strlen(buffer) == 0) {                                       //防止到了最后一行只是一个换行还重复读取
				continue;
			}
			sscanf(buffer, "%s %s", &fileName, &peopleName);                 //格式化字符串
			CChineseCode::UTF_8ToGB2312(str_f, fileName, strlen(fileName));
			CChineseCode::UTF_8ToGB2312(str_p, peopleName, strlen(peopleName));
			FILESTRUCT item(str_f, str_p);
			list.push_back(item);
			memset(buffer, 0, 256);
		}
	}
}

void writeList(ofstream& out, vector<FILESTRUCT>& list)                      //将list文件内容写入数据流
{
	string str_f, str_p;
	if (out.is_open()) {
		for (int i = 0; i < (int) list.size(); ++i) {
			CChineseCode::GB2312ToUTF_8(str_f, (char*) list[i].fileName.data(), list[i].fileName.length());
			CChineseCode::GB2312ToUTF_8(str_p, (char*) list[i].peopleName.data(), list[i].peopleName.length());
			out << str_f.data() << "\t" << str_p.data() << endl;
		}
	}
}

bool trainingWAV(string wavfilePath, string gmmfilePath, string peopleName)  //训练wav文件
{


	return true;
}