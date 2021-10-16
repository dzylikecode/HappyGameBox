#include "La_Dx_Graphic_Base.h"
#include <ddraw.h>
#include <Win32_Debug/La_Debug.h>
#pragma comment(lib, "ddraw.lib")
#include <list>

#define INIT_STRUCT(st)		{memset(&st, 0, sizeof(st)); st.dwSize = sizeof(st);}

namespace
{
	using namespace la;

	bool bInit = false;
	HWND global_hwnd;
	UINT global_width, global_height;
	bool global_bWindowed = true;
	LPDIRECTDRAW7	lpdd7 = nullptr;
	SURFACE_PTR		lpddsprimary = nullptr;
	SURFACE_PTR		lpddsback = nullptr;
	CLIP_PTR		lpddclipper = nullptr;
	CLIP_PTR		lpddclipperwin = nullptr;
	RECT rtClip = { 0 };
	const DWORD SCREEN_COLOR_KEY = 0;
	std::list<SURFACE_PTR> surfaceList;

	CLIP_PTR SurfaceAttachClipper(SURFACE_PTR lpddsurface, int num_rects, LPRECT clip_list)
	{
		LPDIRECTDRAWCLIPPER lpddclipper;
		//表面申请裁剪器
		//第一个必须置为0
		if (FAILED(lpdd7->CreateClipper(0, &lpddclipper, nullptr)))
		{
			DEBUG_INFO(ERR, TEXT("Create Clipper Failed!"));
			return nullptr;
		}

		//准备关联到裁剪器上，需要用到连续的内存，一部分是信息头，一部分是裁剪数据
		LPRGNDATA region_data;

		region_data = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER) + num_rects * sizeof(RECT));

		//把裁剪数据复制到数据载体上
		memcpy(region_data->Buffer, clip_list, num_rects * sizeof(RECT));

		//set up fields of header
		region_data->rdh.dwSize = sizeof(RGNDATAHEADER);
		region_data->rdh.iType = RDH_RECTANGLES;
		region_data->rdh.nCount = num_rects;
		region_data->rdh.nRgnSize = num_rects * sizeof(RECT);

		//找到裁剪列表的并集，放入头文件中
		//内部预设是这么大
		region_data->rdh.rcBound.left = 64000;
		region_data->rdh.rcBound.top = 64000;
		region_data->rdh.rcBound.right = -64000;
		region_data->rdh.rcBound.bottom = -64000;
		//开始查找并集
		for (int i = 0; i < num_rects; i++)
		{
			region_data->rdh.rcBound.left = min(region_data->rdh.rcBound.left, clip_list[i].left);
			region_data->rdh.rcBound.right = max(region_data->rdh.rcBound.right, clip_list[i].right);
			region_data->rdh.rcBound.top = min(region_data->rdh.rcBound.top, clip_list[i].top);
			region_data->rdh.rcBound.bottom = max(region_data->rdh.rcBound.bottom, clip_list[i].bottom);
		}

		//将数据发送
		if (FAILED(lpddclipper->SetClipList(region_data, 0)))
		{
			DEBUG_INFO(ERR, TEXT("Set Clipper List Failed!"));
			free(region_data);
			return nullptr;
		}

		// now attach the clipper to the surface
		if (FAILED(lpddsurface->SetClipper(lpddclipper)))
		{
			// release memory and return error
			DEBUG_INFO(ERR, TEXT("Attach the Clipper to the Surface Failed!"));
			free(region_data);
			return(NULL);
		}

		//释放载体
		free(region_data);
		//将接口返回给调用者
		return lpddclipper;
	}

	SURFACE_PTR CreateBackSurface(int width, int height, DWORD colorKey, DWORD surfaceMemoryStyle)
	{
		DDSURFACEDESC2 ddsd;
		INIT_STRUCT(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

		ddsd.dwHeight = height;
		ddsd.dwWidth = width;

		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | surfaceMemoryStyle;

		SURFACE_PTR lpddsurface;
		if (FAILED(lpdd7->CreateSurface(&ddsd, &lpddsurface, nullptr)))
		{
			DEBUG_INFO(ERR, TEXT("Create Surface Failed"));
			return nullptr;
		}

		DDCOLORKEY color_key;
		color_key.dwColorSpaceLowValue = colorKey;
		color_key.dwColorSpaceHighValue = colorKey;

		//如果设置色彩空间，则要标志 SPACE
		//还可以用 alpha 叠加操作，色彩叠加
		//目标色彩键，相当于栅栏
		if (FAILED(lpddsurface->SetColorKey(DDCKEY_SRCBLT, &color_key)))
		{
			DEBUG_INFO(ERR, TEXT("Surface Sets the Color Key Failed"));
		}
		return lpddsurface;
	}
}


namespace la
{
	bool InitDXGraphic(HWND hwnd, UINT width, UINT height, UINT bpp, bool bWindowed)
	{
		if (bInit) return true;

		global_hwnd = hwnd;
		global_width = width; global_height = height;
		global_bWindowed = bWindowed;
		//首先创建7.0的接口object
		if (FAILED(DirectDrawCreateEx(nullptr, (LPVOID*)&lpdd7, IID_IDirectDraw7, nullptr)))
		{
			DEBUG_INFO(ERR, TEXT("Create COM Direct Draw Failed"));
			return false;
		}

		//然后设置协作等级
		if (bWindowed)//针对窗口
		{
			if (FAILED(lpdd7->SetCooperativeLevel(hwnd, DDSCL_NORMAL)))
			{
				DEBUG_INFO(ERR, TEXT("COM Direct Draw Sets Cooperative Level Failed"));
				return false;
			}
		}
		else//针对全屏
		{
			//DDSCL_ALLOWMODEX 允许mode X
			//DDSCL_ALLOWREBOOT 检测ctrl + alt + delete
			//多线程
			//EXCLUSIVE FULLSCREEN 连用，表示不能用GDI改写屏幕
			if (FAILED(lpdd7->SetCooperativeLevel(hwnd, 
				DDSCL_ALLOWMODEX | 
				DDSCL_FULLSCREEN | 
				DDSCL_EXCLUSIVE | 
				DDSCL_ALLOWREBOOT | 
				DDSCL_MULTITHREADED)))
			{
				DEBUG_INFO(ERR, TEXT("COM Direct Draw Sets Cooperative Level Failed"));
				return false;
			}

			//设置显示模式
			if (FAILED(lpdd7->SetDisplayMode(width, height, 32, 0, 0)))
			{
				DEBUG_INFO(ERR, TEXT("COM Direct Draw Sets Display Mode Failed"));
				return false;
			}
		}

		DDSURFACEDESC2 ddsd;
		INIT_STRUCT(ddsd);

		//创建主表面
		if (!bWindowed)//full screen
		{
			ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
			//primary 可见
			//flip    有一个前端缓冲和后备缓冲
			//complex 翻转链
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

			//一个后备 双缓冲；也可2个 三缓冲
			ddsd.dwBackBufferCount = 1;
		}
		else
		{
			ddsd.dwFlags = DDSD_CAPS;
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

			//windows mode 没有后备表面缓冲，可以自己设计生成
			//没有打开，这个有效符，也没什么赋值的，前面已经清零了
			ddsd.dwBackBufferCount = 0;
		}

		if (FAILED(lpdd7->CreateSurface(&ddsd, &lpddsprimary, nullptr)))
		{
			DEBUG_INFO(ERR, TEXT("Create Primary Surface Failed"));
			return false;
		}

#if _LA_DEBUG_ON_
		//获取主表面的色素格式
		DDPIXELFORMAT ddpf;

		INIT_STRUCT(ddpf);

		lpddsprimary->GetPixelFormat(&ddpf);

		int dd_pixel_format = ddpf.dwRGBBitCount;
#if 0
		// based on masks determine if system is 5.6.5 or 5.5.5
		//RGB Masks for 5.6.5 mode
		//DDPF_RGB  16 R: 0x0000F800  
		//             G: 0x000007E0  
		//             B: 0x0000001F  

		//RGB Masks for 5.5.5 mode
		//DDPF_RGB  16 R: 0x00007C00  
		//             G: 0x000003E0  
		//             B: 0x0000001F  
		// test for 6 bit green mask)
		//if (ddpf.dwGBitMask == 0x000007E0)
		//   dd_pixel_format = DD_PIXEL_FORMAT565;

		// use number of bits, better method
		dd_pixel_format = ddpf.dwRGBBitCount;

		Write_Error("\npixel format = %d", dd_pixel_format);

		// set up conversion macros, so you don't have to test which one to use
		if (dd_pixel_format == DD_PIXEL_FORMAT555)
		{
			RGB16Bit = RGB16Bit555;
			Write_Error("\npixel format = 5.5.5");
		} // end if
		else
		{
			RGB16Bit = RGB16Bit565;
			Write_Error("\npixel format = 5.6.5");
		} // end else
#endif

		//记录并查看
		DEBUG_INFO(INFO, TEXT("\npixel format = %d"), dd_pixel_format);
		if (dd_pixel_format != sizeof(COLOR) * 8)
		{
			DEBUG_INFO(ERR, TEXT("need 32 bits per pixel"), sizeof(COLOR) * 8);
			return false;
		}
#endif

		//设置后备缓冲区,获取相应的指针
		//创建主表面的时候已经要求有1个后备缓冲区了
		if (!bWindowed)
		{
			ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;


			//关联到主表面
			if (FAILED(lpddsprimary->GetAttachedSurface(&ddsd.ddsCaps, &lpddsback)))
			{
				DEBUG_INFO(ERR, TEXT("Attached Surface Failed!"));
				return false;
			}
		}
		else
		{
			//full screen 下可以用flip技术
			//window mode 下创建双缓冲就用blit
			//利用的不是后备表面，而是离屏表面，没有和前表面建立联系
			lpddsback = CreateBackSurface(width, height, SCREEN_COLOR_KEY, DDSCAPS_SYSTEMMEMORY);
		}


		//清空表面，用黑色做背景
		if (bWindowed)
		{
			//只是清除后备缓冲区就好了
			//主表面需要进行裁剪才能清除
			//要不然会清除整个桌面
			//dx 模式下， 它拥有的是整个桌面，不是单独的一个窗口
			FillSurfaceColor(lpddsback, SCREEN_COLOR_KEY, nullptr);
		}
		else
		{
			//前后全部清除
			FillSurfaceColor(lpddsprimary, SCREEN_COLOR_KEY, nullptr);
			FillSurfaceColor(lpddsback, SCREEN_COLOR_KEY, nullptr);
		}

		//设置软件裁剪区域
		//这是相对于表面而定的
		//窗口模式下，也就说明了离屏表面可以设置 裁剪器
		rtClip.left = 0;
		rtClip.right = width - 1;
		rtClip.top = 0;
		rtClip.bottom = height - 1;

		//裁剪应该关联到后备缓冲区,主表面不需要
		//裁剪器最好和主表面通大小

		RECT screen_rect = { 0, 0, width, height };
		lpddclipper = SurfaceAttachClipper(lpddsback, 1, &screen_rect);


		//windows 的裁剪器比较特殊
		//它的后表面也关联了裁剪器，前表面也要，前表面会移动
		if (bWindowed)
		{

			if (FAILED(lpdd7->CreateClipper(0, &lpddclipperwin, nullptr)))
			{
				DEBUG_INFO(ERR, TEXT("Create Clipper Failed!"));
				return false;
			}
			//设置好后，会自动根据窗体来自动裁剪
			if (FAILED(lpddclipperwin->SetHWnd(0, hwnd)))
			{
				DEBUG_INFO(ERR, TEXT("Connect Clip With Window Failed!"));
				return false;
			}
			//关联到主表面
			//这就说明，在back surface 上有一个窗口大小的裁剪区（相对于屏幕的原点）
			//当blt时，它会自动移动到窗口处
			if (FAILED(lpddsprimary->SetClipper(lpddclipperwin)))
			{
				DEBUG_INFO(ERR, TEXT("Attach Clip To Primary Surface Failed!"));
				return false;
			}
		}

		bInit = true;
		return true;
	}
	void CloseDXGraphic()
	{
		if (bInit)
		{
			for (auto p = surfaceList.cbegin(); p != surfaceList.cend(); p++)
				(*p)->Release();

			//释放所有的COM组件
			if (lpddclipper)
			{
				lpddclipper->Release();
				lpddclipper = nullptr;
			}

			if (lpddclipperwin)
			{
				lpddclipperwin->Release();
				lpddclipperwin = nullptr;
			}

			if (lpddsback)
			{
				lpddsback->Release();
				lpddsback = nullptr;
			}

			if (lpddsprimary)
			{
				lpddsprimary->Release();
				lpddsprimary = nullptr;
			}

			if (lpdd7)
			{
				lpdd7->Release();
				lpdd7 = nullptr;
			}

			bInit = false;
		}
	}
	void FillSurfaceColor(SURFACE_PTR lpddsurface, DWORD color, RECT* client)
	{
		DDBLTFX ddbltfx;//用来控制blt的操作信息

		INIT_STRUCT(ddbltfx);

		//设置填充的颜色
		ddbltfx.dwFillColor = color;

		//利用blit来填充
		lpddsurface->Blt(client, nullptr, nullptr, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
	}
	//自己创建离屏表面来当作是后备缓冲
	SURFACE_PTR CreateSurface(int width, int height, DWORD colorKey, DWORD surfaceMemoryStyle)
	{
		DDSURFACEDESC2 ddsd;
		INIT_STRUCT(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

		ddsd.dwHeight = height;
		ddsd.dwWidth = width;

		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | surfaceMemoryStyle;

		SURFACE_PTR lpddsurface;
		if (FAILED(lpdd7->CreateSurface(&ddsd, &lpddsurface, nullptr)))
		{
			DEBUG_INFO(ERR, TEXT("Create Surface Failed"));
			return nullptr;
		}

		DDCOLORKEY color_key;
		color_key.dwColorSpaceLowValue = colorKey;
		color_key.dwColorSpaceHighValue = colorKey;

		//如果设置色彩空间，则要标志 SPACE
		//还可以用 alpha 叠加操作，色彩叠加
		//目标色彩键，相当于栅栏
		if (FAILED(lpddsurface->SetColorKey(DDCKEY_SRCBLT, &color_key)))
		{
			DEBUG_INFO(ERR, TEXT("Surface Sets the Color Key Failed"));
		}
		surfaceList.push_back(lpddsurface);
		return lpddsurface;
	}
	bool ReleaseSurface(SURFACE_PTR sur)
	{
		for (auto p = surfaceList.cbegin(); p != surfaceList.cend(); p++)
		{
			if (sur == *p)
			{
				(*p)->Release();
				p = surfaceList.erase(p);
				return true;
			}
		}
		return false;
	}
	void Flush()
	{
		//必须解锁才可以
		if (!global_bWindowed)//full screen 可以使用页面交换技术
		{
			//nullptr 是和主表面关联的后备缓冲
			//也可指向另外一个表面
			//默认是1次垂直逆程，可以（| DDFLIP_INTERVAL2) 2次
			//什么鬼东西，这么慢
			/*while (FAILED(lpddsprimary->Flip(nullptr, DDFLIP_WAIT)))
				continue;*/
				/*lpddsprimary->Flip(nullptr, DDFLIP_WAIT);*/


					//反正关联的是背面的裁剪器，与前表面无关，直接 fast
					/*RECT source_rect = { 0, 0, width, height };
					lpddsprimary->BltFast(0, 0, lpddsback, &source_rect, DDBLTFAST_WAIT);*/
					//-----------裂开，没 blt Fast 多少


				//不知道为什么最右边会有一条细细的颜色 如果用 nullptr
				//觉得用这个也是多此一举啊，因为我设置的表面宽度，裁剪宽度，以及
				//画线的软件宽度就是这么大呀，真不知道最边上的那些像素哪儿来的
				//RECT source_rect = { min_clip_x, min_clip_y, max_clip_x, max_clip_y };
			lpddsprimary->Blt(nullptr, lpddsback, nullptr, DDBLT_WAIT, nullptr);

		}
		else
		{
			//Windows mode 采用blt
			RECT dest_rect;
			POINT point = { 0, 0 };
			//获得客户区所在的位置
			ClientToScreen(global_hwnd, &point);
			//将后备缓冲坐标转化
			dest_rect.left = point.x;
			dest_rect.top = point.y;

			dest_rect.right = dest_rect.left + global_width;
			dest_rect.bottom = dest_rect.top + global_height;
#if _LA_DEBUG_ON_
			//裁剪器在主表面，用 blt
			if (FAILED(lpddsprimary->Blt(&dest_rect, lpddsback, nullptr, DDBLT_WAIT, nullptr)))
			{
				DEBUG_INFO(ERR, TEXT("Blt Failed!"));
				return;
			}
#else
			lpddsprimary->Blt(&dest_rect, lpddsback, nullptr, DDBLT_WAIT, nullptr);
#endif
		}

	}
	void WaitForVsync(void)
	{
#if _LA_DEBUG_ON_
		//使得系统等待，直到下一个垂直空白周期开始（当光栅化到底部时）
		if (FAILED(lpdd7->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, 0)))
		{
			DEBUG_INFO(ERR, TEXT("Can't Wait For V sync"));
			return;
		}
#else
		lpdd7->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, 0);
#endif
	}
	SURFACE_PTR& GetPrimarySurface() { return lpddsprimary; }
	SURFACE_PTR& GetBackSurface() { return lpddsback; }
	bool LockSurface(const SURFACE_PTR sur, PBYTE& memory, LONG& lpitch)
	{
		DDSURFACEDESC2 ddsd;
		INIT_STRUCT(ddsd);
		HRESULT hre = sur->Lock(nullptr, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, nullptr);
		memory = (PBYTE)ddsd.lpSurface;
		lpitch = ddsd.lPitch;
		return true;
	}
	bool UnlockSurface(const SURFACE_PTR sur)
	{
		HRESULT hre = sur->Unlock(nullptr);
		return true;
	}
}

