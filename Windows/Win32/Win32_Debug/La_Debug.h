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


#define DEBUG_CONSOLE	WriteInConsole


#ifdef _LA_DEBUG_ON_

#define DEBUG_INFO		DEBUG_CONSOLE

#else

#define DEBUG_INFO()	

#endif


#include <assert.h>
template< typename... Args >
std::string string_sprintf(const char* format, Args... args) 
{
	int length = std::snprintf(nullptr, 0, format, args...);
	assert(length >= 0);

	char* buf = new char[length + 1];
	std::snprintf(buf, length + 1, format, args...);

	std::string str(buf);
	delete[] buf;
	return std::move(str);
}

template< typename... Args >
int MessageBoxPrintf(const TCHAR* szCaption, UINT uType, const TCHAR* szFormat, Args... args)
{
	std::tstring szBuffer;
	return MessageBox(NULL, szBuffer, szCaption, uType);
}

#endif