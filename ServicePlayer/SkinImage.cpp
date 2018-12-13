///////////////////////////////////////////////////////////////////////////////////////
//			//皮肤图片处理类实现文件 SkinImage.cpp
//			hubo	2013-2-15
///////////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "SkinImage.h"

extern HINSTANCE g_hInstance;
/////////////////////////////////////////////////////////////////////////////////////////
class CBuildStream
{
public:
	IStream				*m_pIStream;				//流接口

private:
	HGLOBAL				m_hMemImage;				//内存图片

public:
	CBuildStream(DWORD dwResID,HINSTANCE hInstance=NULL)
	{
		m_pIStream=NULL;
		m_hMemImage=NULL;

		HMODULE hModule=hInstance;
		if(hModule==NULL)
			hModule=g_hInstance;
		HRSRC hResour=FindResource(hModule,MAKEINTRESOURCE(dwResID),TEXT("PNG"));
		if(hResour==NULL) return;

		HGLOBAL hGlobal=LoadResource(hModule,hResour);
		if (hGlobal==NULL) return;

		DWORD dwSize=SizeofResource(hModule,hResour);
		m_hMemImage = GlobalAlloc(GMEM_FIXED, dwSize);
		if(m_hMemImage==NULL)
		{
			//清理资源
			UnlockResource(hGlobal);
			FreeResource(hGlobal);
			return;
		}
		CreateStreamOnHGlobal(m_hMemImage, FALSE, &m_pIStream);
		if(m_pIStream==NULL)
		{
			GlobalFree(m_hMemImage);
			//清理资源
			UnlockResource(hGlobal);
			FreeResource(hGlobal);
			return;
		}
		BYTE *pResMem=(BYTE *)LockResource(hGlobal);
		BYTE* pbyBmp = (BYTE *)GlobalLock(m_hMemImage);
		memcpy(pbyBmp,pResMem,dwSize);

		//清理资源
		UnlockResource(hGlobal);
		FreeResource(hGlobal);
	}
	CBuildStream(void *pImgData,DWORD dwSize)
	{
		m_pIStream=NULL;
		m_hMemImage=NULL;

		if(pImgData==NULL || dwSize==0)
			return;

		m_hMemImage = GlobalAlloc(GMEM_FIXED, dwSize);
		if(m_hMemImage==NULL)
		{
			return;
		}
		CreateStreamOnHGlobal(m_hMemImage, FALSE, &m_pIStream);
		if(m_pIStream==NULL)
		{
			GlobalFree(m_hMemImage);
			return;
		}
		BYTE* pbyBmp = (BYTE *)GlobalLock(m_hMemImage);
		memcpy(pbyBmp,pImgData,dwSize);
	}

	~CBuildStream()
	{
		if(m_pIStream!=NULL)
		{
			m_pIStream->Release();
		}
		if(m_hMemImage!=NULL)
		{
			GlobalUnlock(m_hMemImage);
			GlobalFree(m_hMemImage);
		}
	}
};

//从资源加载图片
CSkinImgPlus *LoadImageFromRes(DWORD dwResID,HINSTANCE hInstance)
{
	Image* imImage = NULL;
	
	CBuildStream bs(dwResID,hInstance);
	imImage = Image::FromStream(bs.m_pIStream,FALSE);
	return imImage;
}

CSkinImgPlus *LoadImageFromMem(void *pImgData,DWORD dwDataSize)
{
	Image* imImage = NULL;
	
	CBuildStream bs(pImgData,dwDataSize);
	imImage = Image::FromStream(bs.m_pIStream,FALSE);
	return imImage;
}

CSinkBitmap *LoadBitmapFromRes(DWORD dwResID,HINSTANCE hInstance)
{
	CSinkBitmap* imImage = NULL;
	CBuildStream bs(dwResID,hInstance);
	imImage = CSinkBitmap::FromStream(bs.m_pIStream,FALSE);

	return imImage;
}

void CombineRgbByPos(HRGN wndRgn,int x,int y)
{
	HRGN tempRgn=::CreateRectRgn(x,y,x+1,y+1);
	::CombineRgn(wndRgn,wndRgn,tempRgn,RGN_XOR);
	::DeleteObject(tempRgn);
}

void MyFillSolidRect(HDC hDC,LPCRECT lpRect, COLORREF clr)
{
	ASSERT(hDC != NULL);
	::SetBkColor(hDC, clr);
	::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
}

void MyFillSolidRect(HDC hDC,int x, int y, int cx, int cy, COLORREF clr)
{
	ASSERT(hDC != NULL);
	::SetBkColor(hDC, clr);
	RECT rect={x, y, x + cx, y + cy};
	::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
}


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

///////////////////////////////////////////////////////////////////////////////
CRoundRectPath::CRoundRectPath(void)  
    :Gdiplus::GraphicsPath()  
{        
}  

CRoundRectPath::CRoundRectPath(INT x, INT y, INT width, INT height, INT cornerX, INT cornerY)  
    :Gdiplus::GraphicsPath()  
{  
    AddRoundRect(x,y,width,height,cornerX,cornerY);  
}  
void CRoundRectPath::AddRoundRect(INT x, INT y, INT width, INT height, INT cornerX, INT cornerY)  
{  
    INT elWid = 2*cornerX;  
    INT elHei = 2*cornerY;  
  
    AddArc(x,y,elWid,elHei,180,90); // 左上角圆弧  
    AddLine(x+cornerX,y,x+width-cornerX,y); // 上边  
  
    AddArc(x+width-elWid,y, elWid,elHei,270,90); // 右上角圆弧  
    AddLine(x+width,y+cornerY, x+width,y+height-cornerY);// 右边  
  
    AddArc(x+width-elWid,y+height-elHei, elWid,elHei,0,90); // 右下角圆弧  
    AddLine(x+width-cornerX,y+height, x+cornerX,y+height); // 下边  
  
    AddArc(x,y+height-elHei, elWid,elHei,90,90);   
    AddLine(x,y+cornerY, x, y+height-cornerY);  
} 
