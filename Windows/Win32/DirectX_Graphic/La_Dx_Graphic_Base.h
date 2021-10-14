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
*  创建时间: 2021/06/01 2:39:55
*  最后修改:
*
*  简    介:
*
***************************************************************************************
*/
#ifndef __LA_DX_GRAPHIC_BASE__
#define __LA_DX_GRAPHIC_BASE__
#ifndef INITGUID
#define INITGUID  //使用一些预定义好的 guid
#endif
#include <windows.h>
#include <ddraw.h>

#define PALETTE_NUM			 256
typedef  UINT  COLOR;
typedef  COLOR laPALETTE[PALETTE_NUM];

//创建的是离屏表面
//0 是 默认VRAM内存中
//DDCAPS_SYSTEMMEMORY 是系统内存
//也可以创建在显存当中
#define SURFACE_DEFAULT_M	0
#define SURFACE_LOCALVIDEO	DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM
#define SURFACE_NONLOCALVM	DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM	

#define RGB_DX(r,g,b)          ((COLOR)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((COLOR)(BYTE)(r))<<16)))
#define GetBValue_DX(rgb)      (LOBYTE(rgb))
#define GetGValue_DX(rgb)      (LOBYTE(((WORD)(rgb)) >> 8))
#define GetRValue_DX(rgb)      (LOBYTE((rgb)>>16))
#define GetAValue_DX(rgb)	   (LOBYTE((rgb)>>24))

inline void DisRGB(const int rgb, int& r, int& g, int& b) { r = GetRValue_DX(rgb); g = GetGValue_DX(rgb);  b = GetBValue_DX(rgb); }


namespace la
{
	bool InitDXGraphic(HWND hwnd, UINT width, UINT height, bool bWindowed = true);
	LPDIRECTDRAWCLIPPER SurfaceAttachClipper(LPDIRECTDRAWSURFACE7 lpddsurface, int num_rects, LPRECT clip_list);
	LPDIRECTDRAWSURFACE7 CreateSurface(int width, int height, DWORD colorKey, DWORD surfaceMemoryStyle);
	void FillSurfaceColor(LPDIRECTDRAWSURFACE7 lpddsurface, DWORD color, RECT* client);
	void ResetClipper(int left, int top, int right, int bottom);
	inline void MemSetColor(void* dest, COLOR data, int count)
	{
		//汇编形式
		_asm
		{
			mov edi, dest;//edi 保留指针
			mov ecx, count;//多少个32位,不是内存大小
			mov eax, data;//32位数据
			rep stosd;
			/*
			   SCAS(B/W/D) - to search a byte (word,DWORD) in memory at ES:DI
			   CMPS(B/W/D) - to compare blocks of memory: ES:DI against DS:SI
			   MOVS(B/W/D) - to copy blocks of memory: from DS:SI to ES:DI
			*/
		}
	}
	inline void MemSetWORD(void* dest, USHORT data, int count)
	{
		_asm
		{
			mov edi, dest; //edi points to destination memory
			mov ecx, count; //number of 16 - bit words to move
			mov ax, data; //16 - bit data
			rep stosw; //move data
		}
	}
	void CloseDXGraphic();
	void Flush();
	void WaitForVsync(void);
}



#endif
