#include "WavFile_Initial.h"

int main()
{

	FILE *fp;
	if ((fp = fopen("Œ¢»Ìƒ¨»œ.wav", "rb")) == NULL) {
		cout << "ERROR : File open failed !" << endl;
		exit(-1);
	}
	WavFile_Initial wavFile(fp);
	for (unsigned long i = 0; i < wavFile.Get_voiceNumber(); ++i) {
		wavFile.Pre_emphasis(wavFile.Get_dataVoicePoint(i), wavFile.Get_WavFileData());
	}
	return 0;
}