#include "GMM.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <Windows.h>
using namespace std;

void GMM::Init( const vector<double> &inputData,const int clustNum,double eps,double max_steps )
{
    this->data=inputData;
    this->dataNum=data.size();
    this->clusterNum=clustNum;
    this->epslon=eps;
    this->max_steps=max_steps;
    this->means.resize(clusterNum);
    this->means_bkp.resize(clusterNum);
    this->sigmas.resize(clusterNum);
    this->sigmas_bkp.resize(clusterNum);
    this->memberships.resize(clusterNum);
    this->memberships_bkp.resize(clusterNum);
    for (int i=0;i<clusterNum;i++)
    {
        memberships[i].resize(data.size());
        memberships_bkp[i].resize(data.size());
    }
    this->probilities.resize(clusterNum);
    this->probilities_bkp.resize(clusterNum);
    //initialize mixture probabilities
    for (int i=0;i<clusterNum;i++)
    {
        probilities[i]=probilities_bkp[i]=1.0/(double)clusterNum;
        //init means
        means[i]=means_bkp[i]=255.0*i/(clusterNum);
        //init sigma
        sigmas[i]=sigmas_bkp[i]=50;
    }
}
void GMM::train()
{
    //compute membership probabilities
    int i,j,k,m;
    double sum=0,sum2;
    int steps=0;
    bool go_on;
    do 
    {
        for (k=0;k<clusterNum;k++) {
            //compute membership probabilities
            for (j=0;j<data.size();j++)
            {
            //计算p(k|n)
                sum=0;
                for (m=0;m<clusterNum;m++)
                {
                sum+=probilities[m]*gauss(data[j],means[m],sigmas[m]);
                }
                //求分子
                memberships[k][j]=probilities[k]*gauss(data[j],means[k],sigmas[k])/sum;
            }
            //求均值
            //求条件概率的和
            sum=0;
            for (i=0;i<dataNum;i++)
            {
                sum+=memberships[k][i];
            }
            sum2=0;
            for (j=0;j<dataNum;j++)
            {
                sum2+=memberships[k][j]*data[j];
            }
            means[k]=sum2/sum;
            //求方差
            sum2=0;
            for (j=0;j<dataNum;j++)
            {
                sum2+=memberships[k][j]*(data[j]-means[k])*(data[j]-means[k]);
            }
            sigmas[k]=sqrt(sum2/sum);
            //求概率
            probilities[k]=sum/dataNum;
        }//end for k
        //check improvement
        go_on=false;
        for (k=0;k<clusterNum;k++)
        {
            if (means[k]-means_bkp[k]>epslon)
            {
                go_on=true;
                break;
            }
        }
        //back up
        this->means_bkp=means;
        this->sigmas_bkp=sigmas;
        this->probilities_bkp=probilities;
    } while (go_on&&steps++<max_steps);//end do while
}

double GMM::gauss( const double x,const double m,const double sigma )
{
    return.0/(sqrt(2*3.1415926)*sigma)*exp(-0.5*(x-m)*(x-m)/(sigma*sigma));
}
int GMM::predicate(double x)
{
    double max_p=-100;
    int i;
    double current_p;
    int bestIdx=0;
    for (i=0;i<clusterNum;i++)
    {
        current_p=gauss(x,means[i],sigmas[i]);
        if (current_p>max_p)
        {
            max_p=current_p;
            bestIdx=i;
        }
    }
    return bestIdx;
}
void GMM::print()
{
    int i;
    for (i=0;i<clusterNum;i++)
    {
        cout<<"Mean: "<<means[i]<<" Sigma: "<<sigmas[i]<<" Mixture Probability: "<<probilities[i]<<endl;
    }
}
//void mainxx()
//{
//    GMM gmm;
//    gmm.test();
//}
//void GMM::test()
//{
//    //test guass
//    for (int j=0;j<10;j++)
//    {
//        cout<<gauss(j,0,1)<<endl;
//    }
//    srand(GetTickCount());
//    vector<double> datas;
//    int i;
//    double d;
//    for (i=0;i<20;i++)
//    {
//        d=(rand()/(double)RAND_MAX)*255;
//        cout<<d<<endl;
//        datas.push_back(d);
//    }
//    this->Init(datas,2);
//    this->train();
//    this->predicate(100);
//}