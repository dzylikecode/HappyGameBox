/*
***************************************************************************************
*  程    序:
*
*  作    者: LaDzy
*
*  邮    箱: mathbewithcode@gmail.com
*
*  编译环境: Visual Studio 2019
*
*  创建时间: 2021/09/05 11:02:09
*  最后修改:
*
*  简    介: 我想的是设计这样的，
* #define _LA_DEBUG_ON_ 开始调试
* 默认使用控制台输出信息
* #define _LA_DEBUG_FILE_   "error.txt"
* 这会输出到文件当中
* #define _LA_DEBUG_ON_BOTN_
* 同时输出
*
***************************************************************************************
*/
#ifndef __LA_DEBUG__
#define __LA_DEBUG__

#include <Console/La_WinConosle.h>
#include <Win32_Interface//La_StringClass.h>

#define DEBUG_CONSOLE	WriteInConsole


#ifdef _LA_DEBUG_ON_

#define DEBUG_INFO		DEBUG_CONSOLE

#else

#define DEBUG_INFO()	

#endif


template< typename... Args >
int MessageBoxPrintf(const TCHAR* szCaption, UINT uType, const TCHAR* szFormat, Args... args)
{
	std::tstring szBuffer = tstring_sprintf(szFormat, args...);
	return MessageBox(NULL, szBuffer.c_str(), szCaption, uType);
}

#endif