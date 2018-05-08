#include "PretreatmentControlCommon.h"

const int PretreatmentControlCommon::N = 256;                                         // 初始化每个窗的窗长
const int PretreatmentControlCommon::FrameShift = 125;                                // 初始化窗函数的帧移
const double PretreatmentControlCommon::PI = 3.14159;                                 // 初始化圆周率
const double PretreatmentControlCommon::preCoefficient = -0.98;                       // 预加重系数

const unsigned long PretreatmentControlCommon::minSilence = 6;                        // 最短静音长度
const unsigned long PretreatmentControlCommon::minVoiceLength = 15;                   // 最短语音长度


PretreatmentControlCommon::PretreatmentControlCommon(MediaFile *mediaFile)
{
	this->mediaFile = NULL;
	this->mediaFile = mediaFile;

	this->energyHigh = 0;
	this->energyLow = 0;
	this->zcrHigh = 0;
	this->zcrLow = 0;

	this->mediaDataList = NULL;
	this->mediaDataList = new double[this->mediaFile->getDataNumber()];
	for (unsigned int i = 0; i < this->mediaFile->getDataNumber(); ++i) {
		this->mediaDataList[i] = this->mediaFile->getData(i) / pow(2, (double)this->mediaFile->getSampleBytes() * 8);         // 控制数据在[0,1]之间
	}

	this->maxEnergy = 0;
	this->minEnergy = 0;
	this->maxZCR = 0;
	this->minZCR = 0;

	this->voiceNumber = 0;
}

PretreatmentControlCommon::~PretreatmentControlCommon()
{
	delete this->mediaDataList;
	this->mediaFile = NULL;
	this->mediaDataList = NULL;
}

double PretreatmentControlCommon::Hamming_window(const double data)                   // 汉明窗函数
{
	if (data >= 0 && data <= (PretreatmentControlCommon::N - 1)) {                    // 0 <= n <= N-1的情况
		return 0.54 - 0.46*cos(2 * PretreatmentControlCommon::PI*data / (N - 1));     // 返回数值
	}
	else {
		return 0;
	}
}

short PretreatmentControlCommon::Sign_Function(const double data)                     // 求短时过零率的辅助符号函数
{
	if (data <= 0.5) {
		return 1;
	}
	else {
		return 0;
	}
}

bool PretreatmentControlCommon::Frame_Energy(void)                                    // 用于求短时帧能量
{
	this->maxEnergy = 0;                                                              // 最大短时帧能量置0
	this->minEnergy = DBL_MAX;                                                        // 最小短时帧能量置1000000
	double sum = 0;
	for (unsigned int i = 0; i < this->mediaFile->getDataNumber() - PretreatmentControlCommon::N;
		i += PretreatmentControlCommon::FrameShift) {                                 // 这是所有短时帧能量数据的个数
		for (unsigned int j = i; j < i + PretreatmentControlCommon::N; ++j) {         // 遍历窗中的每一个数据
			sum += pow(this->mediaDataList[j] * Hamming_window(i + PretreatmentControlCommon::N - 1 - j), 2);  // 求每一个数据的能量
		}
		if (sum > this->maxEnergy) {                                                  // 求取最大短时帧能量
			this->maxEnergy = sum;
		}
		if (sum < this->minEnergy) {                                                  // 求取最大短时帧能量
			this->minEnergy = sum;
		}
		this->dataEnergy.push_back(sum);                                              // 将此帧的短时帧能量保存
		sum = 0;
	}

	return true;
}

bool PretreatmentControlCommon::Frame_ZCR(void)                                       // 用于求短时过零率
{
	this->maxZCR = 0;                                                                 // 最大短时过零率置0
	this->minZCR = DBL_MIN;                                                           // 最小短时过零率置1000000
	double sum = 0;
	for (unsigned int i = 0; i < this->mediaFile->getDataNumber() - PretreatmentControlCommon::N;
		i += PretreatmentControlCommon::FrameShift) {                                 // 这是所有短时帧过零率数据的个数
		for (unsigned int j = i; j < i + PretreatmentControlCommon::N; ++j) {         // 遍历窗中的每一个数据
			sum += abs(Sign_Function(this->mediaDataList[j]) - Sign_Function(this->mediaDataList[max(0, (int)j - 1)]))
				*Hamming_window(i + PretreatmentControlCommon::N - 1 - j);            // 过零率中的绝对值部分
		}
		sum /= 2;
		if (sum > this->maxZCR) {                                                     // 求取最大短时过零率
			this->maxZCR = sum;
		}
		if (sum < this->minZCR) {                                                     // 求取最大短时过零率
			this->minZCR = sum;
		}
		this->dataZCR.push_back(sum);
		sum = 0;
	}
	return true;
}

bool PretreatmentControlCommon::Frame_EnergyZcr(void)                                 // 用于同时求取短时帧能量与短时过零率
{
	return this->Frame_Energy() && this->Frame_ZCR();
}

void PretreatmentControlCommon::showData(void)
{
	cout << "--------------------Pretreatment Control Data--------------------" << endl;
	cout << "Energy data :" << endl;
	for (unsigned int i = 0; i < this->dataEnergy.size(); ++i) {
		cout << this->dataEnergy[i] << "\t";
		if ((i + 1) % 10 == 0) {
			cout << endl;
		}
	}
	cout << endl;

	cout << "ZCR data :" << endl;
	for (unsigned int i = 0; i < this->dataZCR.size(); ++i) {
		cout << this->dataZCR[i] << "\t";
		if ((i + 1) % 10 == 0) {
			cout << endl;
		}
	}
	cout << endl;

	cout << "Max energy :" << this->maxEnergy << endl;
	cout << "Min energy :" << this->minEnergy << endl;

	cout << "Max ZCR :" << this->maxZCR << endl;
	cout << "Min ZCR :" << this->minZCR << endl;

	cout << "Voice Paragraph number :" << this->voiceNumber << endl;
	for (unsigned long i = 0; i < this->voiceNumber; ++i) {
		cout << "Begin : " << this->voiceParagraph[i].begin << " " << "End : " << this->voiceParagraph[i].end << endl;
	}
	cout << "--------------------Pretreatment Control Data--------------------" << endl;
}

double* PretreatmentControlCommon::Get_MediaFileData(void)                            // 获取当前处理过的语音数据
{
	return this->mediaDataList;
}

vector<double> PretreatmentControlCommon::Get_DataEnergy(void)                        // 获取短时帧能量的数据
{
	return this->dataEnergy;
}

vector<double> PretreatmentControlCommon::Get_DataZCR(void)                           // 获取短时过零率的数据
{
	return this->dataZCR;
}

double PretreatmentControlCommon::Get_maxEnergy(void)                                 // 获取最大短时帧能量
{
	return this->maxEnergy;
}

double PretreatmentControlCommon::Get_minEnergy(void)                                 // 获取最小短时帧能量
{
	return this->minEnergy;
}

double PretreatmentControlCommon::Get_maxZCR(void)                                    // 获取最大短时过零率
{
	return this->maxZCR;
}

double PretreatmentControlCommon::Get_minZCR(void)                                    // 获取最小短时过零率
{
	return this->minZCR;
}

double PretreatmentControlCommon::Get_dataEZNumber(void)                              // 获取能量过零率的个数
{
	return min(this->dataEnergy.size(), this->dataZCR.size());
}

double PretreatmentControlCommon::Get_DataEnergy(const unsigned long Number)          // 依据序号找到对应的短时帧能量
{
	if (Number >= this->dataEnergy.size() || Number < 0) {                            // 如果所需要的数超过了数据个数
		return -1;
	}
	else {
		return this->dataEnergy[Number];
	}
}

double PretreatmentControlCommon::Get_DataZCR(const unsigned long Number)             // 依据序号找到对应的短时过零率
{
	if (Number >= this->dataZCR.size() || Number < 0) {                               // 如果所需要的数超过了数据个数
		return -1;
	}
	else {
		return this->dataZCR[Number];
	}
}

int PretreatmentControlCommon::Get_WindowLength(void)                                 // 获取帧长（窗的大小）
{
	return PretreatmentControlCommon::N;
}

unsigned long PretreatmentControlCommon::Get_voiceNumber(void)                        // 获取语音段落个数
{
	return this->voiceNumber;
}

unsigned long PretreatmentControlCommon::Get_frameNumber()                            // 获取端点检测后每个段落的帧数和
{
	unsigned long sumNumber = 0;
	for (unsigned long i = 0; i < this->voiceParagraph.size(); ++i) {
		sumNumber += this->Get_frameNumber(this->voiceParagraph[i]);
	}
	return sumNumber;
}

unsigned long PretreatmentControlCommon::Get_frameNumber(const double dataSize)       // 计算长度内的帧数
{
	unsigned long frameNumber = (unsigned long)((dataSize - PretreatmentControlCommon::N) / PretreatmentControlCommon::FrameShift);     //计算这段数据内有多少帧
	unsigned long end = (frameNumber - 1) * PretreatmentControlCommon::FrameShift + PretreatmentControlCommon::N;   // 求出当前计算帧数中所包含的数据量
	if (end < this->mediaFile->getDataNumber()) {                                     // 如果没有包含所有的数据，则帧数+1
		frameNumber++;
	}
	return frameNumber;
}

unsigned long PretreatmentControlCommon::Get_frameNumber(const VoiceParagraph voiceParagraph)                       //获取某个语音段落的帧数
{
	return this->Get_frameNumber(voiceParagraph.voiceLength);
}

VoiceParagraph PretreatmentControlCommon::Get_dataVoicePoint(const unsigned long Number)                            // 获取某个语音段落
{
	if (Number >= this->voiceNumber || Number < 0) {                                  // 如果所需要的数超过了数据个数
		return VoiceParagraph(-1, -1, -1);
	}
	else {
		return this->voiceParagraph[Number];
	}
}

void PretreatmentControlCommon::Pre_emphasis(VoiceParagraph voiceParagraph, const double *dataList)                 // 对一个段落内的数据进行预加重处理 
{
	for (unsigned int i = 0; i < voiceParagraph.voiceLength; ++i) {
		unsigned int dataIndex = voiceParagraph.begin + i;
		if (dataIndex == 0 || dataIndex == this->mediaFile->getDataNumber()) {
			continue;
		}

		double tempValue = PretreatmentControlCommon::preCoefficient * dataList[dataIndex - 1];
		tempValue = dataList[dataIndex] - tempValue;
		this->mediaDataList[dataIndex] = tempValue;                                   // 加一阶数字滤波器
	}
}

bool PretreatmentControlCommon::Frame_Data(double *data, unsigned long index, double* dataSpace, const int dataSpaceSize)               // 获取端点检测后第index帧的分帧加窗操作
{
	if (dataSpaceSize < PretreatmentControlCommon::N) {                               // 预分配的空间不足一帧时
		return false;
	}

	VoiceParagraph voiceParagraph(-1, -1, -1);
	for (unsigned long i = 0; i < this->voiceParagraph.size(); ++i) {
		if ((int)(index - this->Get_frameNumber(this->voiceParagraph[i])) <= 0) {     // 如果减去此段落的帧数，数据小于0，则此帧为当前数据段
			voiceParagraph = this->voiceParagraph[i];
			break;
		}
		index = index - this->Get_frameNumber(this->voiceParagraph[i]);
	}

	if (voiceParagraph.begin == -1 || voiceParagraph.end == -1 || voiceParagraph.voiceLength == -1) {
		return false;
	}

	unsigned long begin = voiceParagraph.begin + (index - 1) * PretreatmentControlCommon::FrameShift;
	unsigned long end = begin + PretreatmentControlCommon::N - 1;
	unsigned long voiceLength = PretreatmentControlCommon::N;

	if (end >= voiceParagraph.end) {
		end = voiceParagraph.end;
		voiceLength = end - begin + 1;
	}

	voiceParagraph.begin = begin;
	voiceParagraph.end = end;
	voiceParagraph.voiceLength = voiceLength;

	for (unsigned long i = voiceParagraph.begin; i <= voiceParagraph.end; ++i) {
		dataSpace[i - voiceParagraph.begin] = data[i] * this->Hamming_window(i - voiceParagraph.begin);             // 加窗功能
	}

	return true;
}

bool PretreatmentControlCommon::Frame_Data(double *data, const double dataSize, const unsigned long index, double* dataSpace, const int dataSpaceSize)      // 对部分数据进行分帧加窗操作
{
	if (dataSpaceSize < PretreatmentControlCommon::N) {                               // 预分配的空间不足一帧时
		return false;
	}

	unsigned long frameNumber = this->Get_frameNumber(dataSize);
	if (index < 1 || index > frameNumber) {                                           // 帧位不属于数据段允许范围内
		return false;
	}

	unsigned long begin = (index - 1) * PretreatmentControlCommon::FrameShift;
	unsigned long end = begin + PretreatmentControlCommon::N - 1;
	unsigned long voiceLength = PretreatmentControlCommon::N;

	if (index == frameNumber && end != dataSize) {                                    // 如果长度不为整帧
		if (end < dataSize) {                                                         // 全部数据多余
		}
		else if (end > dataSize) {                                                    // 全部数据缺少
			end = (unsigned long)(dataSize - 1);
			voiceLength = (unsigned long)(dataSize - begin);
		}
	}
	VoiceParagraph voiceParagraph(begin, end, voiceLength);

	for (unsigned long i = voiceParagraph.begin; i <= voiceParagraph.end; ++i) {
		dataSpace[i - voiceParagraph.begin] = data[i] * this->Hamming_window(i - voiceParagraph.begin);             // 加窗功能
	}
	return true;
}

bool PretreatmentControlCommon::Endpoint_Detection(void)                              // 端点检测函数
{
	this->Frame_EnergyZcr();                                                          // 计算短时帧能量与短时过零率

	this->energyHigh = 10;                                                            // 初始化短时帧能量高门限
	this->energyLow = 2;                                                              // 初始化短时帧能量低门限
	this->zcrHigh = 10;                                                               // 初始化短时过零率高门限
	this->zcrLow = 5;                                                                 // 初始化短时过零率低门限

	this->energyHigh = min(this->energyHigh, this->Get_maxEnergy() / 4);              // 调整短时帧能量高门限
	this->energyLow = min(this->energyLow, this->Get_maxEnergy() / 8);                // 调整短时帧能量低门限
	this->zcrHigh = min(this->zcrHigh, this->Get_maxZCR() / 4);                       // 调整短时帧能量高门限
	this->zcrLow = min(this->zcrLow, this->Get_maxZCR() / 8);                         // 调整短时帧能量低门限

	int statusFlag = 0;                                                               // 设置语音状态标志
	unsigned long begin = 0;                                                          // 语音段落的起点
	unsigned long end = 0;                                                            // 语音段落的终点
	unsigned long voiceLength = 0;                                                    // 语音段落的长度
	unsigned long silence = 0;                                                        // 静音段落的长度
	voiceNumber = 0;

	for (unsigned long i = 0, frame = 0; i < this->mediaFile->getDataNumber() - N; ++i) {                           // 遍历每一帧
		frame = (i - N) / PretreatmentControlCommon::FrameShift + 1;
		if (i <= 256){
			frame = 0;
		}
		switch (statusFlag)
		{
		case MUTEPARAGRAPH:
		case INTERIMPARAGRAPH:
			if (this->Get_DataEnergy(frame) > this->energyHigh) {                     // 帧能量大于能量高门限,进入语音段
				begin = (unsigned long)max((int)(i - voiceLength - 1), 0);
				statusFlag = VOICEPARAGRAPH;
				voiceLength++;
				silence = 0;
			}
			else if (this->Get_DataEnergy(frame) > this->energyLow || this->Get_DataZCR(frame) > this->zcrLow) {    // 过渡段
				statusFlag = INTERIMPARAGRAPH;
				voiceLength++;
			}
			else {                                                                    // 静音段
				statusFlag = MUTEPARAGRAPH;
				voiceLength = 0;
			}
			break;
		case VOICEPARAGRAPH:
			if (this->Get_DataEnergy(frame) > this->Get_minEnergy() || this->Get_DataZCR(frame) > this->Get_minZCR()) {                 // 保持在语音段
				voiceLength++;
			}
			else {                                                                    // 语音将结束
				silence++;
				if (silence < minSilence) {                                           // 静音还不够长，尚未结束
					voiceLength++;
				}
				else {
					if (voiceLength < minVoiceLength) {                               // 语音段长度太短，认为是噪声
						statusFlag = MUTEPARAGRAPH;
						silence = 0;
						voiceLength = 0;
					}
					else {                                                            // 语音结束
						statusFlag = OVERPARAGRAPH;
						end = begin + voiceLength;
					}
				}
			}
			break;
		case OVERPARAGRAPH:
			this->voiceParagraph.push_back(VoiceParagraph(begin, end, voiceLength));  // 保存语音段落信息
			voiceLength = 0;
			voiceNumber++;                                                            // 语音段落+1
			statusFlag = MUTEPARAGRAPH;
			break;
		default:
			return false;
			break;
		}
	}

	if (statusFlag == VOICEPARAGRAPH) {                                               // 说明语音信号还没有结束，以当前记录下的最后一个点为终点保存语音段
		end = begin + voiceLength;
		this->voiceParagraph.push_back(VoiceParagraph(begin, end, voiceLength));
		++voiceNumber;
	}

	if (voiceNumber == 0 && this->voiceParagraph.size() == 0) {                       // 说明没有检测到语音段落，直接将整段语义合成为一个语音段落
		end = 0 + voiceLength;
		this->voiceParagraph.push_back(VoiceParagraph(0, end, voiceLength));
		++voiceNumber;
	}
	return true;
}