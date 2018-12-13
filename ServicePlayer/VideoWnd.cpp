///////////////////////////////////////////////////////////////////////////
//		��Ƶ��ʾ������ʵ���ļ� VideoWnd.cpp
//		2016-8-3 Bowen.hu
////////////////////////////////////////////////////////////////////////////
// VideoWnd.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "localdef.h"
#include "VideoWnd.h"

////////////////////////////////////////////////////////////////////////////
// CVideoWnd
IMPLEMENT_DYNAMIC(CVideoWnd, CWnd)
BEGIN_MESSAGE_MAP(CVideoWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()

CVideoWnd::CVideoWnd()
{
	m_nDevIndex=0;
	m_dwChannelID=1;

	m_txtFont14.CreateFont(14,0,0,0,400,0,0,0,0,DEFAULT_CHARSET,0,0,0,TEXT("����"));
}

CVideoWnd::~CVideoWnd()
{
}

//����ͨ��ID
void CVideoWnd::SetChannelID(DWORD dwChannelID,int nDevIndex)
{
	m_dwChannelID=dwChannelID;
	m_nDevIndex=nDevIndex;
}

//���ڴ�����Ϣ
int CVideoWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct)==-1)
		return -1;

	int nRet=OnSubCreate(lpCreateStruct);

	m_bCreated=true;
	
	return nRet;
}

//���ڴ�С��Ϣ
void CVideoWnd::OnSize(UINT nType, int cx, int cy)
{
	if(!m_bCreated) return;

	OnSubSize(nType,cx,cy);
	__super::OnSize(nType,cx,cy);
}

//������Ϣ
void CVideoWnd::OnPaint()
{
	CPaintDC dc(this);

	RECT rect;
	::GetClientRect(m_hWnd,&rect);
	int cx=RECTWIDTH(rect);
	int cy=RECTHEIGHT(rect);
	int x=0,y=30;
	CDC memDC;
	CBitmap memBmp;
	memDC.CreateCompatibleDC(&dc);
	memBmp.CreateCompatibleBitmap(&dc,cx,cy);
	CBitmap *pOldBmp=memDC.SelectObject(&memBmp);
	memDC.SetBkMode(TRANSPARENT);
	memDC.FillSolidRect(0,0,cx,cy,VIDEO_BG_COLOR);

	CFont *pOldFont=memDC.SelectObject(&m_txtFont14);
	
	OnDrawUserInfo(&memDC,cx,cy);

	memDC.SelectObject(pOldFont);

	dc.BitBlt(0,0,cx,cy,&memDC,0,0,SRCCOPY);
	memDC.SelectObject(pOldBmp);
	memBmp.DeleteObject();
	memDC.DeleteDC();
}

//������Ϣ
void CVideoWnd::OnDrawUserInfo(CDC *pMemDC,int cx,int cy)
{
	RECT txtRc={0};
	txtRc.right=cx;
	txtRc.bottom=cy;
	TCHAR szDefText[]=TEXT("û������");
	pMemDC->SetBkMode(TRANSPARENT);
	pMemDC->SetTextColor(MID_BG_COLOR);
	pMemDC->DrawText(szDefText,-1,&txtRc,DT_CENTER|DT_VCENTER);
}
