/** @file
********************************************************************************
<PRE>
模块名: 特征向量聚类过程
文件名: cluster.c
相关文件: cluser.h
文件实现功能: 特征向量聚类过程实现
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
备注: 
--------------------------------------------------------------------------------
修改记录:
日 期        版本     修改人              修改内容
YYYY/MM/DD   X.Y      <作者或修改者名>    <修改内容>
</PRE>
*******************************************************************************/
#include "cluster.h"

#define DIFF_CENTER_VALUE  1   //!< k均值归一阈值

static double getDistance(double * p1, double * p2);

/** @function
********************************************************************************
<PRE>
函数名: getDistance()
功能: 计算两点间的距离
用法:
参数:
[IN] p1: D维的向量
[IN] p2: D维的向量
返回: 两点距离的平方
调用:
主调函数: k_means()
</PRE>
*******************************************************************************/
static double getDistance(double * p1, double * p2)
{
	int i = 0;
	double distance = 0;

	while (i < D)
	{
		distance += pow(p1[i]-p2[i], 2);
		++i;
	}
	return distance;
}


/** @function
********************************************************************************
<PRE>
函数名: k_means()
功能: 通过k增值算法得到M组均值
用法:
参数:
[IN] c_vector: 特征向量组
[OUT] clusterIndex: 每个聚类所含向量索引
[OUT] cSize: 每个聚类所含向量数
[IN] train_frame_num: 训练帧帧数
[IN] m: 高斯混合数
返回:
调用: getDistance()
主调函数: InitGMM()
</PRE>
*******************************************************************************/
void k_means(double ** c_vector, GMM * pGMM, int * clusterIndex,  int * cSize, int train_frame_num, int m)
{
	int i;
	int j;
	int k; // 迭代次数，理想状态下应该为M
	int farIndex = 0;
	int * clusterSize = NULL;        // 聚类所含向量数
	double maxDistance = 0;          // 记录质心到最远向量的距离
	double minDistance = 0;          // 记录质心到最近向量的距离
	double distance = 0;             // 记录质心到向量的距离的临时变量
	double ** mCenter       = NULL;  // 质心
	double diff_Center       = 0;
    GMM gmm = *pGMM;


    clusterSize = (int *)calloc(m, sizeof(int));
    mCenter = (double **)malloc(m * sizeof(double *));
	for (i = 0; i < m; ++i)
	{
		mCenter[i] = (double *)calloc(D, sizeof(double));
	}

 
	// 求得全局质心
	for (i = 0; i < D; ++i)
	{
		for(j = 0; j < train_frame_num; ++j)
		{
			gmm.u[0][i] += c_vector[j][i];
		}
		gmm.u[0][i] /= train_frame_num;
	}

	//---------------------------------------------------------------------------------------------------
	// M次更新质心迭代过程
	//
	for (k = 1; k < m; ++k)
	{
		farIndex = 0; // 记录最远向量索引
		maxDistance = 0;
		//--------------------------------------------------------------------------------
		// 新增分裂点：计算每个向量到每个质心的距离,寻找离现有质心最远的向量作为新质心
		//
		for (j = 0; j < train_frame_num; ++j)
		{
			distance  = 0;
			// 向量到每个质心的距离和
			for (i = 0; i < k; ++i)    // i表示现有质心索引
			{
				distance += sqrt(getDistance(gmm.u[i], c_vector[j]));
			}
			if (distance > maxDistance)
			{
				maxDistance = distance;
				farIndex = j;
			}
		}
		memcpy(gmm.u[k], c_vector[farIndex], D*sizeof(double)); // 新增质心


		//--------------------------------------------------------------------------------
		// 重新分类及更新质心
		//
NEXTLOOP: // |D1-D2| < DIFF_CENTER_VALUE 过程
		for (j = 0; j < k+1; ++j)
		{
			memcpy(mCenter[j], gmm.u[j], D*sizeof(double));
		}

		for (j = 0; j < train_frame_num; ++j)
		{
			farIndex = 0;
			minDistance = getDistance(mCenter[0], c_vector[j]);
			// 分类过程，farIndex为向量所属质心索引
			for (i = 1; i <= k; ++i)    // i表示现有质心索引
			{
				distance = getDistance(mCenter[i], c_vector[j]);
				if (distance < minDistance)
				{
					minDistance = distance;
					farIndex = i;
				}
			}

			// 获取最后结果的向量所属质心情况
			if (k == m - 1)
			{
				clusterIndex[j] = farIndex;
			}
			clusterSize[farIndex] += 1;
			// 更新质心过程
			for (i = 0; i < D; ++i)
			{
				if (clusterSize[farIndex] == 1)
				{
					// 先清零
					gmm.u[farIndex][i] = 0;
				}
				gmm.u[farIndex][i] += c_vector[j][i];
			}

		} // end for (j = 0; j < GOOD_FRAME_NUM; ++j)

		// TODO   clusterSize有0值，即分堆不均

		// 获取最后结果的各质心所含向量数量
		if (k == m - 1)
		{
			memcpy(cSize, clusterSize, m * sizeof(int));
		}

		// TODO 得到新的质心(如果结果不合理，可继续更新过程)
		diff_Center = 0;
		for (i = 0; i <= k; ++i)
		{
			double temp = 0;

			if (clusterSize[i] > 0)
			{
				for (j = 0; j < D; ++j)
				{
					gmm.u[i][j] /= clusterSize[i];
					temp += pow(gmm.u[i][j] - mCenter[i][j], 2);
				}
				diff_Center += sqrt(temp);
			}
			clusterSize[i] = 0; // 重新置0
		}
		if (diff_Center >= DIFF_CENTER_VALUE)
		{
			goto NEXTLOOP;
		}

	}  // end for (k = 1; k < M; ++k)

	free(clusterSize);
	for (i = 0; i < m; ++i)
	{
		free(mCenter[i]);
	}
	free(mCenter);
}