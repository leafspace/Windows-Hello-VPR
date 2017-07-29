/** @file
********************************************************************************
<PRE>
模块名: mfcc参数的提取
文件名: mfcc.c
相关文件: mfcc.h
文件实现功能: mfcc提取相关函数的实现
作者: Dake
版本: V2010.09.01
编程方式: ANSI C语言编程
授权方式: Copyright(C) Dake
联系方式: chen423@yeah.net
生成日期: 2010-06-22
--------------------------------------------------------------------------------
多线程安全性: <是/否>[，说明]
异常时安全性: <是/否>[，说明]
--------------------------------------------------------------------------------
备注: <其它说明>
--------------------------------------------------------------------------------
修改记录:
日 期        版本     修改人              修改内容
YYYY/MM/DD   X.Y      <作者或修改者名>    <修改内容>
</PRE>
*******************************************************************************/
#include "mfcc.h"

static void PreEmphasis(BYTE * pRawVoice, int iSampleNum, double * pDiffVoice); // 预增强
static double * JudgeTrainFrame2(double * pDiffVoice, int iSampleNum, int train_frame_num); // 判断是否为静寂语音
static BOOL FrameMultiHamming(double * pDiffVoice, VOICEFRAME * pVoiceFrame, int train_frame_num);
static void FrameToMFCC(VOICEFRAME * pVoiceFrame, double  ** MFCC, int train_frame_num);
static double HalfFrameEnergy(double * pDiffVoice); // 计算半帧能量


/**
 * @brief 三角带通滤波器数组
 */
static double melf16[239] =
{
    //0     [3]
    1.4254, 1.2616, 0.0445,
    //1     [3]
    0.7384, 1.9555, 0.9103,
    //2     [3]
    1.0897, 1.8484, 0.8501,
    //3     [5]
    0.1516, 1.1499, 1.9082, 1.0167, 0.1705,
    //4     [5]
    0.0918, 0.9833, 1.8295, 1.3652, 0.5970,
    //5     [5]
    0.6348, 1.4030, 1.8627, 1.1593, 0.4845,
    //6     [7]
    0.1373, 0.8407, 1.5155, 1.8359, 1.2116, 0.6099, 0.0291,
    //7     [7]
    0.1641, 0.7884, 1.3901, 1.9709, 1.4679, 0.9249, 0.3991,
    //8     [7]
    0.5321, 1.0751, 1.6009, 1.8894, 1.3948, 0.9144, 0.4476,
    //9     [9]
    0.1106, 0.6052, 1.0856, 1.5524, 1.9934, 1.5513, 1.1206, 0.7008, 0.2912,
    //10     [11]
    0.0066, 0.4487, 0.8794, 1.2992, 1.7088, 1.8915, 1.5012, 1.1198, 0.7469,
    0.3822, 0.0253,
    //11     [12]
    0.1085, 0.4988, 0.8802, 1.2531, 1.6178, 1.9747, 1.6759, 1.3336, 0.9983,
    0.6696, 0.3472, 0.0309,
    //12     [12]
    0.3241, 0.6664, 1.0017, 1.3304, 1.6528, 1.9691, 1.7205, 1.4158, 1.1166,
    0.8226, 0.5338, 0.2498,
    //13     [14]
    0.2795, 0.5842, 0.8834, 1.1774, 1.4662, 1.7502, 1.9707, 1.6961, 1.4260,
    1.1601, 0.8985, 0.6409, 0.3872, 0.1373,
    //14     [17]
    0.0293, 0.3039, 0.5740, 0.8399, 1.1015, 1.3591, 1.6128, 1.8627, 1.8911,
    1.6485, 1.4094, 1.1737, 0.9412, 0.7119, 0.4858, 0.2627, 0.0425,
    //15     [18]
    0.1089, 0.3515, 0.5906, 0.8263, 1.0588, 1.2881, 1.5142, 1.7373, 1.9575,
    1.8252, 1.6107, 1.3989, 1.1898, 0.9833, 0.7793, 0.5777, 0.3786, 0.1818,
    //16     [20]
    0.1748, 0.3893, 0.6011, 0.8102, 1.0167, 1.2207, 1.4223, 1.6214, 1.8182,
    1.9874, 1.7951, 1.6051, 1.4172, 1.2314, 1.0476, 0.8659, 0.6861, 0.5082,
    0.3322, 0.1580,
    //17     [24]
    0.0126, 0.2049, 0.3949, 0.5828, 0.7686, 0.9524, 1.1341, 1.3139, 1.4918,
    1.6678, 1.8420, 1.9857, 1.8151, 1.6462, 1.4791, 1.3136, 1.1497, 0.9874,
    0.8267, 0.6675, 0.5098, 0.3536, 0.1988, 0.0455,
    //18     [27]
    0.0143, 0.1849, 0.3538, 0.5209, 0.6864, 0.8503, 1.0126, 1.1733, 1.3325,
    1.4902, 1.6464, 1.8012, 1.9545, 1.8936, 1.7430, 1.5938, 1.4459, 1.2993,
    1.1540, 1.0100, 0.8672, 0.7256, 0.5852, 0.4459, 0.3078, 0.1709, 0.0350,
    //19     [29]
    0.1064, 0.2570, 0.4062, 0.5541, 0.7007, 0.8460, 0.9900, 1.1328, 1.2744,
    1.4148, 1.5541, 1.6922, 1.8291, 1.9650, 1.9003, 1.7667, 1.6341, 1.5025,
    1.3720, 1.2425, 1.1140, 0.9865, 0.8599, 0.7343, 0.6097, 0.4859, 0.3631,
    0.2412, 0.1202
};


/**
 * @brief 三角带通滤波器数组下标索引，表示（滤波器数组中的起始位置，最小列号，最大列号）
 */
static int indexM16[D][3] =
{
    {0, 1, 3}, {3, 2, 4}, {6, 4, 6}, {9, 5, 9},
    {14, 7, 11}, {19, 10, 14}, {24, 12, 18}, {31, 15, 21},
    {38, 19, 25}, {45, 22, 30}, {54, 26, 36}, {65, 31, 42},
    {77, 37, 48}, {89, 43, 56}, {103, 49, 65}, {120, 57, 74},
    {138, 66, 85}, {158, 75, 98}, {182, 86, 112}, {209, 99, 127},
};


/** @function
********************************************************************************
<PRE>
函数名: PreEmphasis()
功能: 预增强，对原始语音数据做差分
用法:
参数:
[IN] pRawVoice: 原始语音数据
[IN] iSampleNum: 语音样本数
[OUT] pDiffVoice: 差分后的数据
返回:
调用:
主调函数: voiceToMFCC()
</PRE>
*******************************************************************************/
static void PreEmphasis(BYTE * pRawVoice, int iSampleNum, double * pDiffVoice)
{
    int i;

    for (i = 0; i < iSampleNum; ++i)
    {
        pDiffVoice[i] = (pRawVoice[i] - 128) * 0.0078125;
    }
}


/** @function
********************************************************************************
<PRE>
函数名: JudgeTrainFrame2()
功能: 判断预增强后的数据是否可以作为训练帧（去除静寂语音的多段连接起来）
用法:
参数:
[IN] pDiffVoice: 增强（差分）后的语音数据
[IN] iSampleNum: 语音样本数
[IN] train_frame_num: 训练帧帧数
返回:
非空: 可以作为训练帧的信号首地址
NULL: 不可作为训练帧
调用: HalfFrameEnergy()
主调函数: voiceToMFCC()
备注: 返回的内存需要用户在voiceToMFCC()中自行释放
</PRE>
*******************************************************************************/
static double * JudgeTrainFrame2(double * pDiffVoice, int iSampleNum, int train_frame_num)
{
    int len = 0;
    int iNum;   // 合格的语音帧数
    int iFrame; // 帧序号
    int iFrameNum = (iSampleNum>>7) - 1;   // 帧总数=2*(样本数/256)-1
    double PriorHalfFrame = 0;             // 前半帧能量(前128B)
    double NextHalfFrame = 0;              // 后半帧能量(后128B)
    double FrameEnergy = 0;                // 一帧的累积能量
    double * pTranDiff = NULL;             // 合格语音
    double * pSave;                        // 合格语音存储地址
    double * pDiffHead = pDiffVoice;
    double * pDiffNext  = pDiffVoice;


	if (iFrameNum < train_frame_num)
	{
		return NULL;
	}
    pTranDiff = (double *)calloc((train_frame_num/2 + 1)*FRAME_LEN, sizeof(double));
	if (!pTranDiff)
	{
		return NULL;
	}
    pSave = pTranDiff;
    PriorHalfFrame = HalfFrameEnergy(pDiffHead);
    for (iFrame=1, iNum=0; iFrame<iFrameNum && iNum<train_frame_num; ++iFrame)
    {
        pDiffNext += (FRAME_LEN/2);        // 指向iFrame号帧的后半帧
        NextHalfFrame = HalfFrameEnergy(pDiffNext);
        FrameEnergy = PriorHalfFrame + NextHalfFrame;
        if (FrameEnergy < SILENCE_VALUE)   // 是静寂语音
        {
             if (iFrameNum-iFrame < train_frame_num-iNum)   // 剩下的帧不足以作为训练帧
            {
                free(pTranDiff);
                return NULL;
            }
            len = pDiffNext - pDiffHead - FRAME_LEN/2;
            if (len > 0)
            {
                memcpy(pSave, pDiffHead, len * sizeof(double));
                pSave += len;
            }
            pDiffNext +=  FRAME_LEN/2;  // 重新定位可用作训练的帧
            pDiffHead= pDiffNext;
            NextHalfFrame = HalfFrameEnergy(pDiffHead);
        }
		else
		{
			++iNum;
		}

        PriorHalfFrame = NextHalfFrame;
    } // end: for(;;)

    len = pDiffNext - pDiffHead + FRAME_LEN/2;
    if (len > 0)
    {
        memcpy(pSave, pDiffHead, len * sizeof(double));
    }

    return pTranDiff;
}


/** @function
********************************************************************************
<PRE>
函数名: FrameMultiHamming()
功能: 音框化并乘汉明窗，然后快速傅立叶变换：对差分后的语音数据分帧,并排除静默语音以256个
采样点为一个音框单位（帧），以128为音框之间的重迭单位然后乘以汉明窗,最后每帧做FFT2。
用法:
参数:
[IN] pDiffVoice: 增强（差分）后的语音数据
[OUT] pVoiceFrame: 指向可用的训练帧
[IN] train_frame_num: 训练帧帧数
返回:
TRUE: 分帧成功
FALSE: 分帧失败
调用:
主调函数: voiceToMFCC()
备注: pVoiceFrame指向的训练帧存储空间在此函数中申请
</PRE>
*******************************************************************************/
static BOOL FrameMultiHamming(double * pDiffVoice, VOICEFRAME * pVoiceFrame, int train_frame_num)
{
    int i;            // 分帧中间参数
    int iFrame = 0;   // 帧序号
    int iPointNum =  FRAME_LEN -1;


    //---------------------------------------------------------------------------
    // 边分帧边乘汉明窗，FFT2变换
    //
    for (iFrame = 0 ; iFrame < train_frame_num; ++iFrame)
    {
        pVoiceFrame[iFrame].pCmpx = (COMPX *)malloc(sizeof(COMPX)*FRAME_LEN);
        if (!pVoiceFrame[iFrame].pCmpx)
        {
            printf("malloc Frame error");
            while (iFrame > 0)
            {
                free(pVoiceFrame[--iFrame].pCmpx);
            }
            return FALSE;
        }

        for (i = 0; i < FRAME_LEN; ++i)
        {
            // 乘汉明窗
            pVoiceFrame[iFrame].pCmpx[i].real = pDiffVoice[128*iFrame+i] * (0.54-0.46*cos(_2PI*i/iPointNum));
            pVoiceFrame[iFrame].pCmpx[i].imag = 0;
        }

        FFT2(pVoiceFrame[iFrame].pCmpx, FRAME_LEN); // 基二快速傅立叶变换（时域）
    }
    return TRUE;
}


/** @function
********************************************************************************
<PRE>
函数名: FrameToMFCC()
功能: 从训练帧得到MFCC参数组
用法:
参数:
[IN] pVoiceFrame: FFT2后的训练帧
[OUT] MFCC: 指向存放MFCC参数组的数组指针，大小为MFCC[帧数][20]
[IN] train_frame_num: 训练帧帧数
返回:
调用:
主调函数: voiceToMFCC()
备注: 此函数中释放pVoiceFrame指向的训练帧空间
</PRE>
*******************************************************************************/
static void FrameToMFCC(VOICEFRAME * pVoiceFrame, double  ** MFCC, int train_frame_num)
{
    int i;                // 矩阵列号
    int j;                // 矩阵行号
    int iFrame  = 0;      // 帧序号
    double Z[D] = {0};    // 滤波器输出的对数能量
    double a[2] = {0};


    //---------------------------------------------------------------------------
    // 对每一帧边做三角带通滤波器，DCT，边释放帧空间
    //
    for (iFrame = 0; iFrame < train_frame_num; ++iFrame)
    {
        for (i = 0; i < 129; ++i)
        {
            for (j = 0; j < D; ++j)
            {
                if (i < indexM16[j][1] || i > indexM16[j][2])  // 滤波器元素位置不在索引中
                {
                    // 滤波器对应位置矩阵元素为0
                    continue;
                }
                else
                {
                    // 滤波器矩阵的乘法[20][129]*[129][1]=[20]
                    Z[j] += melf16[indexM16[j][0] + i - indexM16[j][1]] *
                            (pow(pVoiceFrame[iFrame].pCmpx[i].real, 2)+pow(pVoiceFrame[iFrame].pCmpx[i].imag, 2));
                }
            }    // for (j = 0; j < D; ++j)
        }    // for (i = 0; i < 129; ++i)

        for (j = 0; j < D; j++) // 得到滤波器输出的对数能量
        {
            Z[j] = log(Z[j]);
        }
        a[0] = pow(0.05, 0.5);
        a[1] = pow(0.1, 0.5);
        for (i = 0; i < D; i++) // DCT过程
        {
            MFCC[iFrame][i] = 0;
            for (j = 0; j < D; j++)
            {
                MFCC[iFrame][i] += Z[j] * cos(((2 * j + 1) * i * PI) / 40);
            }
            MFCC[iFrame][i] *= a[i != 0 ? 1 : 0];
        }
        for (j = 0; j < D; j++)
        {
            Z[j] = 0; // 重新置为0，以便下一帧使用
        }

        free(pVoiceFrame[iFrame].pCmpx);
    }
}


/** @function
********************************************************************************
<PRE>
函数名: HalfFrameEnergy()
功能: 计算半帧（128BYTE）的累积能量
用法:
参数:
[IN] pDiffVoice: 每半帧的起始指针
返回: 半帧累积能量值
调用:
主调函数: JudgeTrainFrame2()
备注: 效率需要改进
</PRE>
*******************************************************************************/
static double HalfFrameEnergy(double * pDiffVoice)
{
    int i;
    double iHalfEnergy = 0;


    for (i = 0; i < FRAME_LEN/2;  ++i)
    {
        iHalfEnergy += pow(pDiffVoice[i], 2);
    }
    return iHalfEnergy;
}


/** @function
********************************************************************************
<PRE>
函数名: voiceToMFCC()
功能: 得到语音的MFCC参数组
用法:
参数:
[IN] pRawVoice: 原始语音数据
[IN] iSampleNum: 语音样本数
[OUT] MFCC: 指向存放MFCC参数组的指针，大小为MFCC[帧数][20]
[IN] train_frame_num: 训练帧帧数
返回:
TRUE: 成功提取
FALSE: 提取失败
调用: PreEmphasis(), JudgeTrainFrame(), FrameMultiHamming(), FrameToMFCC()
主调函数:
备注: 提供给外层的接口
</PRE>
*******************************************************************************/
BOOL voiceToMFCC(BYTE * pRawVoice, int iSampleNum, double ** MFCC, int train_frame_num)
{
    double * pDiffVoice = NULL;
    double * pDiff = NULL;
    VOICEFRAME * pVoiceFrame = NULL; // 指向可用的训练帧


    //pDiffVoice = (double *)malloc(iSampleNum * sizeof(double));
	pDiffVoice = (double *)calloc(iSampleNum, sizeof(double));
	if (!pDiffVoice)
	{
		xprint("pDiffVoice");
		return FALSE;
	}
    PreEmphasis(pRawVoice, iSampleNum, pDiffVoice);    // 预增强
    pDiff = JudgeTrainFrame2(pDiffVoice, iSampleNum, train_frame_num);
    if (!pDiff)    // 判断静寂语音
    {
		xprint("Error: It`s silent voice!");
        free(pDiffVoice);
        return FALSE;
    }

	pVoiceFrame = (VOICEFRAME *)malloc(train_frame_num * sizeof(VOICEFRAME));
    FrameMultiHamming(pDiff, pVoiceFrame, train_frame_num);  // 得到频域帧
    free(pDiff);
    FrameToMFCC(pVoiceFrame, MFCC, train_frame_num); // 从帧得到MFCC参数组

	free(pVoiceFrame);
    free(pDiffVoice);
    return TRUE;
}
