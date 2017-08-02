#include "WaveRecorder.h"
#include "conio.h"

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