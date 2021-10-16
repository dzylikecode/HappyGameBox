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
		//��������ü���
		//��һ��������Ϊ0
		if (FAILED(lpdd7->CreateClipper(0, &lpddclipper, nullptr)))
		{
			DEBUG_INFO(ERR, TEXT("Create Clipper Failed!"));
			return nullptr;
		}

		//׼���������ü����ϣ���Ҫ�õ��������ڴ棬һ��������Ϣͷ��һ�����ǲü�����
		LPRGNDATA region_data;

		region_data = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER) + num_rects * sizeof(RECT));

		//�Ѳü����ݸ��Ƶ�����������
		memcpy(region_data->Buffer, clip_list, num_rects * sizeof(RECT));

		//set up fields of header
		region_data->rdh.dwSize = sizeof(RGNDATAHEADER);
		region_data->rdh.iType = RDH_RECTANGLES;
		region_data->rdh.nCount = num_rects;
		region_data->rdh.nRgnSize = num_rects * sizeof(RECT);

		//�ҵ��ü��б�Ĳ���������ͷ�ļ���
		//�ڲ�Ԥ������ô��
		region_data->rdh.rcBound.left = 64000;
		region_data->rdh.rcBound.top = 64000;
		region_data->rdh.rcBound.right = -64000;
		region_data->rdh.rcBound.bottom = -64000;
		//��ʼ���Ҳ���
		for (int i = 0; i < num_rects; i++)
		{
			region_data->rdh.rcBound.left = min(region_data->rdh.rcBound.left, clip_list[i].left);
			region_data->rdh.rcBound.right = max(region_data->rdh.rcBound.right, clip_list[i].right);
			region_data->rdh.rcBound.top = min(region_data->rdh.rcBound.top, clip_list[i].top);
			region_data->rdh.rcBound.bottom = max(region_data->rdh.rcBound.bottom, clip_list[i].bottom);
		}

		//�����ݷ���
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

		//�ͷ�����
		free(region_data);
		//���ӿڷ��ظ�������
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

		//�������ɫ�ʿռ䣬��Ҫ��־ SPACE
		//�������� alpha ���Ӳ�����ɫ�ʵ���
		//Ŀ��ɫ�ʼ����൱��դ��
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
		//���ȴ���7.0�Ľӿ�object
		if (FAILED(DirectDrawCreateEx(nullptr, (LPVOID*)&lpdd7, IID_IDirectDraw7, nullptr)))
		{
			DEBUG_INFO(ERR, TEXT("Create COM Direct Draw Failed"));
			return false;
		}

		//Ȼ������Э���ȼ�
		if (bWindowed)//��Դ���
		{
			if (FAILED(lpdd7->SetCooperativeLevel(hwnd, DDSCL_NORMAL)))
			{
				DEBUG_INFO(ERR, TEXT("COM Direct Draw Sets Cooperative Level Failed"));
				return false;
			}
		}
		else//���ȫ��
		{
			//DDSCL_ALLOWMODEX ����mode X
			//DDSCL_ALLOWREBOOT ���ctrl + alt + delete
			//���߳�
			//EXCLUSIVE FULLSCREEN ���ã���ʾ������GDI��д��Ļ
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

			//������ʾģʽ
			if (FAILED(lpdd7->SetDisplayMode(width, height, 32, 0, 0)))
			{
				DEBUG_INFO(ERR, TEXT("COM Direct Draw Sets Display Mode Failed"));
				return false;
			}
		}

		DDSURFACEDESC2 ddsd;
		INIT_STRUCT(ddsd);

		//����������
		if (!bWindowed)//full screen
		{
			ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
			//primary �ɼ�
			//flip    ��һ��ǰ�˻���ͺ󱸻���
			//complex ��ת��
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

			//һ���� ˫���壻Ҳ��2�� ������
			ddsd.dwBackBufferCount = 1;
		}
		else
		{
			ddsd.dwFlags = DDSD_CAPS;
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

			//windows mode û�к󱸱��滺�壬�����Լ��������
			//û�д򿪣������Ч����Ҳûʲô��ֵ�ģ�ǰ���Ѿ�������
			ddsd.dwBackBufferCount = 0;
		}

		if (FAILED(lpdd7->CreateSurface(&ddsd, &lpddsprimary, nullptr)))
		{
			DEBUG_INFO(ERR, TEXT("Create Primary Surface Failed"));
			return false;
		}

#if _LA_DEBUG_ON_
		//��ȡ�������ɫ�ظ�ʽ
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

		//��¼���鿴
		DEBUG_INFO(INFO, TEXT("\npixel format = %d"), dd_pixel_format);
		if (dd_pixel_format != sizeof(COLOR) * 8)
		{
			DEBUG_INFO(ERR, TEXT("need 32 bits per pixel"), sizeof(COLOR) * 8);
			return false;
		}
#endif

		//���ú󱸻�����,��ȡ��Ӧ��ָ��
		//�����������ʱ���Ѿ�Ҫ����1���󱸻�������
		if (!bWindowed)
		{
			ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;


			//������������
			if (FAILED(lpddsprimary->GetAttachedSurface(&ddsd.ddsCaps, &lpddsback)))
			{
				DEBUG_INFO(ERR, TEXT("Attached Surface Failed!"));
				return false;
			}
		}
		else
		{
			//full screen �¿�����flip����
			//window mode �´���˫�������blit
			//���õĲ��Ǻ󱸱��棬�����������棬û�к�ǰ���潨����ϵ
			lpddsback = CreateBackSurface(width, height, SCREEN_COLOR_KEY, DDSCAPS_SYSTEMMEMORY);
		}


		//��ձ��棬�ú�ɫ������
		if (bWindowed)
		{
			//ֻ������󱸻������ͺ���
			//��������Ҫ���вü��������
			//Ҫ��Ȼ�������������
			//dx ģʽ�£� ��ӵ�е����������棬���ǵ�����һ������
			FillSurfaceColor(lpddsback, SCREEN_COLOR_KEY, nullptr);
		}
		else
		{
			//ǰ��ȫ�����
			FillSurfaceColor(lpddsprimary, SCREEN_COLOR_KEY, nullptr);
			FillSurfaceColor(lpddsback, SCREEN_COLOR_KEY, nullptr);
		}

		//��������ü�����
		//��������ڱ��������
		//����ģʽ�£�Ҳ��˵������������������� �ü���
		rtClip.left = 0;
		rtClip.right = width - 1;
		rtClip.top = 0;
		rtClip.bottom = height - 1;

		//�ü�Ӧ�ù������󱸻�����,�����治��Ҫ
		//�ü�����ú�������ͨ��С

		RECT screen_rect = { 0, 0, width, height };
		lpddclipper = SurfaceAttachClipper(lpddsback, 1, &screen_rect);


		//windows �Ĳü����Ƚ�����
		//���ĺ����Ҳ�����˲ü�����ǰ����ҲҪ��ǰ������ƶ�
		if (bWindowed)
		{

			if (FAILED(lpdd7->CreateClipper(0, &lpddclipperwin, nullptr)))
			{
				DEBUG_INFO(ERR, TEXT("Create Clipper Failed!"));
				return false;
			}
			//���úú󣬻��Զ����ݴ������Զ��ü�
			if (FAILED(lpddclipperwin->SetHWnd(0, hwnd)))
			{
				DEBUG_INFO(ERR, TEXT("Connect Clip With Window Failed!"));
				return false;
			}
			//������������
			//���˵������back surface ����һ�����ڴ�С�Ĳü������������Ļ��ԭ�㣩
			//��bltʱ�������Զ��ƶ������ڴ�
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

			//�ͷ����е�COM���
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
		DDBLTFX ddbltfx;//��������blt�Ĳ�����Ϣ

		INIT_STRUCT(ddbltfx);

		//����������ɫ
		ddbltfx.dwFillColor = color;

		//����blit�����
		lpddsurface->Blt(client, nullptr, nullptr, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
	}
	//�Լ��������������������Ǻ󱸻���
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

		//�������ɫ�ʿռ䣬��Ҫ��־ SPACE
		//�������� alpha ���Ӳ�����ɫ�ʵ���
		//Ŀ��ɫ�ʼ����൱��դ��
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
		//��������ſ���
		if (!global_bWindowed)//full screen ����ʹ��ҳ�潻������
		{
			//nullptr �Ǻ�����������ĺ󱸻���
			//Ҳ��ָ������һ������
			//Ĭ����1�δ�ֱ��̣����ԣ�| DDFLIP_INTERVAL2) 2��
			//ʲô��������ô��
			/*while (FAILED(lpddsprimary->Flip(nullptr, DDFLIP_WAIT)))
				continue;*/
				/*lpddsprimary->Flip(nullptr, DDFLIP_WAIT);*/


					//�����������Ǳ���Ĳü�������ǰ�����޹أ�ֱ�� fast
					/*RECT source_rect = { 0, 0, width, height };
					lpddsprimary->BltFast(0, 0, lpddsback, &source_rect, DDBLTFAST_WAIT);*/
					//-----------�ѿ���û blt Fast ����


				//��֪��Ϊʲô���ұ߻���һ��ϸϸ����ɫ ����� nullptr
				//���������Ҳ�Ƕ��һ�ٰ�����Ϊ�����õı����ȣ��ü���ȣ��Լ�
				//���ߵ������Ⱦ�����ô��ѽ���治֪������ϵ���Щ�����Ķ�����
				//RECT source_rect = { min_clip_x, min_clip_y, max_clip_x, max_clip_y };
			lpddsprimary->Blt(nullptr, lpddsback, nullptr, DDBLT_WAIT, nullptr);

		}
		else
		{
			//Windows mode ����blt
			RECT dest_rect;
			POINT point = { 0, 0 };
			//��ÿͻ������ڵ�λ��
			ClientToScreen(global_hwnd, &point);
			//���󱸻�������ת��
			dest_rect.left = point.x;
			dest_rect.top = point.y;

			dest_rect.right = dest_rect.left + global_width;
			dest_rect.bottom = dest_rect.top + global_height;
#if _LA_DEBUG_ON_
			//�ü����������棬�� blt
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
		//ʹ��ϵͳ�ȴ���ֱ����һ����ֱ�հ����ڿ�ʼ������դ�����ײ�ʱ��
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

