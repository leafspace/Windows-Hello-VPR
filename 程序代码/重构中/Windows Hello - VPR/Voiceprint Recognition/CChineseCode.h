#pragma once

#include "WaveRecorder.h"

class CChineseCode
{
public:
	static void UTF_8ToUnicode(wchar_t* pOut, char *pText);                  //UTF-8   转换成 Unicode
	static void UnicodeToUTF_8(char* pOut, wchar_t* pText);                  //Unicode 转换成 UTF-8
	static void UnicodeToGB2312(char* pOut, wchar_t uData);                  //Unicode 转换成 GB2312 
	static void Gb2312ToUnicode(wchar_t* pOut, char *gbBuffer);              //GB2312  转换成 Unicode
	static void GB2312ToUTF_8(string& pOut, char *pText, int pLen);          //GB2312  转换成 UTF-8
	static void UTF_8ToGB2312(string &pOut, char *pText, int pLen);          //UTF-8   转换成 GB2312
};
