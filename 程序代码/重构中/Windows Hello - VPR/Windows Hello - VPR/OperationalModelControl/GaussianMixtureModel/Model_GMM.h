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

class GMM
{
public:
	static const int SGMNumber;                                                       // 单高斯混合模型的个数

	GMM(int dimNum = 1, int mixNum = 1);
	~GMM();

	void Copy(GMM* gmm);

	void SetMaxIterNum(int i)     { m_maxIterNum = i; }
	void SetEndError(double f)    { m_endError = f; }

	int GetDimNum()         { return m_dimNum; }
	int GetMixNum()         { return m_mixNum; }
	int GetMaxIterNum()     { return m_maxIterNum; }
	double GetEndError()    { return m_endError; }

	double& Prior(int i)    { return m_priors[i]; }
	double* Mean(int i)     { return m_means[i]; }
	double* Variance(int i) { return m_vars[i]; }

	void setPrior(int i, double val)     { m_priors[i] = val; }
	void setMean(int i, double *val)     { for (int j = 0; j < m_dimNum; j++) m_means[i][j] = val[j]; }
	void setVariance(int i, double *val) { for (int j = 0; j < m_dimNum; j++) m_vars[i][j] = val[j]; }

	double GetProbability(const double* sample);

	/*	SampleFile: <size><dim><data>...*/
	void Train(const char* sampleFileName);
	void Train(double *data, int N);
	void Init(const char* sampleFileName);
	void Init(double *data, int N);

	void DumpSampleFile(const char* fileName);

	friend std::ostream& operator<<(std::ostream& out, GMM& gmm);
	friend std::istream& operator>>(std::istream& in, GMM& gmm);

private:
	int m_dimNum;		                                                              // 样本维数
	int m_mixNum;		                                                              // Gaussian数目
	double* m_priors;	                                                              // Gaussian权重
	double** m_means;	                                                              // Gaussian均值
	double** m_vars;	                                                              // Gaussian方差

	double* m_minVars;                                                                // A minimum variance is required. Now, it is the overall variance * 0.01.
	int m_maxIterNum;		                                                          // The stopping criterion regarding the number of iterations
	double m_endError;		                                                          // The stopping criterion regarding the error

private:
	double GetProbability(const double* x, int j);
	void Allocate();
	void Dispose();
};
