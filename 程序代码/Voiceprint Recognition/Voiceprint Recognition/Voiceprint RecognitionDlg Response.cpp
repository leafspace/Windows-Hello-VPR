#include "stdafx.h"
#include "Voiceprint RecognitionDlg.h"

WaveRecorder waveRecorder;
char* fileName;

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

void* record(void* args)                                                     //录音线程
{
	waveRecorder.set_FileName((char*)fileName);
	waveRecorder.Start();
	delete fileName;
	return NULL;
}

void getFiles(string path, vector<string>& files)                            //获取path文件夹下的所有文件名
{
    long hFile = 0;
    struct _finddata_t fileinfo;
    string p;
    if((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
		do {
            if((fileinfo.attrib & _A_SUBDIR)) {
				if(strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);  
				}
            } else {
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
        } while(_findnext(hFile, &fileinfo) == 0);  
		_findclose(hFile);
	}
}