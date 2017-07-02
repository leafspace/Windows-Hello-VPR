#include "WavFile_Initial.h"

int main()
{

	FILE *fp;
	if ((fp = fopen("微软默认.wav", "rb")) == NULL) {
		cout << "ERROR : File open failed !" << endl;
		exit(-1);
	}
	WavFile_Initial wavFile(fp);
	for (unsigned long i = 0; i < wavFile.Get_voiceNumber(); ++i) {
		wavFile.Pre_emphasis(wavFile.Get_dataVoicePoint(i), wavFile.Get_WavFileData());
	}

	double *dataSpace = new double[WavFile_Initial::N];
	for (unsigned long i = 1; i <= wavFile.Get_frameNumber(wavFile.Get_dataNumber()); ++i) {
		wavFile.Frame_Data(wavFile.Get_WavFileData(), wavFile.Get_dataNumber(), i, dataSpace, WavFile_Initial::N);
	}

	return 0;
}