#pragma once
#pragma once
/*******************************************************************

utility.h		developed by naka_t	2011.02.01

	ファイル入出力系関数


Copyright (C) 2011  naka_t <naka_t@apple.ee.uec.ac.jp>
*******************************************************************/
#include <vector>
#include <windows.h>

#pragma warning(disable: 4996)
#pragma warning(disable: 4018)

#ifndef Message
#define Message( mess ) ::MessageBox( 0 , mess , "Message" , 0 )
#endif


// ファイル入出力関数
static int fprintfVar( int val , FILE *fp ){ return fprintf( fp , "%d" , val ); }
static int fprintfVar( double val , FILE *fp ){ return fprintf( fp , "%lf" , val ); }
static int fprintfVar( std::string &val , FILE *fp ){ return fprintf( fp , "%s" , val.c_str() ); }
static int fprintfVar( const char *&val , FILE *fp ){ return fprintf( fp , "%s" , val ); }
static int fscanfVar( int *var , FILE *fp ){ return fscanf( fp, "%d" , var ); }
static int fscanfVar( double *var , FILE *fp ){ return fscanf( fp, "%lf" , var ); }
static int fscanfVar( float *var , FILE *fp ){ return fscanf( fp, "%f" , var ); }
static int fscanfVar( bool *var , FILE *fp ){ return fscanf( fp, "%d" , var ); }
static int fscanfVar( std::string *var , FILE *fp ){
	char tmpStr[1024] = {0};
	int ret = fscanf( fp , "%s" , tmpStr );
	*var = tmpStr;
	return ret;
}


//行数をカウント
static int CountLine( FILE *fp )
{
	int num = 0;
	int c;

	while(1)
	{
		c = fgetc(fp);

		if( c == '\0' || c == EOF ) break;
		else if( c == '\n' ) num++;
	}

	//printf( "lines %d\n" , num );

	rewind(fp);

	return num;
}

//行内のタブの数を数える
static int CountCols( FILE *fp )
{
	int num = 0;
	int c , old;

	while(1)
	{
		c = fgetc(fp);

		if( c == '\n' || c == EOF ) break;
		else if( c == '\t' ) num++;

		old = c;
	}

	//最後の列にタブが入っていない可能性もあるので
	if( old != '\t' && c == '\n' ) num++;

	rewind(fp);

	return num;
}

// メモリ確保
template<typename T> T** AllocMatrix( int ysiz , int xsiz)
{
	//配列の実態は1次元で確保していることに注意！

	T **mptr = NULL;
	T *data = NULL;	//配列の実態

	try
	{
		mptr = new T*[ysiz];
		data = new T[ysiz*xsiz];

		for (int i=0;i<ysiz;i++)
		{
			mptr[i] = data + i*xsiz;
		}

		return mptr;
	}
	catch( std::bad_alloc &ba )
	{
		std::string mess = "AllocMatrix : メモリの確保に失敗！\r\n";
		mess += ba.what();
		Message(mess.c_str());
		return NULL;
	}
}


// メモリ解放
template <typename T> void Free(T **mptr)
{
	if( mptr )
	{
		delete [] mptr[0];
		delete [] mptr;
	}
}

// 二次元データの読み込み
template <typename T> bool LoadMatrix( T **array , int xsize, int ysize , FILE *fp )
{
	T data;

	for(int m=0 ; m<ysize ; m++)
	{
		for(int n=0 ; n<xsize ; n++)
		{
			if( fscanfVar( &data , fp ) != 1 )
			{
				return false;
			}
			fscanf( fp , "	" );

			array[m][n]=data;
		}
		
		fscanf( fp , "\n");
	}
	return true;
}

// 二次元データの読み込み
template <typename T> T **LoadMatrix(int &xsize, int &ysize , const char *filename )
{
	FILE *fp = NULL;
	T **array = NULL;
	char mess[1024];

	fp = fopen( filename , "r");

	if( fp==NULL )
	{
		sprintf( mess, "%s がありません．" , filename );
		Message( mess );

	}


	//行数取得
	ysize = CountLine(fp);
	xsize = CountCols(fp);

	array = (T **)AllocMatrix<T>( ysize , xsize );

	if( !LoadMatrix( array , xsize, ysize , fp ) )
	{
		if( !fp ) fclose( fp );
		if( !array ) Free( array );

		sprintf( mess, "%s の書式が不正です．" , filename );
		Message( mess );

		return NULL;
	}

	fclose(fp);

	return array;
}


// 配列の保存
template <typename T> void SaveArray( T array, int size, const char *filename)
{
	int n;
	FILE *fp;

	fp = fopen( filename , "w" );
	for( n=0 ; n<size ; n++)
	{
		fprintfVar( array[n] , fp );
		fprintf( fp , "\n" );
	}
	fclose( fp );
};
