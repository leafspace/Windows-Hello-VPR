#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

/** @file
********************************************************************************
<PRE>
模块名: 声纹识别
文件名: global.h
相关文件:
文件实现功能: 全局资源声明及定义
作者: Dake
版本: V0.0.0.0
编程方式: ANSI C语言编程
授权方式: Copyright(C) Dake
联系方式: chen423@yeah.net
生成日期: 2010-11-16
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
#include "../global.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

//#define _DEBUG   //DEBUG选项

#define PI   3.1415926L
#define _2PI 6.2831853L


#ifndef BOOL
#define BOOL int
#endif

#ifndef BYTE
#define BYTE unsigned char
#define PBYTE BYTE *
#endif

#ifndef FALSE
#define FALSE 0
#define TRUE  (!FALSE)
#endif

#endif // GLOBAL_H_INCLUDED
