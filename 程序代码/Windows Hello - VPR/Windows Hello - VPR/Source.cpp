#include "Wav_File_Handle.h"

int main()
{

	FILE *fp;
	if ((fp = fopen("Î¢ÈíÄ¬ÈÏ.wav", "rb")) == NULL) {
		cout << "ERROR : File open failed !" << endl;
		exit(-1);
	}
	Wav_File_Work wavFile(fp);
	return 0;
}