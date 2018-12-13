///////////////////////////////////////////////////////////////////////////////////////
//			//皮肤图片处理类头文件 SkinImage.h
//			hubo	2013-2-15
///////////////////////////////////////////////////////////////////////////////////////
#ifndef __DEIRLYM_HUBO_SKIN_IMAGE_HEAD_FILE__
#define __DEIRLYM_HUBO_SKIN_IMAGE_HEAD_FILE__

#include <GdiPlus.h>
#include <AtlImage.h>
using namespace Gdiplus;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
typedef Gdiplus::Image	CSkinImgPlus;
typedef Gdiplus::Bitmap	CSinkBitmap;			//位图皮肤图片类
typedef Gdiplus::Font	CGdiPlusFont;
 
////////////////////////////////////////////
//从资源加载图片
CSkinImgPlus *LoadImageFromRes(DWORD dwResID,HINSTANCE hInstance=NULL);
CSinkBitmap *LoadBitmapFromRes(DWORD dwResID,HINSTANCE hInstance=NULL);
CSkinImgPlus *LoadImageFromMem(void *pImgData,DWORD dwDataSize);

///////////////////////////////////////////////////////////////////////////
void CombineRgbByPos(HRGN wndRgn,int x,int y);
void MyFillSolidRect(HDC hDC,LPCRECT lpRect, COLORREF clr);
void MyFillSolidRect(HDC hDC,int x, int y, int cx, int cy, COLORREF clr);
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

/////////////////////////////////////////////////////////////////////////////
class CRoundRectPath: public GraphicsPath  
{            
public:  
    CRoundRectPath();  
    CRoundRectPath(INT x, INT y, INT width, INT height, INT cornerX, INT cornerY);  
  
public:  
    void AddRoundRect(INT x, INT y, INT width, INT height, INT cornerX, INT cornerY);  
};


#endif //__DEIRLYM_HUBO_SKIN_IMAGE_HEAD_FILE__
