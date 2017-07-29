/** @file
********************************************************************************
<PRE>
模块名: 基二时域快速傅立叶变换
文件名: FFT2.c
相关文件: FFT2.h
文件实现功能: 基二时域快速傅立叶变换相关函数的实现
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
#include "FFT2.h"


// 复数乘法
static void EE(COMPX b1, COMPX b2, COMPX  * result)
{
    result->real = b1.real*b2.real - b1.imag*b2.imag;
    result->imag = b1.real*b2.imag + b1.imag*b2.real;
}


void FFT2(COMPX  * xin, int N)
{
    int f, m, LH, nm, i, k, j, L;
    double p, ps;
    int le, B, ip;
    COMPX w, t;

    LH = N / 2;
    f = N;
    for (m = 1; (f = f / 2) != 1; ++m)
    {
        ;
    }
    nm = N-2;
    j = N / 2;
    /* 变址运算 */
    for (i = 1; i <= nm; ++i)
    {
        if (i < j)
        {
            t = xin[j];
            xin[j] = xin[i];
            xin[i] = t;
        }
        k = LH;
        while (j >= k)
        {
            j = j - k;
            k = k / 2;
        }
        j = j + k;
    }

    {
        for (L = 1; L<=m; L++)
        {
            le = (int)pow(2, L);
            B = le / 2;
            for (j = 0; j <= B - 1; j++)
            {
                p = pow(2, m-L) * j;
                ps = 2 * PI/ N * p;
                w.real = cos(ps);
                w.imag = -sin(ps);
                for (i = j; i <= N-1; i = i + le)
                {
                    ip = i + B;
                    EE(xin[ip], w, &t);
                    xin[ip].real = xin[i].real - t.real;
                    xin[ip].imag = xin[i].imag - t.imag;
                    xin[i].real = xin[i].real + t.real;
                    xin[i].imag = xin[i].imag + t.imag;
                }
            }
        }
    }
    return;
}


/*****************main programe********************/

//#include <math.h>
//#include <stdio.h>
//#include <stdlib.h>
//
//float  result[257];
//COMPX  s[257];
//int   Num=16;
//const float pp=3.14159;
//
//main()
//{
//
//	int i;
//	for(i=0;i<16;i++)   //初始化信号数据
//	{
//		s[i].real=sin(pp*i/32);
//		s[i].imag=0;
//	}
//
//	FFT(s,Num);
//
//	for(i=0;i<16;i++)
//	{
//		printf("%.4f",s[i].real);
//		printf("+%.4fj\n",s[i].imag);
//		result[i]=sqrt(pow(s[i].real,2)+pow(s[i].imag,2));
//	}
//}
