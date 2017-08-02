#include "WaveRecorder.h"
#include "conio.h"

/*这是一个回调函数示例*/
void CALLBACK ChunkArrived(array <char, CHUNCK_SIZE> ChunkData, UINT ChunksCount, bool bIsLast);

int main()
{
	char ch = 0;
	WaveRecorder sound_gbr;
	while (ch!='q')
	{
		ch = _getch();
		switch (ch)
		{
		case 'a':
			cout << "实时获取音频数据并保存于相对路径下test.wav" << endl;
			sound_gbr.set_FileName("test.wav");
			sound_gbr.Start();
			system("pause");
			sound_gbr.Stop();
			sound_gbr.Reset();
			break;
		case 's':
			cout << "仅实时获取音频数据，ChunkData" << endl;
			sound_gbr.set_Callback(ChunkArrived);
			sound_gbr.Start();
			system("pause");
			sound_gbr.Stop();
			sound_gbr.Reset();
			break;
		case 'd':
			cout << "Start后音频数据会存放于RawData，你可以直接处理它" << endl;
			sound_gbr.Start();
			system("pause");
			sound_gbr.Stop();
			sound_gbr.Reset();
			break;
		default:
			break;
		}
	}
}

/*这是一个回调函数示例*/
void CALLBACK ChunkArrived(array <char, CHUNCK_SIZE> ChunkData, UINT ChunksCount, bool bIsLast)
{
	// ChunkData.data()是数据的起始指针
	// ChunkData.size()等于CHUNCK_SIZE
	if (bIsLast)
	{
		// 在此添加最后一块数据的处理
		// AudioWrite(sessionID, ChunkData.data(), ChunkData.size(), AUDIO_LAST);
	}
	// cout << "ChunkData计数：" << ChunksCount << endl;
	if (ChunksCount > 0)
	{
		// AudioWrite(sessionID, ChunkData.data(), ChunkData.size(), AUDIO_CONTINUE);
	}
	else
	{
		// AudioWrite(sessionID, ChunkData.data(), ChunkData.size(), AUDIO_FIRST );
	}
}