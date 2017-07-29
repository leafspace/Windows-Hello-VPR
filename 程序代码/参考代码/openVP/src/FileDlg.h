#include "global.h"

#ifndef _FILEDLG_H_
#define _FILEDLG_H_

typedef struct FILE_CLASS
{
	void (* f_Init)(struct FILE_CLASS *);
	TCHAR szFileName[MAX_PATH]; // 完整路径
	TCHAR szTitleName[60];      // 文件名
    int file_size;              // 文件大小
	void (* f_FileInitialize)(HWND);
	BOOL (* f_FileOpenDlg)(HWND, struct FILE_CLASS *);
	BOOL (* f_FileSaveDlg)(HWND, struct FILE_CLASS *);
	int (* f_CheckFileSize)(struct FILE_CLASS *);
}FILE_CLASS;

//extern FILE_CLASS voice_file;

void FileClassInit(FILE_CLASS * this);
void FileInitialize(HWND hwnd);
BOOL FileOpenDlg(HWND hwnd, FILE_CLASS * myfile);
BOOL FileSaveDlg(HWND hwnd, FILE_CLASS * myfile);
int CheckFileSize(FILE_CLASS * myfile);
#endif //_FILEDLG_H_