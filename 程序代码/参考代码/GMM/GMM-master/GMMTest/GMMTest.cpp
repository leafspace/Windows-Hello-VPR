// GMMTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <assert.h>
#include "KMeans.h"
#include "GMM.h"
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	double * data = NULL;

	double data1[] = {
		0.0, 0.2, 0.4,
		0.3, 0.2, 0.4,
		0.4, 0.2, 0.4,
		0.5, 0.2, 0.4,
		5.0, 5.2, 8.4,
		6.0, 5.2, 7.4,
		4.0, 5.2, 4.4,
		10.3, 10.4, 10.5,
		10.1, 10.6, 10.7,
		11.3, 10.2, 10.9
	};

	data = data1;
	int size = 10; //Number of samples
	int dim = 3;   //Dimension of feature
	int cluster_num = 4; //Cluster number
	
	//New test code:
	double dataroot[] = {
		1.0, 0.0, 1.0,
		0.0, 1.0, 1.0,
		1.0, 1.0, 0.0,
		0.0, 2.0, 1.0,
	};

	int datarootprob[] = {
		10,
		30,
		20,
		10
	};

	double datarootstd[] = {
		1.0, 0.2, 0.1,
		0.2, 1.1, 0.4,
		0.0, 0.2, 1.3,
		1.2, 0.1, 1.3
	};

	int cTtlPoints = 0;
	for (int i = 0; i < sizeof(datarootprob)/sizeof(datarootprob[0]); i++)
	{
		cTtlPoints += datarootprob[i];
	}
	
	double * data2 = new double[cTtlPoints * dim];

	int iPoint = 0;
	for (int i = 0; i < sizeof(datarootprob)/sizeof(datarootprob[0]); i++)
	{
		for (int j = 0; j < datarootprob[i]; j++)
		{
			for (int k = 0; k < dim; k++)
			{
				double iOffset = 0.0;
				for (int l = 0; l < 12; l++)
				{
					iOffset += (rand() * 1.0f / RAND_MAX);
				}
				iOffset = iOffset / 12;
				data2[iPoint * dim + k] = dataroot[i * dim + k] + iOffset * datarootstd[i * dim + k];
				printf("%f, %f ", iOffset * datarootstd[i * dim + k], data2[iPoint * dim + k]);
			}
			printf("\n");
			iPoint++;
		}
	}
	
	size = cTtlPoints;
	data = data2;
	//end of new test code
	 
	KMeans* kmeans = new KMeans(dim, cluster_num);
	int* labels = new int[size];
	kmeans->SetInitMode(KMeans::InitUniform);
	kmeans->Cluster(data, size, labels);

	printf("Clustering result by k-meams:\n");
	for (int i = 0; i < size; ++i)
	{
		printf("%f, %f, %f belongs to %d cluster\n", data[i*dim + 0], data[i*dim + 1], data[i*dim + 2], labels[i]);
	}

	delete[]labels;
	delete kmeans;
	
	double test_data[4][3] = {
		1.0, 0.0, 1.0,
		0.4, 0.5, 0.6,
		5.0, 6.2, 8.4,
		10.3, 10.4, 10.5
	};

	GMM *gmm = new GMM(dim, 4); //GMM has 3 SGM
	gmm->Train(data, size); //Training GMM

	printf("\nTest GMM:\n");
	for (int i = 0; i < 4; ++i)
	{
		printf("The Probability of %f, %f, %f  is %f \n", test_data[i][0], test_data[i][1], test_data[i][2], gmm->GetProbability(test_data[i]));
	}

	//save GMM to file
	ofstream gmm_file("gmm.txt");
	assert(gmm_file);
	gmm_file << *gmm;
	gmm_file.close();

	delete gmm;

	delete[] data2;

	return 0;
}
