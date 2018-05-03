/***************************************************************************
	Module Name:
	KMeans

	History:
	2003/10/16	Fei Wang
	2013 luxiaoxun
	Edit by leafspace
	TIME
	First time : 2017-7-23
	Last time : 2017-7-24
	CONTACT ME
	E-mail : 18852923073@163.com
	***************************************************************************/

#pragma once
#include <fstream>
#include <assert.h>

class KMeans
{
public:
	static const int ClusterNumber;                                                   // 聚类中簇的个数

	enum InitMode                                                                     // 定义的模型初始化方法标签类型，代表着三种数据初始化的方法，与本类内m_initMode相关联
	{
		InitRandom,
		InitManual,
		InitUniform,
	};

	KMeans(int dimNum = 1, int clusterNum = 1);                                       // |Paragram1 : 数据阶数 |Paragram2 : 簇个数| 初始化Kmeans类
	~KMeans();

	void SetMeans(double **means)      { m_means = means; }                           // 设置数据
	void SetInitMode(int initMode)     { m_initMode = initMode; }                     // 设置初始化方式
	void SetMaxIterNum(int iterNum)    { m_maxIterNum = iterNum; }                    // 设置最高迭代次数
	void SetEndError(double endError)  { m_endError = endError; }                     // 设置误差的停止判断

	double** GetMean()                 { return m_means; }                            // 获取数据
	double* GetMean(int i)	           { return m_means[i]; }                         // 获取某行数据
	int GetInitMode()		           { return m_initMode; }                         // 获取初始化的方法
	int GetMaxIterNum()		           { return m_maxIterNum; }                       // 获取最高迭代次数
	double GetEndError()	           { return m_endError; }                         // 获取误差的停止判断


	/*
		SampleFile: <size><dim><data>...
		LabelFile:	<size><label>...
	*/
	void Cluster(const char* sampleFileName, const char* labelFileName);              // 划分簇，即GMM中的分类，方式：从文件中的数据读取
	void Cluster(double *data, int N, int *Label);                                    // 划分簇，即GMM中的分类，方式：原自用户的数据
	void Init(std::ifstream& sampleFile);
	void Init(double *data, int N);
	friend std::ostream& operator<<(std::ostream& out, KMeans& kmeans);

private:
	int m_dimNum;                                                                     // 数据阶数
	int m_clusterNum;                                                                 // 簇个数
	double** m_means;                                                                 // 数据

	int m_initMode;                                                                   // 数据初始化方法
	int m_maxIterNum;		                                                          // The stopping criterion regarding the number of iterations
	double m_endError;		                                                          // The stopping criterion regarding the error

	double GetLabel(const double* x, int* label);
	double CalcDistance(const double* x, const double* u, int dimNum);
};
