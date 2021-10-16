/*
***************************************************************************************
*  ��    ��:
*
*  ��    ��: LaDzy
*
*  ��    ��: mathbewithcode@gmail.com
*
*  ���뻷��: Visual Studio 2019
*
*  ����ʱ��: 2021/09/05 11:02:09
*  ����޸�:
*
*  ��    ��: ���������������ģ�
* #define _LA_DEBUG_ON_ ��ʼ����
* Ĭ��ʹ�ÿ���̨�����Ϣ
* #define _LA_DEBUG_FILE_   "error.txt"
* ���������ļ�����
* #define _LA_DEBUG_ON_BOTN_
* ͬʱ���
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