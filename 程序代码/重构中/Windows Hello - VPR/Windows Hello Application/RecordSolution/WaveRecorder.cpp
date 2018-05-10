#include "WaveRecorder.h"

// 静态变量初始化
array <char, CHUNCK_SIZE> WaveRecorder::ChunkData = {};
vector<array<char, CHUNCK_SIZE>> WaveRecorder::RawData;
UINT WaveRecorder::ChunksCount = 0;

WAVEHDR WaveRecorder::pwh[BUFFER_LAYER] = {};
BOOL WaveRecorder::bCallback = false;
CNKDATAUpdateCallback WaveRecorder::callback = NULL;
bool WaveRecorder::stop = false;
bool WaveRecorder::dat_ignore = false;

void WaveRecorder::set_Callback(CNKDATAUpdateCallback fn)
{
	bCallback = true;
	callback = fn;
}

void WaveRecorder::set_FileName(string Target)
{
	// 若启用存档则保存一个wav文件
	bSaveFile = true;
	dest_path = Target;
	// 尝试打开文件【不存在则创建】
	errno_t err = fopen_s(&fp, dest_path.c_str(), "wb");
	if (err > 0) {
#if _DEBUG
		cout << "ERROR : File create failed：" << err << endl;
#endif
		bSaveFile = false;
	}
}

void WaveRecorder::Start()
{
	for (int layer = 0; layer < BUFFER_LAYER; layer++) {
		// 配置缓冲区
		pwh[layer].lpData = new char[CHUNCK_SIZE];
		pwh[layer].dwBufferLength = CHUNCK_SIZE;
		pwh[layer].dwBytesRecorded = 0;
		pwh[layer].dwUser = layer;
		pwh[layer].dwFlags = 0;
		pwh[layer].dwLoops = 0;
		pwh[layer].lpNext = NULL;
		pwh[layer].reserved = 0;
		// 排进缓冲区
		waveInPrepareHeader(hwi, &pwh[layer], sizeof(WAVEHDR));
		waveInAddBuffer(hwi, &pwh[layer], sizeof(WAVEHDR));
	}
	// 初始化裸数据缓存
	RawData.clear();
	RawData.reserve(10);
	// 发送录音开始消息
	waveInStart(hwi);
}

void WaveRecorder::Stop()
{
	// 停止标记
	stop = true;
	// 设备停止
	waveInStop(hwi);
	waveInReset(hwi);
	// 释放缓冲区
	for (int layer = 0; layer < BUFFER_LAYER; layer++) {
		waveInUnprepareHeader(hwi, &pwh[layer], sizeof(WAVEHDR));
		delete pwh[layer].lpData;
	}
	// 保存Header+RawData
	if (bSaveFile) {
		WaveFileWrite();
	}
}

void WaveRecorder::Reset()
{
	// 静态变量初始化
	RawData.clear();
	ChunksCount = 0;
	bSaveFile = false;
	bCallback = false;
	callback = NULL;
	stop = false;
	dat_ignore = false;
}

void WaveRecorder::WaveInitFormat(LPWAVEFORMATEX WaveFormat, WORD Ch, DWORD SampleRate, WORD BitsPerSample)
{
	// 自动配置参数
	WaveFormat->wFormatTag = WAVE_FORMAT_PCM;
	WaveFormat->nChannels = Ch;
	WaveFormat->nSamplesPerSec = SampleRate;
	WaveFormat->nAvgBytesPerSec = SampleRate * Ch * BitsPerSample / 8;
	WaveFormat->nBlockAlign = Ch * BitsPerSample / 8;
	WaveFormat->wBitsPerSample = BitsPerSample;
	WaveFormat->cbSize = 0;
#if _DEBUG
	cout << "   采样参数：" << endl;
	cout << "   声道数" << Ch << endl;
	cout << "   每秒采样率" << SampleRate << "Hz" << endl;
	cout << "   位深" << BitsPerSample << endl;
#endif
}

void WaveRecorder::WaveFileWrite()
{
	// 编辑并写入Wave头信息
	WavHeader.data_size = CHUNCK_SIZE*RawData.size();
	WavHeader.size_8 = WavHeader.data_size + 32;
	fwrite(&WavHeader, sizeof(WavHeader), 1, fp);
	// 追加RawData
	fwrite(RawData.data(), CHUNCK_SIZE*RawData.size(), 1, fp);
	// 写入结束
	fclose(fp);
}

DWORD WaveRecorder::WaveXAPI_Callback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	// 消息switch
	switch (uMsg)
	{
	case WIM_OPEN:		                                                     // 设备成功已打开
		ChunksCount = 0;                                                     // 这次"session"的数据块计数
		break;

	case WIM_DATA:                                                           // 缓冲区数据填充完毕
		// 停止后会频繁发出WIM_DATA,已经将数据转移所以不必理会后继数据【后继数据在这里来看是是重复的】
		if (!dat_ignore) {
			// 把缓冲区数据拷贝出来
			memcpy(ChunkData.data(), ((LPWAVEHDR)dwParam1)->lpData, CHUNCK_SIZE);
			// 没有录进去的被填充为0xcd,改成0来避免末尾出现爆音【只在结束录音时进行，不影响添加缓存效率】
			if (((LPWAVEHDR)dwParam1)->dwBytesRecorded < CHUNCK_SIZE) {
				for (size_t i = ((LPWAVEHDR)dwParam1)->dwBytesRecorded; i < CHUNCK_SIZE; i++) {
					ChunkData.at(i) = 0;
				}
			}
			// 添加这一帧
			RawData.push_back(ChunkData);
			// 如果你设置了回调函数
			if (bCallback) {
				callback(ChunkData, ChunksCount, stop);
			}
		}
		ChunksCount += 1;
		// 如果需要停止录音则不继续添加缓存
		if (!stop) {
			waveInAddBuffer(hwavein, (LPWAVEHDR)dwParam1, sizeof(WAVEHDR));  //添加到缓冲区
		}
		else {                                                               // 防止重复记录数据
			dat_ignore = true;
		}
		break;

	case WIM_CLOSE:
		break;

	default:
		break;
	}
	return 0;
}

WaveRecorder::WaveRecorder()
{
	this->fp = NULL;
	/*
	this->WavHeader = {
		{ 'R', 'I', 'F', 'F' },
		0,
		{ 'W', 'A', 'V', 'E' },
		{ 'f', 'm', 't', ' ' },
		sizeof(PCMWAVEFORMAT) ,
		WAVE_FORMAT_PCM,
		1,
		SAMPLE_RATE,
		SAMPLE_RATE*(SAMPLE_BITS / 8),
		SAMPLE_BITS / 8,
		SAMPLE_BITS,
		{ 'd', 'a', 't', 'a' },
		0
	};
	*/

	this->WavHeader.riff[0] = 'R';
	this->WavHeader.riff[1] = 'I';
	this->WavHeader.riff[2] = 'F';
	this->WavHeader.riff[3] = 'F';
	this->WavHeader.size_8 = 0;
	this->WavHeader.wave[0] = 'W';
	this->WavHeader.wave[1] = 'A';
	this->WavHeader.wave[2] = 'V';
	this->WavHeader.wave[3] = 'E';
	this->WavHeader.fmt[0] = 'f';
	this->WavHeader.fmt[1] = 'm';
	this->WavHeader.fmt[2] = 't';
	this->WavHeader.fmt[3] = ' ';
	this->WavHeader.fmt_size = sizeof(PCMWAVEFORMAT);
	this->WavHeader.format_tag = WAVE_FORMAT_PCM;
	this->WavHeader.channels = 1;
	this->WavHeader.samples_per_sec = SAMPLE_RATE;
	this->WavHeader.avg_bytes_per_sec = SAMPLE_RATE*(SAMPLE_BITS / 8);
	this->WavHeader.block_align = SAMPLE_BITS / 8;
	this->WavHeader.bits_per_sample = SAMPLE_BITS;
	this->WavHeader.data[0] = 'd';
	this->WavHeader.data[1] = 'a';
	this->WavHeader.data[2] = 't';
	this->WavHeader.data[3] = 'a';
	this->WavHeader.data_size = 0;

	// 如果没有输入设备则析构
	if (!waveInGetNumDevs()) {
#if _DEBUG
		cout << "ERROR : Windows can't find audio drive." << endl;
#endif
	}

#if _DEBUG
	WAVEINCAPS WaveInCaps;
	MMRESULT mmResult = waveInGetDevCaps(0, &WaveInCaps, sizeof(WAVEINCAPS));
	cout << "默认设备描述：(" << WaveInCaps.szPname << ")" << endl;
#endif
	WAVEFORMATEX pwfx;
	WaveInitFormat(&pwfx, CHANNEL_MUM, SAMPLE_RATE, SAMPLE_BITS);
	waveInOpen(&hwi, WAVE_MAPPER, &pwfx, (DWORD)WaveXAPI_Callback, NULL, CALLBACK_FUNCTION);
}

WaveRecorder::~WaveRecorder()
{
	// 关闭设备并发出WIM_CLOSE
	waveInClose(hwi);
}