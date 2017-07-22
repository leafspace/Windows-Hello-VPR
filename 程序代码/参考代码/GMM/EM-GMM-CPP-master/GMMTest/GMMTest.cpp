// GMMTest.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//
#include "stdafx.h"
#include "EMGMM.h"
#include "utility.h"


int _tmain(int argc, _TCHAR* argv[])
{
	CEMGMM gmm;
	int d, N;					// 次元、データ数
	double **x;					// データ
	double maxLik = -DBL_MAX;	// 対数尤度の最大値

	// データ読み込み
	x = LoadMatrix<double>( d , N , "sample.txt" );

	// 初期値を変えて計算
	for(int i=0 ; i<10 ; i++ )
	{
		double lik;
		gmm.SetData( 4 , x , N , d );

		// 収束するまで繰り返す
		for(int i=0 ; i<40 ; i++ )
		{
			printf("%d回目..." , i );
			gmm.EStep();
			gmm.MStep();
			lik = gmm.CalcLogliklihood();
			printf("L = %lf\n" , gmm.CalcLogliklihood() );
		}

		if( maxLik < lik )
		{
			// 尤度が最大ならば保存
			maxLik = lik;
			gmm.SaveResult( "result" );
			printf("更新 L = %lf \n" , lik );
		}
	}

	// メモリ解放
	Free( x );
	return 0;
}

