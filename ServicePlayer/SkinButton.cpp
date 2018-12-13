///////////////////////////////////////////////////////////////////////////
//		ͼƬ��ťʵ����  SkinButton.cpp
////////////////////////////////////////////////////////////////////////////
#include "Stdafx.h"
#include "Resource.h"
#include "SkinButton.h"
#include "localdef.h"

#define BTN_FRAME_NUM		4				//��ť֡��
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CSkinButton, CButton)

BEGIN_MESSAGE_MAP(CSkinButton, CButton)
	ON_WM_CREATE()
	//ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	//ON_WM_SETCURSOR()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
//�궨��
#define DEF_TEXT_COLOR				RGB(200,200,200)								//Ĭ����ɫ
//////////////////////////////////////////////////////////////////////////
//���캯��
CSkinButton::CSkinButton(int nStateNum):m_nStatusNum(nStateNum)
{
	m_dwBitmapID=0;
	m_hIcon=NULL;
	m_bExpand=true;
	m_bHovering=false;
	m_bTransparent=false;
	m_bButtonDown=false;
	m_pIMouseEvent=NULL;
	m_crTextColor=RGB(0,0,0);
	m_pImageBack=NULL;
	m_pDrawImage=NULL;
	m_nWidth=0;
	m_nHeight=0;
	m_nFontSize=12;
	lstrcpy(m_szFontName,TEXT("����"));
}

//��������
CSkinButton::~CSkinButton()
{
	if(m_pImageBack!=NULL)
	{
		delete m_pImageBack;
		m_pImageBack=NULL;
	}
}

//���󸽼ӵ����д���
void CSkinButton::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	SetButtonStyle(GetButtonStyle()|BS_OWNERDRAW);
	//::SendMessage(m_hWnd,WM_SETFONT,(WPARAM)m_DefaultFont,TRUE);
	return;
}

//������Ϣ
int CSkinButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	SetButtonStyle(GetButtonStyle()|BS_OWNERDRAW);

	return 0;
}

//������ɫ
bool CSkinButton::SetTextColor(COLORREF crTextColor)
{
	m_crTextColor=crTextColor;
	if (GetSafeHwnd()) PaintParent();//Invalidate(FALSE);
	return true;
}

//��������
bool CSkinButton::SetTextFont(int nFontSize,LPCTSTR lpszFontName)
{
	m_nFontSize=nFontSize;
	if(lpszFontName!=NULL)
	{
		lstrcpy(m_szFontName,lpszFontName);
	}
	return true;
}

//����ͼ��
bool CSkinButton::SetButtonIcon(HICON hIcon)
{
	m_hIcon=hIcon;
	if (GetSafeHwnd()) PaintParent();//Invalidate(FALSE);
	return true;
}

//����λͼ
bool CSkinButton::SetButtonImage(LPCTSTR pszFileName,bool bExpandImage,bool bTransparent)
{
	//Ч�����
	ASSERT(pszFileName!=NULL);
	if(pszFileName==NULL) return false;

	//����λͼ
	m_bExpand=bExpandImage;
	m_bTransparent=bTransparent;
	
	if(m_pImageBack!=NULL)
	{
		delete m_pImageBack;
		m_pImageBack=NULL;
		m_pDrawImage=NULL;
	}
	const int WBUFFSIZE=MAX_PATH;
	WCHAR szFileName[WBUFFSIZE]=L"";
	#ifndef _UNICODE
	::MultiByteToWideChar(0,0,pszFileName,-1,szFileName,WBUFFSIZE-1);
	#else
	lstrcpy(szFileName,pszFileName);
	#endif
	m_pImageBack=CSinkBitmap::FromFile(szFileName);
	if(m_pImageBack==NULL) return false;

	m_pDrawImage=m_pImageBack;
	//����λ��
	if (m_bExpand==false) FixButtonSize();

	if(GetSafeHwnd()) PaintParent();//Invalidate(FALSE);

	return true;
}

//����λͼ
bool CSkinButton::SetButtonImage(UINT uBitmapID, HINSTANCE hInstance, bool bExpandImage,bool bTransparent)
{
	//����λͼ
	ASSERT(uBitmapID!=0);
	if (uBitmapID==0) return false;
	if(m_dwBitmapID==uBitmapID) return true;

	m_bExpand=bExpandImage;
	m_bTransparent=bTransparent;
	
	m_pImageBack=LoadBitmapFromRes(uBitmapID,hInstance);
	if(m_pImageBack==NULL)
		return false;

	m_pDrawImage=m_pImageBack;
	m_dwBitmapID=uBitmapID;

	//����λ��
	if (m_bExpand==false) FixButtonSize();
	
	//��������
	//if(m_bTransparent==true) RectifyRgn();    

	if (GetSafeHwnd()) PaintParent();//Invalidate(FALSE);

	return true;
}

//Ĭ�ϰ�ť
void CSkinButton::SetDefaultButton(bool bResetIcon, bool bResetColor, bool bResetImage)
{
	//�ָ�Ĭ��
	if (bResetImage) 
	{
		m_bExpand=true;
	}
	if (bResetIcon)	m_hIcon=NULL;
	if (bResetColor) m_crTextColor=RGB(0,0,0);

	//ˢ�½���
	if (GetSafeHwnd()) PaintParent();//Invalidate(FALSE);

	return;
}

//����λ��
bool CSkinButton::FixButtonSize()
{
 	if (m_pDrawImage!=NULL && GetSafeHwnd())
	{
		int nImgWidth=m_pDrawImage->GetWidth();
		int nImgHeight=m_pDrawImage->GetHeight();
		m_nWidth=nImgWidth/BTN_FRAME_NUM;
		m_nHeight=nImgHeight;
		SetWindowPos(NULL,0,0,nImgWidth/m_nStatusNum,nImgHeight,SWP_NOMOVE);
		return true;
	}
	return false;
}

//������ť��������
void CSkinButton::RectifyRgn()
{
	RECT rcButton;
	::GetClientRect(m_hWnd,&rcButton);
	//����ͼ������
	if(m_pDrawImage!=NULL && GetSafeHwnd())
	{
		int nWidth=RECTWIDTH(rcButton);
		int nHeight=RECTHEIGHT(rcButton);

		//Gdiplus::PixelFormat pf=PixelFormat32bppARGB;

		HRGN wndRgn,tempRgn;
		wndRgn=::CreateRectRgn(0,0,nWidth,nHeight);

		Color tmpCol;
		for(int x=0;x<nWidth;x++)
		{
			for(int y=0;y<nHeight;y++)
			{
				m_pDrawImage->GetPixel(x,y,&tmpCol);
				if(tmpCol.GetAlpha()==0)
				{
					tempRgn=::CreateRectRgn(x,y,x+1,y+1);
					::CombineRgn(wndRgn,wndRgn,tempRgn,RGN_XOR);
					::DeleteObject(tempRgn);
				}
			}
		}
		::SetWindowRgn(m_hWnd,wndRgn,TRUE);
		::DeleteObject(wndRgn);
	}
}

//��������
void CSkinButton::SetButtonFont(int nFontSize,LPCTSTR lpszFontName)
{
	m_nFontSize=nFontSize;
	lstrcpy(m_szFontName,lpszFontName);
}

//����ƶ���Ϣ
void CSkinButton::OnMouseMove(UINT nFlags, CPoint point)
{
	//TRACE(TEXT("CSkinButton::OnMouseMove[%d--%d]\n"),point.x,point.y);
	if(m_pIMouseEvent==NULL)
	{
		__super::OnMouseMove(nFlags,point);
		return;
	}

	if (m_bHovering==false)
	{
		//ע����Ϣ
		m_bHovering=true;
		Invalidate(FALSE);
		
		TRACKMOUSEEVENT TrackMouseEvent;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.hwndTrack=GetSafeHwnd();
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		_TrackMouseEvent(&TrackMouseEvent);
		
		//PaintParent();
	}
	__super::OnMouseMove(nFlags, point);
}

//����뿪��Ϣ
LRESULT CSkinButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bHovering=false;
	PaintParent();
	return 0;
}

//��������
BOOL CSkinButton::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	return TRUE;
}

//���ù��
BOOL CSkinButton::OnSetCursor(CWnd * pWnd,UINT nHitTest,UINT message)
{
	/*HINSTANCE hInstance=AfxGetApp()->m_hInstance;
	HCURSOR hCursor=::LoadCursor(hInstance,MAKEINTRESOURCE(IDC_HAND_CUR));
	::SetCursor(hCursor);*/

	return TRUE;
}

//����滭����
void CSkinButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//���� DC
	HDC hDC=lpDrawItemStruct->hDC;
	if(hDC==NULL) return;

	//�������
	bool bDisable=((lpDrawItemStruct->itemState&ODS_DISABLED)!=0);
	bool bButtonDown=(m_bButtonDown || ((lpDrawItemStruct->itemState&ODS_SELECTED)!=0));
	bool bFocus=((lpDrawItemStruct->itemState&ODS_FOCUS)!=0);
	
	DrawButton(hDC,bDisable,bButtonDown,bFocus);
}

//���ư�ť
void CSkinButton::DrawButton(HDC hDC,bool bDisable,bool bButtonDown,bool bFocus)
{
	//TRACE(TEXT("------DrawItem[%x]\n"),this);
	//�������
	RECT ClientRect;
	::GetClientRect(m_hWnd,&ClientRect);
	int cx=RECTWIDTH(ClientRect);
	int cy=RECTHEIGHT(ClientRect);
	::SetBkMode(hDC,TRANSPARENT);

	Graphics graphics(hDC);

	if(m_pDrawImage!=NULL)
	{
		//����λͼλ��
		int iDrawPos=0;
		int iPartWidth=m_pDrawImage->GetWidth()/m_nStatusNum;
		int iImgHeight=m_pDrawImage->GetHeight();
		if (bDisable) iDrawPos=iPartWidth*3;
		else if (bButtonDown) iDrawPos=iPartWidth*2;
		else if (m_bHovering) iDrawPos=iPartWidth;
		else if (bFocus) iDrawPos=0;

		CSinkBitmap *pBgImage=m_pDrawImage;		
		//�滭����ͼ
		if (m_bExpand==false)
		{
			graphics.DrawImage(pBgImage,0,0,iDrawPos,0,iPartWidth,iImgHeight,UnitPixel);	
		}
		else
		{
			Gdiplus::RectF rcf;
			rcf.X=(REAL)ClientRect.left;
			rcf.Y=(REAL)ClientRect.top;
			rcf.Width=(REAL)cx;
			rcf.Height=(REAL)cy;
			graphics.DrawImage(pBgImage,rcf,(REAL)iDrawPos,(REAL)0,(REAL)iPartWidth,(REAL)iImgHeight,UnitPixel);
			//ImageHandle->StretchBlt(hDC,0,0,nClientWidth,nClientHeight,iDrawPos,0,iPartWidth,ImageHandle->GetHeight(),SRCCOPY);
		}
	}
	else
	{
		//�滭Ĭ�Ͻ���		
		COLORREF tmpColor=GetSysColor(COLOR_WINDOWFRAME);
		if(bButtonDown) tmpColor=GetSysColor(COLOR_3DHILIGHT);
		BYTE r=(BYTE)GetRValue(tmpColor);
		BYTE g=(BYTE)GetGValue(tmpColor);
		BYTE b=(BYTE)GetBValue(tmpColor);
		Color cr=Color(r,g,b);
		Gdiplus::SolidBrush brush(cr);
		graphics.FillRectangle(&brush,0,0,cx,cy);
	}
		
	//�滭ͼ��
	if (m_hIcon)
	{
		DrawIconEx(hDC,ClientRect.left+6,ClientRect.top+(cy-16)/2+1,m_hIcon,16,16,0,NULL,DI_NORMAL);
		ClientRect.left+=22;
	}

	//��ȡ����
	CString strText;
	int nLen = GetWindowTextLength();
	
	//�滭����
	if(nLen>0)
	{
		GetWindowText(strText.GetBufferSetLength(nLen), nLen+1);
		ClientRect.top+=3;
		ClientRect.left+=1;
		ClientRect.bottom-=1;
		const int WBUFFSIZE=64;
		WCHAR szFontName[WBUFFSIZE]=L"";
		#ifndef _UNICODE
		::MultiByteToWideChar(0,0,m_szFontName,-1,szFontName,WBUFFSIZE-1);
		#else
		lstrcpy(szFontName,m_szFontName);
		#endif
		Gdiplus::Font myFont(szFontName,(REAL)m_nFontSize);
		ClientRect.top+=2;
		RectF layoutRect((REAL)ClientRect.left,(REAL)ClientRect.top,
			(REAL)(ClientRect.right-ClientRect.left),(REAL)(ClientRect.bottom-ClientRect.top));
						
		StringFormat format;
		format.SetAlignment(StringAlignmentCenter);
		format.SetLineAlignment(StringAlignmentCenter);
		
		Color txtColor;
		if (bDisable)
			txtColor.SetFromCOLORREF(GetSysColor(COLOR_GRAYTEXT));
		else 
			txtColor.SetFromCOLORREF(m_crTextColor);
		SolidBrush blackBrush(txtColor);
		//graphics.DrawString(strText,strText.GetLength(),&myFont,layoutRect,&format,&blackBrush);
	}	
}

//������
BOOL CSkinButton::CheckMouseInButton()
{
	POINT point;
	RECT rect;
	::GetWindowRect(m_hWnd,&rect);
	::GetCursorPos(&point);
	return ::PtInRect(&rect,point);
}

//���Ƹ�����ָ������
void CSkinButton::PaintParent()
{	
	CWnd * pParentWnd=GetParent();
	if(pParentWnd==NULL) return;

	RECT rc;
	::GetWindowRect(m_hWnd,&rc);
	pParentWnd->ScreenToClient(&rc);
	pParentWnd->InvalidateRect(&rc,FALSE);

	//TRACE(TEXT("+++++++PaintParent [0x%x][%d,%d,%d,%d]\n"),pParentWnd,rc.left,rc.top,RECTWIDTH(rc),RECTHEIGHT(rc));
}

//��������Ϣ
void CSkinButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_pIMouseEvent==NULL)
	{
		__super::OnLButtonDown(nFlags,point);
		return;
	}

	m_bButtonDown=true;
	::SetCapture(m_hWnd);
	Invalidate(FALSE);
}

//��������Ϣ
void CSkinButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_pIMouseEvent==NULL)
	{
		__super::OnLButtonUp(nFlags,point);
		return;
	}

	m_bButtonDown=false;
	::ReleaseCapture();

	if(!CheckMouseInButton())
		return;
	PaintParent();
	UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
	if(m_pIMouseEvent!=NULL)
		m_pIMouseEvent->OnEventLButtonUp(uButtonID,nFlags,point);
}

//����Ҽ���Ϣ
void CSkinButton::OnRButtonDown(UINT nFlags, CPoint point)
{
	if(m_pIMouseEvent==NULL)
	{
		__super::OnRButtonDown(nFlags,point);
		return;
	}
	::SetCapture(m_hWnd);
	Invalidate(FALSE);
}

//����Ҽ���Ϣ
void CSkinButton::OnRButtonUp(UINT nFlags, CPoint point)
{
	if(m_pIMouseEvent==NULL)
	{
		__super::OnRButtonUp(nFlags,point);
		return;
	}

	::ReleaseCapture();
	if(!CheckMouseInButton())
		return;
	PaintParent();
	UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
	m_pIMouseEvent->OnEventRButtonUp(uButtonID,nFlags,point);
}

//////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CStatusButton,CFlashButton)

BEGIN_MESSAGE_MAP(CStatusButton,CFlashButton)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

CStatusButton::CStatusButton()
{
	m_bAutoSwitch=TRUE;
	m_bSecondImg=FALSE;
	m_pImageBack1=NULL;
	m_pImageBack2=NULL;
}

CStatusButton::~CStatusButton()
{
	if(m_pImageBack1!=NULL)
	{
		delete m_pImageBack1;
		m_pImageBack1=NULL;
	}
	if(m_pImageBack2!=NULL)
	{
		delete m_pImageBack2;
		m_pImageBack2=NULL;
	}
}

//����λͼ
bool CStatusButton::SetButtonStatusImage(UINT uImgID1,UINT uImgID2,HINSTANCE hInstance,bool bExpandImage,bool bTransparent)
{
	//����λͼ
	ASSERT(uImgID1!=0);
	if (uImgID1==0) return false;
	ASSERT(uImgID2!=0);
	if (uImgID2==0) return false;
	m_bExpand=bExpandImage;
	m_bTransparent=bTransparent;

	m_pImageBack1=LoadBitmapFromRes(uImgID1,hInstance);
	if(m_pImageBack1==NULL)
		return false;

	m_pImageBack2=LoadBitmapFromRes(uImgID2,hInstance);
	if(m_pImageBack2==NULL)
		return false;

	m_bSecondImg=FALSE;

	m_pDrawImage=m_pImageBack1;

	//����λ��
	if (m_bExpand==false) FixButtonSize(); 

	if (GetSafeHwnd()) PaintParent();//Invalidate(FALSE);

	return true;
}

//��������Ϣ
void CStatusButton::OnLButtonUp(UINT nFlags, CPoint point)
{	
	if(m_bAutoSwitch)
	{
		m_bSecondImg=!m_bSecondImg;
		if(m_bSecondImg)
			m_pDrawImage=m_pImageBack2;
		else
			m_pDrawImage=m_pImageBack1;
	}

	__super::OnLButtonUp(nFlags,point);
}

//���ð�ťͼƬ״̬
void CStatusButton::SetSecondImage(BOOL bIsSecond)
{
	if(m_bSecondImg==bIsSecond) return;

	m_bSecondImg=bIsSecond;
	if(m_bSecondImg)
		m_pDrawImage=m_pImageBack2;
	else
		m_pDrawImage=m_pImageBack1;

	::InvalidateRect(m_hWnd,NULL,FALSE);
}

//////////////////////////////////////////////////////////////////////////////////////
#define IDM_FLASH_BTN		0x9999				//��ť��˸��ʱ��ID
#define FLASH_TIME_SPACE	100					//��˸���
#define FLASH_COUNT			40					//��˸����

IMPLEMENT_DYNAMIC(CFlashButton,CSkinButton)

BEGIN_MESSAGE_MAP(CFlashButton,CSkinButton)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

CFlashButton::CFlashButton()
{
	m_pHighLigthImg=NULL;
	m_pSrcBgImage=NULL;
	m_bBtnFlash=false;
	m_bFlashing=false;
	m_nLastCount=0;
	m_nFlashCount=FLASH_COUNT;
	m_nFlashRate=FLASH_TIME_SPACE;
	
}

CFlashButton::~CFlashButton()
{
	if(m_pHighLigthImg!=NULL)
	{
		delete m_pHighLigthImg;
	}
}

//������˸����(nFlashRate=��/s)
void CFlashButton::SetFlashParam(int nFlashCount,int nFlashRate)
{
	if(nFlashCount>0)
	{
		m_nFlashCount=nFlashCount;
	}
	if(nFlashRate>0)
	{
		m_nFlashRate=1000/nFlashRate;		
	}
}

//��˸��ť
void CFlashButton::FlashButtonImage()
{
	if(m_pDrawImage==NULL) return;
	if(m_bFlashing) return;
	if(m_pHighLigthImg==NULL)
	{
		int cx=m_pDrawImage->GetWidth();
		int cy=m_pDrawImage->GetHeight();
		HDC hDC=::GetDC(m_hWnd);
		HDC hMemDC=::CreateCompatibleDC(hDC);
		HBITMAP hMemBmp=::CreateCompatibleBitmap(hDC,cx,cy);
		HBITMAP hOldBmp=(HBITMAP)::SelectObject(hMemDC,hMemBmp);
		Gdiplus::ImageAttributes imgAttr;		
		/*Gdiplus:: ColorMatrix HotMat = 
		{1.05f, 0.00f, 0.00f, 0.00f,0.00f,
		0.00f, 1.05f, 0.00f, 0.00f, 0.00f,
		0.00f, 0.00f, 1.05f, 0.00f, 0.00f,
		0.00f, 0.00f, 0.00f, 1.00f, 0.00f,
		0.05f, 0.05f, 0.05f, 0.00f, 1.00f};
		imgAttr.SetColorMatrix(&HotMat);*/
		imgAttr.SetGamma(1/2.2f);
		Graphics graphics(hMemDC);
		Gdiplus::RectF drawRc(0.0,0.0,(REAL)cx,(REAL)cy);
		graphics.DrawImage(m_pDrawImage,drawRc,(REAL)0,(REAL)0,(REAL)cx,(REAL)cy,Gdiplus::UnitPixel,&imgAttr);
		::SelectObject(hMemDC,hOldBmp);
		m_pHighLigthImg=Gdiplus::Bitmap::FromHBITMAP(hMemBmp,NULL);
		::DeleteObject(hMemBmp);
		::DeleteDC(hMemDC);
		::ReleaseDC(m_hWnd,hDC);
		if(m_pHighLigthImg==NULL) return;
	}

	m_pSrcBgImage=m_pDrawImage;
	m_nLastCount=m_nFlashCount;
	::SetTimer(m_hWnd,IDM_FLASH_BTN,m_nFlashRate,NULL);
	m_bFlashing=true;
}
	
//��ʱ����Ϣ
void CFlashButton::OnTimer(UINT_PTR nEventID)
{
	if(m_nLastCount<=0)
	{
		m_bFlashing=false;
		::KillTimer(m_hWnd,IDM_FLASH_BTN);
		m_pDrawImage=m_pSrcBgImage;		
		return;
	}
	m_nLastCount--;
	m_bBtnFlash=!m_bBtnFlash;
	m_pDrawImage=m_bBtnFlash?m_pHighLigthImg:m_pSrcBgImage;	
	::InvalidateRect(m_hWnd,NULL,FALSE);
}

//����ƶ�
void CFlashButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bFlashing)
	{
		m_bFlashing=false;
		::KillTimer(m_hWnd,IDM_FLASH_BTN);
		m_pDrawImage=m_pSrcBgImage;
	}

	__super::OnMouseMove(nFlags,point);
}

/////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CSelectButton::CSelectButton()
{
	m_bSelected=false;
}

//��������
CSelectButton::~CSelectButton()
{
}

//����״̬
void CSelectButton::SetSelected(bool bSelected)
{
	bool bOld=m_bSelected;
	if(bOld!=bSelected)
	{
		m_bSelected=bSelected;
		::InvalidateRect(m_hWnd,NULL,FALSE);
	}
}

//���ư�ť
void CSelectButton::DrawButton(HDC hDC,bool bDisable,bool bButtonDown,bool bFocus)
{
	__super::DrawButton(hDC,bDisable,bButtonDown,bFocus);
	if(m_bSelected)
	{
		RECT btnRc;
		GetClientRect(&btnRc);
		::DrawFocusRect(hDC,&btnRc);

		POINT pt;
		COLORREF col = RGB(28,28,28);
		HPEN hPen=::CreatePen(PS_DOT,1,col);
		HPEN hOldPen=(HPEN)::SelectObject(hDC,hPen);
		::MoveToEx(hDC,0,0,&pt);
		::LineTo(hDC,btnRc.right,btnRc.top);
		::LineTo(hDC,btnRc.right-1,btnRc.bottom);
		::LineTo(hDC,btnRc.left,btnRc.bottom-1);
		::LineTo(hDC,btnRc.left,btnRc.top);
		::SelectObject(hDC,hOldPen);
		::DeleteObject(hPen);
	}
}