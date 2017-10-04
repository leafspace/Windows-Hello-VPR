/***************************************************************************
Module Name:
	KMeans

History:
	2003/10/16	Fei Wang
	2013 luxiaoxun
	Edit by leafspace
		TIME
			First time : 2017-7-24
			Last time : 2017-7-24
		CONTACT ME
			E-mail : 18852923073@163.com
***************************************************************************/
#include "../stdafx.h"

#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include "Model_KMeans.h"

using namespace std;

const int KMeans::ClusterNumber = 13;                                        //聚类中簇的个数

KMeans::KMeans(int dimNum, int clusterNum)
{
	m_dimNum = dimNum;
	m_clusterNum = clusterNum;

	m_means = new double*[m_clusterNum];                                     //分配空间
	for (int i = 0; i < m_clusterNum; i++)
	{
		m_means[i] = new double[m_dimNum];
		memset(m_means[i], 0, sizeof(double) * m_dimNum);                    //空间置零
	}

	m_initMode = InitRandom;
	m_maxIterNum = 100;
	m_endError = 0.001;
}

KMeans::~KMeans()
{
	for (int i = 0; i < m_clusterNum; i++)
	{
		delete[] m_means[i];
	}
	delete[] m_means;
}

void KMeans::Cluster(const char* sampleFileName, const char* labelFileName)
{
	cout << "TIP : K-means begin cluster ..." << endl;
	// Check the sample file
	ifstream sampleFile(sampleFileName, ios_base::binary);                   //初始化文件流
	assert(sampleFile);

	int size = 0;
	int dim = 0;
	sampleFile.read((char*)&size, sizeof(int));                              //读取size数据
	sampleFile.read((char*)&dim, sizeof(int));                               //读取dim数据
	assert(size >= m_clusterNum);
	assert(dim == m_dimNum);

	// Initialize model
	Init(sampleFile);                                                        //从文件流中读取数据并装载进分配的空间

	// Recursion
	double* x = new double[m_dimNum];	                                     //Sample data
	int label = -1;		                                                     //Class index
	double iterNum = 0;
	double lastCost = 0;
	double currCost = 0;
	int unchanged = 0;
	int* counts = new int[m_clusterNum];
	double** next_means = new double*[m_clusterNum];	                     //重新估计的模型数据 分配内存空间
	for (int i = 0; i < m_clusterNum; i++)
	{
		next_means[i] = new double[m_dimNum];
	}

	while (true)
	{
		memset(counts, 0, sizeof(int) * m_clusterNum);
		for (int i = 0; i < m_clusterNum; i++)
		{
			memset(next_means[i], 0, sizeof(double) * m_dimNum);
		}

		lastCost = currCost;
		currCost = 0;

		sampleFile.clear();
		sampleFile.seekg(sizeof(int) * 2, ios_base::beg);

		// Classification
		for (int i = 0; i < size; i++)
		{
			sampleFile.read((char*)x, sizeof(double) * m_dimNum);
			currCost += GetLabel(x, &label);

			counts[label]++;
			for (int d = 0; d < m_dimNum; d++)
			{
				next_means[label][d] += x[d];
			}
		}
		currCost /= size;

		// Reestimation
		for (int i = 0; i < m_clusterNum; i++)
		{
			if (counts[i] > 0)
			{
				for (int d = 0; d < m_dimNum; d++)
				{
					next_means[i][d] /= counts[i];
				}
				memcpy(m_means[i], next_means[i], sizeof(double) * m_dimNum);
			}
		}

		// Terminal conditions
		iterNum++;
		if (fabs(lastCost - currCost) < m_endError * lastCost)
		{
			unchanged++;
		}
		if (iterNum >= m_maxIterNum || unchanged >= 3)
		{
			break;
		}
	}

	// Output the label file
	ofstream labelFile(labelFileName, ios_base::binary);
	assert(labelFile);

	labelFile.write((char*)&size, sizeof(int));
	sampleFile.clear();
	sampleFile.seekg(sizeof(int) * 2, ios_base::beg);

	for (int i = 0; i < size; i++)
	{
		sampleFile.read((char*)x, sizeof(double) * m_dimNum);
		GetLabel(x, &label);
		labelFile.write((char*)&label, sizeof(int));
	}

	sampleFile.close();
	labelFile.close();

	delete[] counts;
	delete[] x;
	for (int i = 0; i < m_clusterNum; i++)
	{
		delete[] next_means[i];
	}
	delete[] next_means;
}

//N 为特征向量数
void KMeans::Cluster(double *data, int N, int *Label)
{
	cout << "TIP : K-means begin cluster ..." << endl;
	int size = 0;
	size = N;

	assert(size >= m_clusterNum);

	// Initialize model
	Init(data, N);

	// Recursion
	double* x = new double[m_dimNum];	                                     // Sample data
	int label = -1;		                                                     // Class index
	double iterNum = 0;
	double lastCost = 0;
	double currCost = 0;
	int unchanged = 0;
	//bool loop = true;
	int* counts = new int[m_clusterNum];
	double** next_means = new double*[m_clusterNum];	                     // New model for reestimation
	for (int i = 0; i < m_clusterNum; i++)
	{
		next_means[i] = new double[m_dimNum];
	}

	while (true)
	{
		memset(counts, 0, sizeof(int) * m_clusterNum);
		for (int i = 0; i < m_clusterNum; i++)
		{
			memset(next_means[i], 0, sizeof(double) * m_dimNum);
		}

		lastCost = currCost;
		currCost = 0;

		// Classification
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < m_dimNum; j++)
				x[j] = data[i*m_dimNum + j];

			currCost += GetLabel(x, &label);

			counts[label]++;
			for (int d = 0; d < m_dimNum; d++)
			{
				next_means[label][d] += x[d];
			}
		}
		currCost /= size;

		// Reestimation
		for (int i = 0; i < m_clusterNum; i++)
		{
			if (counts[i] > 0)
			{
				for (int d = 0; d < m_dimNum; d++)
				{
					next_means[i][d] /= counts[i];
				}
				memcpy(m_means[i], next_means[i], sizeof(double) * m_dimNum);
			}
		}

		// Terminal conditions
		iterNum++;
		if (fabs(lastCost - currCost) < m_endError * lastCost)
		{
			unchanged++;
		}
		if (iterNum >= m_maxIterNum || unchanged >= 3)
		{
			break;
		}
	}

	// Output the label file
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < m_dimNum; j++)
		{
			x[j] = data[i*m_dimNum + j];
		}
		GetLabel(x, &label);
		Label[i] = label;
	}
	delete[] counts;
	delete[] x;
	for (int i = 0; i < m_clusterNum; i++)
	{
		delete[] next_means[i];
	}
	delete[] next_means;
}

void KMeans::Init(ifstream& sampleFile)
{
	int size = 0;
	sampleFile.seekg(0, ios_base::beg);                                      //移动文件流到最顶端
	sampleFile.read((char*)&size, sizeof(int));

	if (m_initMode == InitRandom)
	{
		int inteval = size / m_clusterNum;
		double* sample = new double[m_dimNum];

		// Seed the random-number generator with current time
		srand((unsigned)time(NULL));

		for (int i = 0; i < m_clusterNum; i++)
		{
			int select = inteval * i + (inteval - 1) * rand() / RAND_MAX;    //选择随机选择的数据的个数
			int offset = sizeof(int) * 2 + select * sizeof(double) * m_dimNum;         //选择随机选择的数据的开始位置

			sampleFile.seekg(offset, ios_base::beg);                         //移动到开始位置位置
			sampleFile.read((char*)sample, sizeof(double) * m_dimNum);       //读取数据到sample内
			memcpy(m_means[i], sample, sizeof(double) * m_dimNum);           //将选中的数据段拷贝到保存空间处
		}

		delete[] sample;
	}
	else if (m_initMode == InitUniform)
	{
		double* sample = new double[m_dimNum];

		for (int i = 0; i < m_clusterNum; i++)
		{
			int select = i * size / m_clusterNum;
			int offset = sizeof(int) * 2 + select * sizeof(double) * m_dimNum;         //选择随机选择的数据的开始位置

			sampleFile.seekg(offset, ios_base::beg);                         //移动到开始位置位置
			sampleFile.read((char*)sample, sizeof(double) * m_dimNum);       //读取数据到sample内
			memcpy(m_means[i], sample, sizeof(double) * m_dimNum);           //将选中的数据段拷贝到保存空间处
		}

		delete[] sample;
	}
	else if (m_initMode == InitManual)
	{
		// Do nothing
	}
}

void KMeans::Init(double *data, int N)
{
	int size = N;

	if (m_initMode == InitRandom)
	{
		int inteval = size / m_clusterNum;
		double* sample = new double[m_dimNum];

		// Seed the random-number generator with current time
		srand((unsigned)time(NULL));

		for (int i = 0; i < m_clusterNum; i++)
		{
			int select = inteval * i + (inteval - 1) * rand() / RAND_MAX;    //定位要保存的目标数据区
			for (int j = 0; j < m_dimNum; j++)
			{
				sample[j] = data[select * m_dimNum + j];
			}
			memcpy(m_means[i], sample, sizeof(double) * m_dimNum);
		}

		delete[] sample;
	}
	else if (m_initMode == InitUniform)
	{
		double* sample = new double[m_dimNum];

		for (int i = 0; i < m_clusterNum; i++)
		{
			int select = i * size / m_clusterNum;                            //定位要保存的目标数据区
			for (int j = 0; j < m_dimNum; j++)
			{
				sample[j] = data[select * m_dimNum + j];
			}
			memcpy(m_means[i], sample, sizeof(double) * m_dimNum);
		}

		delete[] sample;
	}
	else if (m_initMode == InitManual)
	{
		// Do nothing
	}
}

double KMeans::GetLabel(const double* sample, int* label)
{
	double dist = -1;
	for (int i = 0; i < m_clusterNum; i++)
	{
		double temp = CalcDistance(sample, m_means[i], m_dimNum);
		if (temp < dist || dist == -1)
		{
			dist = temp;
			*label = i;
		}
	}
	return dist;
}

double KMeans::CalcDistance(const double* sample, const double* means, int dimNum)
{
	double temp = 0;
	for (int i = 0; i < dimNum; i++)
	{
		temp += pow(sample[i] - means[i], 2);
	}
	return sqrt(temp);
}

ostream& operator<<(ostream& out, KMeans& kmeans)
{
	out << "<KMeans>" << endl;
	out << "<DimNum> " << kmeans.m_dimNum << " </DimNum>" << endl;
	out << "<ClusterNum> " << kmeans.m_clusterNum << " </CluterNum>" << endl;

	out << "<Mean>" << endl;
	for (int i = 0; i < kmeans.m_clusterNum; i++)
	{
		for (int j = 0; j < kmeans.m_dimNum; j++)
		{
			out << kmeans.m_means[i][j] << " ";
		}
		out << endl;
	}
	out << "</Mean>" << endl;

	out << "</KMeans>" << endl;
	return out;
}
