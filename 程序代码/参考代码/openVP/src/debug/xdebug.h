/** @file
********************************************************************************
<PRE>
模块名: 通用调试模块
文件名: xdebug.h
相关文件: xdebug.c
文件实现功能: 通用调试模块定义
作者: Dake
版本: V2011.07.06
编程方式: ANSI C语言编程
授权方式: Copyright(C) Dake
联系方式: chen423@yeah.net
生成日期: 2011-07-06
--------------------------------------------------------------------------------
多线程安全性: <是>
异常时安全性: <是/否>[，说明]
--------------------------------------------------------------------------------
备注: 
--------------------------------------------------------------------------------
修改记录:
日 期        版本     修改人              修改内容
YYYY/MM/DD   X.Y      <作者或修改者名>    <修改内容>
</PRE>
*******************************************************************************/

#ifndef XDEBUG_H
#define XDEBUG_H

#if (defined _WIN32) || (defined __linux__) /* windows或linux平台 */

#if (defined _DEBUG) || (defined DEBUG)
#define XDEBUG

#if (defined _WIN32) && (defined _WINDOWS) /* win32窗口模式 */
/**
 * 需在引用文件中重新定义以下两个宏值
 */
#undef X_PRINT  /*< 打印函数 */
#undef X_HANDLE /*< 打印窗口句柄 */
		
#elif (defined _WIN32) && (defined _CONSOLE) /* win32控制台模式 */
#define X_PRINT fprintf
#define X_HANDLE stdout

#else /* 其它模式或控制台 */
#define X_PRINT fprintf
#define X_HANDLE stdout	
#endif /* (defined _WIN32) && (defined _WINDOWS) */
	
#define xprint(fmt, ...) \
        X_PRINT(X_HANDLE, "%s(%d)-%s: "fmt"\r\n", __FILE__, __LINE__, __FUNCTION__,## __VA_ARGS__)
		
#else /* (defined _DEBUG) || (defined DEBUG) */
#define xprint(fmt, ...) ;
#endif /*（defined _DEBUG) || (defined DEBUG) */

#endif /* (defined _WIN32) || (defined __LINUX__) */

#endif /* XDEBUG_H */
