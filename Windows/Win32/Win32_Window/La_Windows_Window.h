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
*  ����ʱ��: 2021/10/14 0:34:10
*  ����޸�: 
*
*  ��    ��: ����뵱Ȼ���Ǻã�һ�������½�һ���̣߳���������
*
***************************************************************************************
*/


#ifndef __LA_WINDOWS_WINDOW__
#define __LA_WINDOWS_WINDOW__
#include <Win32_Interface/La_Tchar.h>
#include <windows.h>

bool laCreateWindow(UINT width, UINT height, const TCHAR* title, WNDPROC wndProc = nullptr);
bool laGetWindowHandle(HINSTANCE& hInst, HWND& hwnd);
bool GetWindowPos(HWND hwnd, int& x, int& y);
bool laGetWindowPos(int& x, int& y);
bool laCloseWindow();


#define SET_BIT(word, bit_flag)		((word) |= (bit_flag))
#define RESET_BIT(word, bit_flag)	((word) &= ~(bit_flag))


#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)



#endif
