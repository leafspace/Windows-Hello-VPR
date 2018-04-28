#include "../stdafx.h"
#include "WavFile_Struct.h"

bool WavFile::Conversion_Tuple(void)
{
	try
	{
		cout << "TIP : Change to real format data ..." << endl;
		dataNumber = dataSize / chunkAlign;                                  //求出数据个数
		dataTuple = (long*)malloc(sizeof(long)*dataNumber);                  //为新字节数据空间赋值
		long tempNum = 0;                                                    //用来保存每个新字节数据
		for (unsigned long i = 0; i < dataNumber; ++i) {                     //总共有dataNumber个数据
			int Flag = 0;
			for (short j = 0; j < chunkAlign; ++j) {                         //每个数据有两个字节
				tempNum = MakeWord((long)data[i*chunkAlign + j], tempNum);   //每次将tempNum做为高字节数据，新字节为低字节数据制作新数据				
			}
			dataTuple[i] = tempNum;                                          //保存下新字节数据
			tempNum = 0;                                                     //初始化缓冲区操作

			if (dataMax < dataTuple[i]) {                                    //求出最大值
				dataMax = dataTuple[i];
			}
			if (dataMin > dataTuple[i]) {                                    //求出最小值
				dataMin = dataTuple[i];
			}
		}
	}
	catch (invalid_argument &e) {
		cerr << e.what() << endl;
		MessageBoxA(NULL, e.what(), "ERROR", MB_ICONHAND);
		return false;
	}
	return true;
}

long WavFile::MakeWord(long NumberA, long NumberB)                           //合并字节，NumberA表示高字节位，NumberB表示低字节位
{
	int Flag = 0;                                                            //标志，用来表示最后的数的正负，1为负，0为正
	if (NumberA >= 0) {
		if (NumberB >= 0) {
			return NumberA * 256 + NumberB;                                  //高低字节都是正数，则直接移位合并
		}
		else {
			return NumberA * 256 + abs(NumberB) + 128;                       //高字节为正数，低字节为负数
		}
	}
	else {
		Flag = 1;
		if (NumberB > 0) {                                                   //高字节为负数，低字节为正数
			return -1 * (abs(NumberA) * 256 + NumberB);
		}
		else {                                                               //高低字节都是负数
			return -1 * (abs(NumberA) * 256 + abs(NumberB) + 128);
		}
	}
}

bool WavFile::Read_File(void)
{
	try
	{
		cout << "TIP : Reading file ..." << endl;
		fread(id_RIFF, sizeof(char), 4, fp);                                 //读取'RIFF'
		id_RIFF[4] = 0;                                                      //末尾添零
		if (strcmp(id_RIFF, "RIFF")) {
			throw invalid_argument("ERROR : File not RIFF file !");
		}
		fread(&fileSize, sizeof(unsigned long), 1, fp);                      //读取文件的大小
		fread(id_WAVE, sizeof(char), 4, fp);                                 //读取'WAVE'
		id_WAVE[4] = 0;
		if (strcmp(id_WAVE, "WAVE")) {
			throw invalid_argument("ERROR : File not WAVE file !");
		}
		fread(id_FMT, sizeof(char), 4, fp);                                  //读取'FMT'
		id_FMT[3] = 32;
		id_FMT[4] = 0;
		fread(&formatLength, sizeof(unsigned long), 1, fp);                  //
		fread(&formatTag, sizeof(short), 1, fp);                             //读取文件标签
		fread(&channelsNumber, sizeof(short), 1, fp);                        //读取通道数目
		fread(&sampleRate, sizeof(unsigned long), 1, fp);                    //读取采样频率
		fread(&secondBytes, sizeof(unsigned long), 1, fp);                   //读取每秒数据量
		fread(&chunkAlign, sizeof(short), 1, fp);                            //读取块对其
		fread(&sampleBits, sizeof(short), 1, fp);                            //读取样本大小
		if (formatLength > 16) {
			fread(&appendInformation, sizeof(short), 1, fp);                 //读取附加信息
		}
		fread(id_DATA, sizeof(char), 4, fp);                                 //读取'DATA'
		id_DATA[4] = 0;
		fread(&dataSize, sizeof(unsigned long), 1, fp);                      //读取数据大小
		data = (char*)malloc(sizeof(char)*dataSize);                         //申请数据的存储空间
		fread(data, sizeof(char), dataSize, fp);                             //读取数据
		fclose(fp);
	}
	catch (invalid_argument &e)
	{
		MessageBoxA(NULL, e.what(), "ERROR", MB_ICONHAND);
		return false;
	}
	if (Conversion_Tuple() == true) {                                        //转换格式成功
		MessageBoxA(NULL, "TIP : File read ok !", "TIP", MB_ICONASTERISK);
	}
	else {
		MessageBoxA(NULL, "ERROR : Data change failure !", "ERROR", 0);
		return false;
	}

	return true;
}

unsigned long WavFile::Get_SampleRate(void)                                  //获取采样频率
{
	return sampleRate;
}

short WavFile::Get_ChunkAlign(void)                                          //获取样本字节数
{
	return chunkAlign;
}

long WavFile::Get_Data(unsigned long Number)                                 //获取某个位置上的数据
{
	if (Number >= dataNumber) {                                              //如果所需要的数超过了数据个数
		MessageBoxA(NULL, "ERROR : Over list !", "ERROR", MB_ICONHAND);
		return -1;
	}
	else {
		return dataTuple[Number];
	}
}

unsigned long WavFile::Get_dataNumber(void)                                  //获取样本数据个数
{
	return dataNumber;
}

void WavFile::ShowData(void)
{
	cout << "TIP : Show data ..." << endl;
	cout << id_RIFF << endl;
	cout << fileSize << endl;
	cout << id_WAVE << endl << endl;

	cout << id_FMT << endl;
	cout << formatLength << endl;
	cout << formatTag << endl;
	cout << channelsNumber << endl;
	cout << sampleRate << endl;
	cout << secondBytes << endl;

	cout << chunkAlign << endl;
	cout << sampleBits << endl;
	cout << appendInformation << endl << endl;

	cout << id_FACT << endl;
	cout << appendLength << endl;
	cout << appendNone << endl << endl;

	cout << id_DATA << endl;
	cout << dataSize << endl;

	for (unsigned int i = 0; i < dataNumber; ++i) {
		cout << dataTuple[i] << "\t";
	}
	cout << endl;

	cout << "MAX :" << dataMax << endl;
	cout << "MIN :" << dataMin << endl;
}

void WavFile::SaveNewWav(unsigned long voiceNumber, vector<VoiceParagraph> voiceParagraph)       //保存去掉空白处的语音文件，参数1为段落的个数，参数2为各个语音段落的开始点与结束点信息
{
	unsigned long endPointLength = 0;
	for (unsigned long i = 0; i < voiceNumber; ++i) {
		VoiceParagraph temp = voiceParagraph[i];
		endPointLength += temp.voiceLength;
	}
	unsigned long removerLength = dataNumber - endPointLength;
	unsigned long removerSize = removerLength*chunkAlign;

	//更改数据操作较多，暂时无法完成
	//Wav_File* newWav = (Wav_File*)malloc(sizeof(Wav_File));
	//newWav = this;

	FILE *fp;
	if ((fp = fopen("new file.wav", "wb")) == NULL) {
		cout << "ERROR : File open failed !" << endl;
		exit(-1);
	}

	try
	{
		unsigned long tempLength;
		cout << "TIP : Writing file ..." << endl;
		fwrite(id_RIFF, sizeof(char), 4, fp);                                //写入'RIFF'
		tempLength = fileSize - removerSize;
		fwrite(&tempLength, sizeof(unsigned long), 1, fp);                   //写入文件的大小
		fwrite(id_WAVE, sizeof(char), 4, fp);                                //写入'WAVE'
		fwrite(id_FMT, sizeof(char), 4, fp);                                 //写入'FMT'
		fwrite(&formatLength, sizeof(unsigned long), 1, fp);                 //写入格式块长度
		fwrite(&formatTag, sizeof(short), 1, fp);                            //写入文件标签
		fwrite(&channelsNumber, sizeof(short), 1, fp);                       //写入通道数目
		fwrite(&sampleRate, sizeof(unsigned long), 1, fp);                   //写入采样频率
		fwrite(&secondBytes, sizeof(unsigned long), 1, fp);                  //写入每秒数据量
		fwrite(&chunkAlign, sizeof(short), 1, fp);                           //写入块对其
		fwrite(&sampleBits, sizeof(short), 1, fp);                           //写入样本大小
		if (formatLength > 16) {
			fwrite(&appendInformation, sizeof(short), 1, fp);                //写入附加信息
		}
		fwrite(id_DATA, sizeof(char), 4, fp);                                //写入'DATA'
		tempLength = dataSize - removerSize;
		fwrite(&tempLength, sizeof(unsigned long), 1, fp);                   //写入数据大小

		unsigned long n = 0;
		for (unsigned long i = 0; i < voiceNumber; ++i) {
			VoiceParagraph tempParagraph = voiceParagraph[i];
			for (unsigned long j = tempParagraph.begin; j < tempParagraph.end; ++j, n += 2) {
				fwrite(&data[j * 2], sizeof(char), 1, fp);
				fwrite(&data[j * 2 + 1], sizeof(char), 1, fp);
			}
		}
		fclose(fp);
	}
	catch (invalid_argument &e)
	{
		MessageBoxA(NULL, e.what(), "ERROR", MB_ICONHAND);
	}
}