/** @file
********************************************************************************
<PRE>
模块名: 高斯混合模型过程
文件名: gmm.c
相关文件: gmm.h
文件实现功能: 高斯混合模型过程相关函数的实现
作者: Dake
版本: V2010.09.01
编程方式: ANSI C语言编程
授权方式: Copyright(C) Dake
联系方式: chen423@yeah.net
生成日期: 2010-07-05
--------------------------------------------------------------------------------
多线程安全性: <是/否>[，说明]
异常时安全性: <是/否>[，说明]
--------------------------------------------------------------------------------
备注: 高斯混合模型：GMM＝｛p[i], u[i][D], cMatrix[i][D]｝，i=0,...,M-1
--------------------------------------------------------------------------------
修改记录:
日 期        版本     修改人              修改内容
YYYY/MM/DD   X.Y      <作者或修改者名>    <修改内容>
</PRE>
*******************************************************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "gmm.h"
#include "cluster.h"


#define DIFF_GMM_VALUE     1              //!< GMM归一阈值
#define IDENTIFY_VALUE    (-30000)        //!< 识别阈值
//#define IDENTITY_FRAME_NUM 100           //!< 识别帧数


static double getDistance(double * p1, double * p2);
static BOOL InitGMM(double ** X, GMM * pGMM, int train_frame_num, int m); // 初始化GMM参数
static BOOL BuildGMM(GMM * pGMM, GMM * Out, double ** X, int train_frame_num, int m);
static double GMM_diff(GMM * gmm1, GMM * gmm2);
static double GMM_density(GMM * pGMM, double * X, int index); // 高斯密度函数


/** @function
********************************************************************************
<PRE>
函数名: InitGMMClass()
功能: 初始化GMM结构
用法: 由GMM结构的f_init调用
参数:
[OUT] pGMM: GMM结构指针
[IN] m: GMM的高期混合数
返回: 
调用:
</PRE>
*******************************************************************************/
void InitGMMClass(GMM * pGMM, int m)
{
	pGMM->m = m;
	pGMM->p = NULL;
	pGMM->u = NULL;
	pGMM->cMatrix = NULL;
	pGMM->f_CallocGMM = CallocGMM;
	pGMM->f_FreeGMM = FreeGMM;
}


/** @function
********************************************************************************
<PRE>
函数名: CallocGMM()
功能: 为GMM结构内的指针分配内存
用法: 由GMM结构的f_CallocGMM调用
参数:
[OUT] pGMM: GMM结构指针
返回: 
调用:
</PRE>
*******************************************************************************/
void * CallocGMM(GMM * pGMM)
{
	int i;
	int m = pGMM->m;

	pGMM->p = (double *)calloc(m, sizeof(double));
	if (!pGMM->p)
	{
		return NULL;
	}
	pGMM->u = (double **)malloc(m * sizeof(double *));
	if (!pGMM->u)
	{
		free(pGMM->p);
		return NULL;
	}
	pGMM->cMatrix = (double **)malloc(m * sizeof(double *));
	if (!pGMM->cMatrix)
	{
		free(pGMM->p);
		free(pGMM->u);
		return NULL;
	}
	for (i = 0; i < m; ++i)
	{
		pGMM->u[i] = (double *)calloc(D, sizeof(double)); 
		if (!pGMM->u[i])
		{
			for(--i; i >=0 ; --i)
			{
				free(pGMM->u[i]);
				free(pGMM->cMatrix[i]);
			}
			free(pGMM->p);
			free(pGMM->u);
			free(pGMM->cMatrix);
			return NULL;
		}
		pGMM->cMatrix[i] = (double *)calloc(D, sizeof(double));
		if (!pGMM->cMatrix[i])
		{
			free(pGMM->u[i]);
			for(--i; i >=0 ; --i)
			{
				free(pGMM->u[i]);
				free(pGMM->cMatrix[i]);
			}
			free(pGMM->p);
			free(pGMM->u);
			free(pGMM->cMatrix);
			return NULL;
		}
	} // for (i = 0; i < m; ++i)
	return pGMM;
}


/** @function
********************************************************************************
<PRE>
函数名: FreeGMM()
功能: 释放为GMM结构内的指针分配内存
用法: 由GMM结构的f_FreeGMM调用
参数:
[IN] pGMM: GMM结构指针
返回: 
调用:
</PRE>
*******************************************************************************/
void FreeGMM(GMM * pGMM)
{
	int i;
	int m = pGMM->m;

	if (!pGMM)
	{
		return;
	}
	if (pGMM->p)
	{
		free(pGMM->p);
	}
	if (!pGMM->u && !pGMM->cMatrix)
	{
		return;
	}
	for (i = 0; i < m; ++i)
	{
		if (pGMM->u && pGMM->u[i])
		{
			free(pGMM->u[i]);
		}
		if (pGMM->cMatrix && pGMM->cMatrix[i])
		{
			free(pGMM->cMatrix[i]);
		}
	}
	if (pGMM->u)
	{
		free(pGMM->u);
	}
	if(pGMM->cMatrix)
	{
		free(pGMM->cMatrix);
	}
}





/** @function
********************************************************************************
<PRE>
函数名: InitGMM()
功能: 获得GMM初值
用法:
参数:
[IN] X: 特征向量组
[OUT] pGMM: GMM初值
[IN] train_frame_num: 训练帧帧数
[IN] m: 高斯混合数
返回:
TRUE: 成功
FALSE：失败
调用: k_means()
主调函数: GMMs()
</PRE>
*******************************************************************************/
static BOOL InitGMM(double ** X, GMM * pGMM, int train_frame_num, int m)
{
	int i;
	int j;
	int * clusterIndex = NULL;  // 向量所属质心索引
	int * clusterSize = NULL;   // 聚类所含向量数
	double ** sum = NULL;
	GMM gmm = *pGMM;


	clusterIndex = (int *)calloc(train_frame_num, sizeof(int));
	clusterSize = (int *)calloc(m, sizeof(int));
    sum = (double **)malloc(m * sizeof(double *));
	for (i = 0; i < m; ++i)
	{
		sum[i] = (double *)calloc(D, sizeof(double));
	}
	// 初始化均值u
	k_means(X, pGMM, clusterIndex, clusterSize, train_frame_num, m);

	// 初始化加权系数p
	for (i = 0; i < m; ++i)
	{
		gmm.p[i] = 1.0 / m;
	}

	// 初始化协方差矩阵cMatrix
	for (i = 0; i < train_frame_num; ++i)
	{
		for (j = 0; j < D; ++j)
		{
			sum[clusterIndex[i]][j] += pow(X[i][j], 2);
		}
	}
	for (i = 0; i < m; ++i)
	{
		if (clusterSize[i] > 0) // 防止分母为0
		{
			for (j = 0; j < D; ++j)
			{
				// TODO: 此处有负值和0值
				gmm.cMatrix[i][j] = sum[i][j] / clusterSize[i] - pow(gmm.u[i][j], 2);
				if (gmm.cMatrix[i][j] < 0)
				{
					xprint("Initial value of GMM`cMatrix < 0");
					gmm.f_FreeGMM(&gmm);
					free(clusterIndex);
					free(clusterSize);
					for (i = 0; i < m; ++i)
					{
						free(sum[i]);
					}
					free(sum);
					return FALSE;
				}
				else if(gmm.cMatrix[i][j] < 1e-10) // 消除相同double值相减的误差
				{
					gmm.cMatrix[i][j] = 0;
				}
			}
		} // end if (clusterSize[i] > 0)
		else
		{
			xprint(TEXT("InitGMM: Decrease M and increase frame number"));
			xprint(TEXT("clusterSize[%d]: %d\n"), i, clusterSize[i]);
			gmm.f_FreeGMM(&gmm);
			free(clusterIndex);
			free(clusterSize);
			for (i = 0; i < m; ++i)
			{
				free(sum[i]);
			}
			free(sum);
			return FALSE;
		}
	} // end for (i = 0; i < m; ++i)


	free(clusterIndex);
	free(clusterSize);
	for (i = 0; i < m; ++i)
	{
		free(sum[i]);
	}
	free(sum);
	return TRUE;
}


/** @function
********************************************************************************
<PRE>
函数名: GMM_diff()
功能: 计算迭代GMM的差值
用法:
参数:
[IN] gmm1: 这一轮迭代的结果
[IN] gmm2: 上一轮迭代的结果
返回: 两次迭代GMM的差值（各对应点差的平方和）
调用:
主调函数: GMMs()
</PRE>
*******************************************************************************/
static double GMM_diff(GMM * gmm1, GMM * gmm2)
{
	int i;
	int j;
	double diff = 0;


	for (i = 0; i < M; ++i)
	{
		diff += pow(gmm1->p[i] - gmm2->p[i], 2);
		for (j = 0; j < D; ++j)
		{
			diff += pow(gmm1->u[i][j] - gmm2->u[i][j], 2);
			diff += pow(gmm1->cMatrix[i][j] - gmm2->cMatrix[i][j], 2);
		}
	}

	return diff;
}


/** @function
********************************************************************************
<PRE>
函数名: GMM_density()
功能: 计算高斯概率密度
用法:
参数:
[IN] pGMM: 上一轮迭代的结果
[IN] X: 特征向量组
[IN] index: 高斯混合模型阶数索引
返回: 高斯概率密度值
调用:
主调函数: BuildGMM()
</PRE>
*******************************************************************************/
static double GMM_density(GMM * pGMM, double * X, int index) //高斯密度函数
{
	int i;
	double temp = 0;
	double sqrt_Matrix_value = 1;
	double res = 0;


	for (i = 0; i < D; ++i)
	{
		//TODO 对角矩阵的对角线中出现0，导致无穷大1#INF
		temp += pow(X[i] - pGMM->u[index][i], 2) / pGMM->cMatrix[index][i];
		//TODO sqrt出现-nan,因为开方值为负
		sqrt_Matrix_value *= sqrt(pGMM->cMatrix[index][i]);
	}
	//TODO temp值太大，导致exp(temp / -2)为0
	res = pow(_2PI, D/-2) / sqrt_Matrix_value * exp(temp / -2);
	return res;
}


/** @function
********************************************************************************
<PRE>
函数名: BuildGMM()
功能: 迭代下一轮GMM
用法:
参数:
[IN] pGMM: 上一次迭代的结果
[OUT] Out: 迭代得到的GMM
[IN] X: 特征向量组
[IN] train_frame_num: 训练帧帧数
[IN] m: 高斯混合数
返回: 
TRUE: 成功
FALSE: 失败
调用: GMM_density()
主调函数: GMMs()
</PRE>
*******************************************************************************/
static BOOL BuildGMM(GMM * pGMM, GMM * Out, double ** X, int train_frame_num, int m)
{
	int i;
	int j;
	int k;
	GMM temp_gmm = *Out;
	double ** p = NULL;      // train_frame_num个m组的事后概率
	double * sum_p = NULL;
	double * pb = NULL;      // 计算每个向量事后概率的中间量
	double sum_pb = 0;


	pb = (double *)calloc(m, sizeof(double));
	sum_p = (double *)calloc(m, sizeof(double));
	p = (double **)malloc(train_frame_num * sizeof(double *));
	for (i = 0; i < train_frame_num; ++i)
	{
		p[i] = (double *)calloc(m, sizeof(double));
	}
	// 计算事后概率
	for (i = 0; i < train_frame_num; ++i)
	{
		sum_pb = 0;
		for (j = 0; j < m; ++j)
		{
			pb[j] = pGMM->p[j] * GMM_density(pGMM, X[i], j);
			sum_pb += pb[j];
		}
		if (sum_pb == 0 || _isnan(sum_pb))
		{
			xprint("sum_ph = %lf", sum_pb);
			goto ERROR_RETURN;
		}
		for (j = 0; j < m; ++j)
		{
			p[i][j] = pb[j] / sum_pb;
			sum_p[j] += p[i][j];
		}
	}

	// 计算下一次迭代的GMM
	for (j = 0; j < m; ++j)
	{
		// 计算加权系数P
		temp_gmm.p[j] = sum_p[j] / train_frame_num;

		// 计算均值和协方差矩阵
		for (i = 0; i < train_frame_num; ++i)
		{
			for (k = 0; k < D; ++k)
			{
				if (i == 0)
				{
					temp_gmm.u[j][k] = 0;
					temp_gmm.cMatrix[j][k] = 0;
				}
				temp_gmm.u[j][k] += p[i][j] * X[i][k];
				temp_gmm.cMatrix[j][k] += p[i][j] * pow(X[i][k], 2);
			}
		}
		for (k = 0; k < D; ++k)
		{
			// 均值u
			temp_gmm.u[j][k] /= sum_p[j];
			// 协方差矩阵cMatrix
			temp_gmm.cMatrix[j][k] /= sum_p[j];
			temp_gmm.cMatrix[j][k] -= pow(temp_gmm.u[j][k], 2);
			if (temp_gmm.cMatrix[j][k] <= 0)
			{
				xprint("BuildGMM: cMatrix <= 0");
ERROR_RETURN:				
				for (i = 0; i < train_frame_num; ++i)
				{
					free(p[i]);
				}
				free(p);
				free(pb);
				free(sum_p);
				return FALSE;
			}
		} // for (k = 0; k < D; ++k)
	}

	for (i = 0; i < train_frame_num; ++i)
	{
		free(p[i]);
	}
	free(p);
	free(pb);
	free(sum_p);
	return TRUE;
}


/** @function
********************************************************************************
<PRE>
函数名: GMMs()
功能: GMM建模过程
用法:
参数:
[IN] X: 特征向量组
[OUT] pGMM: GMM模型
[IN] train_frame_num: 训练帧帧数
[IN] m: 高斯混合数
返回:
TRUE: 成功
FALSE: 失败
调用: BuildGMM(), GMM_density()
主调函数:
</PRE>
*******************************************************************************/
BOOL GMMs(double ** X, GMM * pGMM, int train_frame_num, int m)
{
	GMM first_gmm = {InitGMMClass};
	GMM second_gmm = {InitGMMClass};
	GMM * first = &first_gmm;
	GMM * second = &second_gmm;
	GMM * temp = NULL;
	BOOL bRes;
	double diff = 0;
    

	first_gmm.f_Init(&first_gmm, m);
	first_gmm.f_CallocGMM(&first_gmm);
	second_gmm.f_Init(&second_gmm, m);
	second_gmm.f_CallocGMM(&second_gmm);
	// TODO 初值的对角阵有0值和负值
	if (!InitGMM(X, first, train_frame_num, m)) // 获得GMM初值失败
	{
		return FALSE;
	}

	do // GMM迭代过程
	{
		bRes = BuildGMM(first, second, X, train_frame_num, m);
		if (!bRes)
		{
			first->f_FreeGMM(first);
			second->f_FreeGMM(second);
			return FALSE;
		}
		temp = first;
		first = second;
		second = temp;
		diff = GMM_diff(first, second);

		xprint("diff = %lf\n", diff);
	}
	while (diff >= DIFF_GMM_VALUE);

    first->f_FreeGMM(first);
	*pGMM = *second;
	return TRUE;
}


/** @function
********************************************************************************
<PRE>
函数名: GMM_identify()
功能: 计算后验概率，声纹识别过程
用法:
参数:
[IN] X: 特征向量组
[OUT] value: 识别值
[IN] pGMM: GMM模型
[IN] frame_num: 识别帧数
[IN] m: GMM模型的高斯混合数
返回:
成功: TRUE
失败: FALSE
调用: GMM_density()
主调函数:
</PRE>
*******************************************************************************/
BOOL GMM_identify(double ** X, double * value, GMM * pGMM, int frame_num, int m)
{
	int j;
	int k;
	double temp = 0;
	double * pb = NULL;
	double sum_pb = 0;


	pb = (double *)calloc(m, sizeof(double));
	for (j = 0; j < frame_num; ++j)
	{
		sum_pb = 0;
		for (k = 0; k < m; ++k)
		{
			pb[k] = pGMM[0].p[k] * GMM_density(pGMM, X[j], k);
			sum_pb += pb[k];
		}
// TODO sum_pb==0导致负无穷-nan的temp_max
		if (sum_pb <= 0)
		{
			xprint("后验概率sum_pb <= 0");
			return FALSE;
		}
		else
		{
			temp += log(sum_pb);
		}
	}
	xprint("max = %lf", temp);

	*value = temp;
	free(pb);
	return TRUE;
}

#ifdef DEBUG_GMM
int main(void)
{

	GMM gmm;
	double MFCC[9][2] = {{1, 1}, {1, 1.5}, {1.5, 1},
		{3, 3}, {3.5, 3}, {3, 3.5},
		{5, 0}, {5, 0.5}, {5.5, 0}
	};
	gmm = GMMs(MFCC);

	return 0;
}
#endif //DEBUG_GMM
