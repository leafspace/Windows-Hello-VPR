#include "StdAfx.h"
#include "EMGMM.h"
#include <stdlib.h>
#include <math.h>

#pragma warning(disable: 4996)


CEMGMM::CEMGMM(void)
{
}

CEMGMM::~CEMGMM(void)
{
}


void CEMGMM::SetData( int m_ , double **x , int N ,int dim )
{
	m_d = dim;
	m_m = m_;
	m_N = N;
	cv::Mat mu;	// データ全体の平均
	cv::Mat s;	// データ全体の分散

	// メモリ確保
	m_sigma_i.resize( m_ );
	m_mu_i.resize( m_ );
	m_N_i.resize( m_ );
	m_alpha_i.resize( m_ );
	m_x_n.resize( N );
	m_P_in.resize( m_ );

	mu = cv::Mat::zeros( dim , 1 , CV_64F );
	s = cv::Mat::zeros( dim , dim , CV_64F );

	for(int i=0 ; i<m_ ; i++ )
	{
		m_sigma_i[i].create( dim , dim , CV_64F );
		m_mu_i[i].create( dim , 1 , CV_64F );
		m_P_in[i].resize( N );

		m_alpha_i[i] = 1.0/m_;	// 初期値
	}

	// データを代入
	for(int n=0 ; n<N ; n++ )
	{
		m_x_n[n] = cv::Mat( dim , 1 , CV_64F , x[n] );
		mu += m_x_n[n];
	}
	mu /= N;

	// 分散を計算
	for(int n=0 ; n<N ; n++ )
	{
		s += ( m_x_n[n] - mu )*( m_x_n[n] - mu ).t();
	}
	s /= N;

	// 初期値設定
	for(int i=0 ; i<m_ ; i++ )
	{
		unsigned int rnd;
		rand_s( &rnd );
		m_sigma_i[i] = s.clone();				// データ全体の分散を分散の初期値とする
		m_mu_i[i] = m_x_n[ rnd % N ].clone();	// 適当なデータを平均の初期値とする
	}
}

double CEMGMM::CalcPx_i( cv::Mat &x , int i )
{
	double detS = cv::determinant( m_sigma_i[i] );
	cv::Mat s = ( x - m_mu_i[i] ).t() * m_sigma_i[i].inv() * ( x - m_mu_i[i] );

	return pow( 2*M_PI , -m_d*0.5 ) * pow( detS , -0.5 ) * ::exp( -0.5 * s.at<double>(0,0)  );
}

void CEMGMM::EStep()
{
	for(int n=0 ; n<m_N ; n++ )
	{
		double sum = 0;
		for(int i=0 ; i<m_m ; i++ )
		{
			m_P_in[i][n] = m_alpha_i[i] * CalcPx_i( m_x_n[n] , i );
			sum += m_P_in[i][n];
		}

		for(int i=0 ; i<m_m ; i++ )
		{
			m_P_in[i][n] /= sum;
		}

	}
}

void CEMGMM::MStep()
{
	// Ni, μの計算
	for(int i=0 ; i<m_m ; i++ )
	{
		m_N_i[i] = 0;
		m_mu_i[i] = cv::Mat::zeros( m_d , 1 , CV_64F );
		for(int n=0 ; n<m_N ; n++ )
		{
			m_N_i[i] += m_P_in[i][n];
			m_mu_i[i] += m_P_in[i][n] * m_x_n[n];
		}
	}

	for(int i=0 ; i<m_m ; i++ )
	{
		m_mu_i[i] /= m_N_i[i];
	}

	// Σ, αiの計算
	for(int i=0 ; i<m_m ; i++ )
	{
		m_alpha_i[i] = m_N_i[i] / m_N;
		m_sigma_i[i] = cv::Mat::zeros( m_d , m_d , CV_64F );
		for(int n=0 ; n<m_N ; n++ )
		{
			m_sigma_i[i] += m_P_in[i][n] * (m_x_n[n] - m_mu_i[i]) * (m_x_n[n] - m_mu_i[i]).t();
		}
		m_sigma_i[i] /= m_N_i[i];
	}

	// 分散が0となるとまずいのでオフセットする
	for(int i=0 ; i<m_m ; i++ )
	{
		for (int j=0 ; j<m_d ; j++)
		{
			m_sigma_i[i].at<double>(j,j) += 0.000000000001;
		}
	}
}

int CEMGMM::GetCluster(int n)
{
	int maxIndex = -1;
	double maxP = -DBL_MAX;

	for(int i=0 ; i<m_m ; i++ )
	{
		if( maxP < m_P_in[i][n] )
		{
			maxP = m_P_in[i][n];
			maxIndex = i;
		}
	}

	return maxIndex;
}

double CEMGMM::CalcLogliklihood()
{
	double lik = 0;
	for(int n=0 ; n<m_N ; n++ )
	{
		double sum = 0;
		for(int i=0 ; i<m_m ; i++ )
		{
			sum += m_alpha_i[i] * CalcPx_i( m_x_n[n] , i );
		}
		lik += log( sum );
	}
	return lik;
}

void CEMGMM::SaveResult( const char *dir )
{
	char dirname[256];
	char filename[256];
	FILE *fp;

	strcpy( dirname , dir );
	int len = (int)strlen( dir );
	if( len==0 || dir[len-1] != '\\' || dir[len-1] != '/' ) strcat( dirname , "\\" );

	CreateDirectory( dirname , NULL );

	sprintf( filename , "%sClusteringResult.txt" , dirname );
	fp = fopen( filename , "w" );
	for(int n=0 ; n<m_N ; n++ )
	{
		fprintf( fp , "%d\n" , GetCluster( n ) );
	}
	fclose( fp );


	for(int i=0 ; i<m_m ; i++ )
	{
		sprintf( filename , "%sparam%03d.txt" , dirname , i );
		fp = fopen( filename , "w" );
		fprintf( fp , "平均\n" );
		for(int d=0 ; d<m_d ; d++ )
		{
			fprintf( fp , "%lf	" , m_mu_i[i].at<double>(d,0) );
		}
		fprintf( fp , "\n\n" );


		fprintf( fp , "分散\n" );
		for(int y=0 ; y<m_d ; y++ )
		{
			for(int x=0 ; x<m_d ; x++ )
			{
				fprintf( fp , "%lf	" , m_sigma_i[i].at<double>(y,x) );
			}
			fprintf( fp , "\n" );
		}
		fprintf( fp , "\n\n" );
		fclose( fp );
	}
}