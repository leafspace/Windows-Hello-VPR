#pragma once
/*******************************************************************

EMGMM.h		developed by naka_t	2011.03.08


	混合ガウス分布のEMアルゴリズムによるパラメータ推定

　＊プログラム作成				naka_t	2010.03.08

  Copyright (C) 2011  naka_t <naka_t@apple.ee.uec.ac.jp>
 *******************************************************************/
#pragma warning(disable: 4819)
#include <vector>
#include <cv.h>
#include <cxcore.h> 
#include <highgui.h>

#pragma comment (lib, "cv210.lib") 
#pragma comment( lib, "cxcore210.lib")
#pragma comment( lib, "highgui210.lib")

class CEMGMM
{
public:
	CEMGMM(void);
	~CEMGMM(void);

	// データのセット＆初期化
	// m ; クラス数
	// x : データ
	// N : データ数
	// dim : データの次元
	void SetData( int m , double **x , int N ,int dim );

	void EStep();						// E step
	void MStep();						// M step
	int GetCluster(int n);				// クラスタリング結果を取得
	double CalcLogliklihood();			// 尤度を取得
	void SaveResult( const char *dir );	// 結果を保存

protected:
	int m_m;	// クラス数
	int m_N;	// データ数
	int m_d;	// 次元

	std::vector<cv::Mat> m_sigma_i;				// 分散
	std::vector<cv::Mat> m_mu_i;				// 平均
	std::vector<double> m_alpha_i;				// 混合比
	std::vector<double> m_N_i;					// i番目のクラスに属するデータ数
	std::vector<cv::Mat> m_x_n;					// データ
	std::vector<std::vector<double>> m_P_in;	// P(i|x_n,θ)

	double CalcPx_i( cv::Mat &x , int i );		// P( x | i ) の計算
};
