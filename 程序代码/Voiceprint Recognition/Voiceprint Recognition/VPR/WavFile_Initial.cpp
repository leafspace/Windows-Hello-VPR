#include "../stdafx.h"
#include "WavFile_Initial.h"

const int WavFile_Initial::N = 256;                                          //初始化每个窗的窗长
const int WavFile_Initial::FrameShift = 125;                                 //初始化窗函数的帧移
const double WavFile_Initial::PI = 3.14159;                                  //初始化圆周率
const double WavFile_Initial::preCoefficient = -0.98;                        //预加重系数

const unsigned long WavFile_Initial::minSilence = 6;                         //最短静音长度
const unsigned long WavFile_Initial::minVoiceLength = 15;                    //最短语音长度

bool WavFile_Initial::Conversion_Double(void)                                //用来将新字节数据转换为Double数据
{
	try
	{
		cout << "TIP : Change to double data ..." << endl;
		const double Flag = pow((double)2, WavFile::Get_ChunkAlign() * 8);   //表示的是原数据最大值的一半
		for (unsigned long i = 0; i < WavFile::Get_dataNumber(); ++i) {      //遍历每个数据
			dataDouble[i] = (double)WavFile::Get_Data(i) / Flag;             //控制每个数据在[-1,1]之间
		}
	}
	catch (invalid_argument &e) {
		cerr << e.what() << endl;
		MessageBoxA(NULL, e.what(), "ERROR", MB_ICONHAND);
		return false;
	}
	return true;
}

double WavFile_Initial::Hamming_window(double data)                          //汉明窗函数
{
	if (data >= 0 && data <= (N - 1)) {                                      //0<= n <= N-1的情况
		return 0.54 - 0.46*cos(2 * PI*data / (N - 1));                       //返回数值
	}
	else {
		return 0;
	}
}

short WavFile_Initial::Sign_Function(double data)                            //求短时过零率的辅助符号函数
{
	if (data >= 0) {
		return 1;
	}
	else {
		return 0;
	}
}

bool WavFile_Initial::Frame_Energy(void)                                     //用于求短时帧能量
{
	cout << "TIP : Calculate Energy ..." << endl;
	maxEnergy = 0;                                                           //最大短时帧能量置0
	minEnergy = 1000000;                                                     //最小短时帧能量置1000000
	double sum = 0;
	for (unsigned long i = 0; i < this->Get_dataNumber() - N; i += WavFile_Initial::FrameShift) { //这是所有短时帧能量数据的个数
		for (unsigned long j = i; j < i + N; ++j) {                          //遍历窗中的每一个数据
			sum += pow(dataDouble[j] * Hamming_window(i + N - 1 - j), 2);    //求每一个数据的能量
		}
		if (sum > maxEnergy) {                                               //求取最大短时帧能量
			maxEnergy = sum;
		}
		if (sum < minEnergy) {                                               //求取最大短时帧能量
			minEnergy = sum;
		}
		dataEnergy.push_back(sum);                                           //将此帧的短时帧能量保存
		sum = 0;
	}

	return true;
}

bool WavFile_Initial::Frame_ZCR(void)                                        //用于求短时过零率
{
	cout << "TIP : Calculate ZCR ..." << endl;
	maxZCR = 0;                                                              //最大短时过零率置0
	minZCR = 1000000;                                                        //最小短时过零率置1000000
	double sum = 0;
	for (unsigned long i = 0; i < this->Get_dataNumber() - N; i += WavFile_Initial::FrameShift) {//这是所有短时帧过零率数据的个数
		for (unsigned long j = i; j < i + N; ++j) {                          //遍历窗中的每一个数据
			sum += abs(Sign_Function(dataDouble[j]) - Sign_Function(dataDouble[j - 1]))//过零率中的绝对值部分
				*Hamming_window(i + N - 1 - j);
		}
		sum /= 2;
		if (sum > maxZCR) {                                                  //求取最大短时过零率
			maxZCR = sum;
		}
		if (sum < minZCR) {                                                  //求取最大短时过零率
			minZCR = sum;
		}
		dataZCR.push_back(sum);
		sum = 0;
	}
	return true;
}

bool WavFile_Initial::Frame_EnergyZcr(void)                                  //用于同时求取短时帧能量与短时过零率
{
	cout << "TIP : Calculate Engry and ZCR ..." << endl;
	maxEnergy = 0;                                                           //最大短时帧能量置0
	minEnergy = 1000000;                                                     //最小短时帧能量置1000000
	maxZCR = 0;                                                              //最大短时过零率置0
	minZCR = 1000000;                                                        //最小短时过零率置1000000
	double sumEnergy = 0;
	double sumZcr = 0;
	double hanming = 0;
	for (unsigned long i = 0; i < this->Get_dataNumber() - N; i += WavFile_Initial::FrameShift) {//这是所有短时帧能量数据的个数
		for (unsigned long j = i; j < i + N; ++j) {                          //遍历窗中的每一个数据
			hanming = Hamming_window(i + N - 1 - j);
			sumEnergy += pow(dataDouble[j] * hanming, 2);                    //求每一个数据的能量
			sumZcr += abs(Sign_Function(dataDouble[j]) - Sign_Function(dataDouble[j - 1]))       //过零率中的绝对值部分
				*hanming;
		}
		sumZcr /= 2;

		if (sumEnergy > maxEnergy) {                                         //求取最大短时帧能量
			maxEnergy = sumEnergy;
		}
		if (sumEnergy < minEnergy) {                                         //求取最大短时帧能量
			minEnergy = sumEnergy;
		}
		if (sumZcr > maxZCR) {                                               //求取最大短时过零率
			maxZCR = sumZcr;
		}
		if (sumZcr < minZCR) {                                               //求取最大短时过零率
			minZCR = sumZcr;
		}
		dataEnergy.push_back(sumEnergy);                                     //将此帧的短时帧能量保存
		dataZCR.push_back(sumZcr);
		sumEnergy = 0;
		sumZcr = 0;

	}

	return true;
}

double* WavFile_Initial::Get_WavFileData(void)                               //获取合成完毕的语音数据
{
	return this->dataDouble;
}

unsigned long WavFile_Initial::Get_SampleRate(void)                          //获取采样频率
{
	return WavFile::Get_SampleRate();
}

vector<double> WavFile_Initial::Get_DataEnergy(void)                         //获取短时帧能量的数据
{
	return this->dataEnergy;
}

vector<double> WavFile_Initial::Get_DataZCR(void)                            //获取短时过零率的数据
{
	return this->dataZCR;
}

double WavFile_Initial::Get_maxEnergy(void)                                  //获取最大短时帧能量
{
	return maxEnergy;
}
double WavFile_Initial::Get_minEnergy(void)                                  //获取最小短时帧能量
{
	return minEnergy;
}
double WavFile_Initial::Get_maxZCR(void)                                     //获取最大短时过零率
{
	return maxZCR;
}
double WavFile_Initial::Get_minZCR(void)                                     //获取最小短时过零率
{
	return minZCR;
}

double WavFile_Initial::Get_dataNumber(void)                                 //获取Double数据的个数
{
	return WavFile::Get_dataNumber();
}

double WavFile_Initial::Get_dataEZNumber(void)                               //获取能量过零率的个数
{
	return min(dataEnergy.size(), dataZCR.size());
}

double WavFile_Initial::Get_DataDouble(unsigned long Number)                 //获取转换后的Double数据
{
	if (Number >= (this->Get_dataNumber()) || Number < 0) {                  //如果所需要的数超过了数据个数
		MessageBoxA(NULL, "ERROR : Over list !", "ERROR", MB_ICONHAND);
		throw invalid_argument("ERROR : Over list !");
		return -1;
	}
	else {
		return dataDouble[Number];
	}
}

double WavFile_Initial::Get_DataEnergy(unsigned long Number)                 //依据序号找到对应的短时帧能量
{
	if (Number >= dataEnergy.size() || Number < 0) {                         //如果所需要的数超过了数据个数
		MessageBoxA(NULL, "ERROR : Over list !", "ERROR", MB_ICONHAND);
		throw invalid_argument("ERROR : Over list !");
		return -1;
	}
	else {
		return dataEnergy[Number];
	}
}
double WavFile_Initial::Get_DataZCR(unsigned long Number)                    //依据序号找到对应的短时过零率
{
	if (Number >= dataZCR.size() || Number < 0) {                            //如果所需要的数超过了数据个数
		MessageBoxA(NULL, "ERROR : Over list !", "ERROR", MB_ICONHAND);
		throw invalid_argument("ERROR : Over list !");
		return -1;
	}
	else {
		return dataZCR[Number];
	}
}
int WavFile_Initial::Get_WindowLength(void)                                  //获取帧长（窗的大小）
{
	return N;
}

unsigned long WavFile_Initial::Get_voiceNumber(void)                         //获取语音段落个数
{
	return this->voiceNumber;
}

unsigned long WavFile_Initial::Get_frameNumber()                             //获取端点检测后每个段落的帧数和
{
	unsigned long sumNumber = 0;
	for (unsigned long i = 0; i < this->voiceParagraph.size(); ++i) {
		sumNumber += this->Get_frameNumber(this->voiceParagraph[i]);
	}
	return sumNumber;
}

unsigned long WavFile_Initial::Get_frameNumber(double dataSize)              //计算长度内的帧数
{
	unsigned long frameNumber = (unsigned long)((dataSize - WavFile_Initial::N) / WavFile_Initial::FrameShift);       //计算这段数据内有多少帧
	unsigned long end = (frameNumber - 1) * WavFile_Initial::FrameShift + WavFile_Initial::N;    //求出当前计算帧数中所包含的数据量
	if (end < this->Get_dataNumber()) {                                      //如果没有包含所有的数据，则帧数+1
		frameNumber++;
	}
	return frameNumber;
}

unsigned long WavFile_Initial::Get_frameNumber(VoiceParagraph voiceParagraph)//获取某个语音段落的帧数
{
	return this->Get_frameNumber(voiceParagraph.voiceLength);
}

VoiceParagraph WavFile_Initial::Get_dataVoicePoint(unsigned long Number)     //获取某个语音段落
{
	if (Number >= voiceNumber || Number < 0) {                               //如果所需要的数超过了数据个数
		MessageBoxA(NULL, "ERROR : Over list !", "ERROR", MB_ICONHAND);
		throw invalid_argument("ERROR : Over list !");
	}
	else {
		return voiceParagraph[Number];
	}
}

void WavFile_Initial::ShowData(void)                                         //覆盖父类的展示数据函数
{
	int max = 0, min = 0;
	cout << "TIP : Double data " << endl;
	for (unsigned long i = 0; i < this->Get_dataNumber(); ++i) {
		cout << dataDouble[i] << "\t";
		if (dataDouble[i] > dataDouble[max]) {
			max = i;
		}
		if (dataDouble[i] < dataDouble[min]) {
			min = i;
		}
	}
	cout << endl;
	cout << "Max " << dataDouble[max] << endl;
	cout << "Min " << dataDouble[min] << endl;

	cout << "TIP : Parameter " << endl;
	cout << "Max energy " << Get_maxEnergy() << endl;
	cout << "Min energy " << Get_minEnergy() << endl;
	cout << "Max ZCR " << Get_maxZCR() << endl;
	cout << "Min ZCR " << Get_minZCR() << endl;

	/*
	cout << "TIP : Energy " << endl;
	for (auto i : dataEnergy) {
		cout << i << "\t";
	}
	cout << endl;

	cout << "TIP : ZCR " << endl;
	for (auto i : dataZCR) {
		cout << i << "\t";
	}
	cout << endl;
	*/

	for (int i = 0; i < (WavFile_Initial::Get_dataNumber() - N); ++i) {
		cout << Get_DataEnergy(i) << endl;
	}

	for (int i = 0; i < (WavFile_Initial::Get_dataNumber() - N); ++i) {
		cout << Get_DataZCR(i) << endl;
	}

}

void WavFile_Initial::SaveNewWav(void)                                       //保存去掉空白处的语音文件
{
	WavFile::SaveNewWav(voiceNumber, voiceParagraph);                        //调用父类的生成函数
}

void WavFile_Initial::Pre_emphasis(VoiceParagraph voiceParagraph, double *dataDouble)  //对一个段落内的数据进行预加重处理 
{
	for (unsigned long i = 0; i < voiceParagraph.voiceLength; ++i) {
		unsigned long dataIndex = voiceParagraph.begin + i;
		if (dataIndex == 0 || dataIndex == this->Get_dataNumber()) {
			continue;
		}
		dataDouble[dataIndex] = dataDouble[dataIndex] - WavFile_Initial::preCoefficient * dataDouble[dataIndex - 1]; //加一阶数字滤波器
	}
}

bool WavFile_Initial::Frame_Data(double *data, unsigned long index, double* dataSpace, int dataSpaceSize)                      //获取端点检测后第index帧的分帧加窗操作
{
	if (dataSpaceSize < WavFile_Initial::N) {                                //预分配的空间不足一帧时
		return false;
	}

	VoiceParagraph voiceParagraph(-1, -1, -1);
	for (unsigned long i = 0; i < this->voiceParagraph.size(); ++i) {
		if ((int)(index - this->Get_frameNumber(this->voiceParagraph[i])) <= 0) {      //如果减去此段落的帧数，数据小于0，则此帧为当前数据段
			voiceParagraph = this->voiceParagraph[i];
			break;
		}
		index = index - this->Get_frameNumber(this->voiceParagraph[i]);
	}

	if (voiceParagraph.begin == -1 || voiceParagraph.end == -1 || voiceParagraph.voiceLength == -1) {
		return false;
	}

	unsigned long begin = voiceParagraph.begin + (index - 1) * WavFile_Initial::FrameShift;
	unsigned long end = begin + WavFile_Initial::N - 1;
	unsigned long voiceLength = WavFile_Initial::N;

	if (end >= voiceParagraph.end) {
		end = voiceParagraph.end;
		voiceLength = end - begin + 1;
	}

	voiceParagraph.begin = begin;
	voiceParagraph.end = end;
	voiceParagraph.voiceLength = voiceLength;

	for (unsigned long i = voiceParagraph.begin; i <= voiceParagraph.end; ++i) {
		dataSpace[i - voiceParagraph.begin] = data[i] * this->Hamming_window(i - voiceParagraph.begin);    //加窗功能
	}

	return true;
}

bool WavFile_Initial::Frame_Data(double *data, double dataSize, unsigned long index, double* dataSpace, int dataSpaceSize)     //对部分数据进行分帧加窗操作
{
	if (dataSpaceSize < WavFile_Initial::N) {                                //预分配的空间不足一帧时
		return false;
	}

	unsigned long frameNumber = this->Get_frameNumber(dataSize);
	if (index < 1 || index > frameNumber) {                                  //帧位不属于数据段允许范围内
		return false;
	}

	unsigned long begin = (index - 1) * WavFile_Initial::FrameShift;
	unsigned long end = begin + WavFile_Initial::N - 1;
	unsigned long voiceLength = WavFile_Initial::N;

	if (index == frameNumber && end != dataSize) {                           //如果长度不为整帧
		if (end < dataSize) {                                                //全部数据多余
		}
		else if (end > dataSize) {                                         //全部数据缺少
			end = (unsigned long)(dataSize - 1);
			voiceLength = (unsigned long)(dataSize - begin);
		}
	}
	VoiceParagraph voiceParagraph(begin, end, voiceLength);

	for (unsigned long i = voiceParagraph.begin; i <= voiceParagraph.end; ++i) {
		dataSpace[i - voiceParagraph.begin] = data[i] * this->Hamming_window(i - voiceParagraph.begin);    //加窗功能
	}
	return true;
}

bool WavFile_Initial::Endpoint_Detection(void)                               //端点检测函数
{
	//this->Frame_Energy();                                                    //计算短时帧能量
	//this->Frame_ZCR();                                                       //计算短时过零率
	this->Frame_EnergyZcr();                                                 //计算短时帧能量与短时过零率

	energyHigh = 10;                                                         //初始化短时帧能量高门限
	energyLow = 2;                                                           //初始化短时帧能量低门限
	zcrHigh = 10;                                                            //初始化短时过零率高门限
	zcrLow = 5;                                                              //初始化短时过零率低门限

	energyHigh = min(energyHigh, Get_maxEnergy() / 4);                       //调整短时帧能量高门限
	energyLow = min(energyLow, Get_maxEnergy() / 8);                         //调整短时帧能量低门限
	zcrHigh = min(zcrHigh, Get_maxZCR() / 4);                                //调整短时帧能量高门限
	zcrLow = min(zcrLow, Get_maxZCR() / 8);                                  //调整短时帧能量低门限

	int statusFlag = 0;                                                      //设置语音状态标志
	unsigned long begin = 0;                                                 //语音段落的起点
	unsigned long end = 0;                                                   //语音段落的终点
	unsigned long voiceLength = 0;                                           //语音段落的长度
	unsigned long silence = 0;                                               //静音段落的长度
	voiceNumber = 0;

	for (unsigned long i = 0, frame = 0; i < this->Get_dataNumber() - N; ++i) { //遍历每一帧
		frame = (i - N) / WavFile_Initial::FrameShift + 1;
		if (i <= 256) {
			frame = 0;
		}
		switch (statusFlag)
		{
		case MUTEPARAGRAPH:
		case INTERIMPARAGRAPH:
			if (Get_DataEnergy(frame) > energyHigh) {                        //帧能量大于能量高门限,进入语音段
				begin = (unsigned long)max((int)(i - voiceLength - 1), 0);
				statusFlag = VOICEPARAGRAPH;
				voiceLength++;
				silence = 0;
			}
			else if (Get_DataEnergy(frame) > energyLow || Get_DataZCR(frame) > zcrLow) {         //过渡段
				statusFlag = INTERIMPARAGRAPH;
				voiceLength++;
			}
			else {                                                           //静音段
				statusFlag = MUTEPARAGRAPH;
				voiceLength = 0;
			}
			break;
		case VOICEPARAGRAPH:
			if (Get_DataEnergy(frame) > Get_minEnergy() || Get_DataZCR(frame) > Get_minZCR()) {  //保持在语音段
				voiceLength++;
			}
			else {                                                           //语音将结束
				silence++;
				if (silence < minSilence) {                                  //静音还不够长，尚未结束
					voiceLength++;
				}
				else {
					if (voiceLength < minVoiceLength) {                      //语音段长度太短，认为是噪声
						statusFlag = MUTEPARAGRAPH;
						silence = 0;
						voiceLength = 0;
					}
					else {                                                   //语音结束
						statusFlag = OVERPARAGRAPH;
						end = max(begin + voiceLength, 0);
					}
				}
			}
			break;
		case OVERPARAGRAPH:
			voiceParagraph.push_back(VoiceParagraph(begin, end, voiceLength));         //保存语音段落信息
			voiceLength = 0;
			voiceNumber++;                                                   //语音段落+1
			statusFlag = MUTEPARAGRAPH;
			break;
		default:
			MessageBoxA(NULL, "ERROR : Status failure !", "ERROR", MB_ICONHAND);
			return false;
			break;
		}
	}

	if (statusFlag == VOICEPARAGRAPH) {                                      //说明语音信号还没有结束，以当前记录下的最后一个点为终点保存语音段
		end = begin + voiceLength;
		voiceParagraph.push_back(VoiceParagraph(begin, end, voiceLength));
		++voiceNumber;
	}

	if (voiceNumber == 0 && voiceParagraph.size() == 0) {                    //说明没有检测到语音段落，直接将整段语义合成为一个语音段落
		end = 0 + voiceLength;
		voiceParagraph.push_back(VoiceParagraph(0, end, voiceLength));
		++voiceNumber;
	}

	cout << "TIP : Voice number is " << voiceNumber << endl;

	return true;
}