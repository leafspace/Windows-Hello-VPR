#include "voiceIdentify.h"

// 静态内存测试版
#ifndef DEBUG_GMM
int main(void)
{
    enum {num = 10};
    int i;
    int j;
    int k;
    int res = 0;
    FILE * fi =NULL;
    char path[20];
    static double MFCC[num][GOOD_FRAME_NUM][D];
    //double ** MFCC[num] = {NULL};   //用来存储MFCC值
    PBYTE pRawVoice = NULL;   //指向原始语音数据

    GMM gmm[num] = {{{0}, {{0}}, {{0}}}};

    for (i = 0; i < num; ++i)
    {
        sprintf(path, "data/data%d.dat", i);
        fi = fopen(path, "rb");
        pRawVoice = (PBYTE)calloc(320, FRAME_LEN);
        fread(pRawVoice, 1, 320 * FRAME_LEN, fi);
        fclose(fi);

        if (voiceToMFCC(pRawVoice, 320 * FRAME_LEN, MFCC[i], 400))
        {
            free(pRawVoice);
            #ifdef _DEBUG
			printf("\nGMM[%d]:\n", i);
            printf("\nMFCC[%d]:\n", i);
            for (j = 0; j < 10; ++j)
            {
                for (k = 0; k < 1; ++k)
                {
                    printf("%lf  ",MFCC[i][j][k]);
                }
                printf("\n");
            }
            #endif // _DEBUG

            GMMs(MFCC[i], gmm+i);
        }
		else
		{
		    free(pRawVoice);
			return 0;
		}

    } //end  for (i = 0; i < num; ++i)

    res = GMM_identify(MFCC[1], &gmm, 100);
    printf("\nGMM[%d] identified!\n", res);

    return 0;
}
#endif //DEBUG_GMM
