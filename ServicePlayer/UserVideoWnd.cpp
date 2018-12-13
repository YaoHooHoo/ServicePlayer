///////////////////////////////////////////////////////////////////////////
//		我的视频显示窗口类实现文件 UserVideoWnd.cpp
//		2016-8-3 Bowen.hu
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UserVideoWnd.h"
#include "resource.h"

extern IShowLogToWnd *g_pIConfirmToWnd;
////////////////////////////////////////////////////////////////////////////
// CMyVideoWnd
IMPLEMENT_DYNAMIC(CUserVideoWnd, CVideoWnd)
BEGIN_MESSAGE_MAP(CUserVideoWnd, CVideoWnd)
	ON_MESSAGE(WM_GL_RENDER_ID,OnOpenGLRender)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

CUserVideoWnd::CUserVideoWnd()
{
	m_wVideoCX=0;
	m_wVideoCY=0;
	m_nEventCX=0;
	m_nEventCY=0;
	m_nOffsetX=0;
	m_nOffsetY=0;
	m_nWndCX=0;
	m_nWndCY=0;
	m_bReadering=FALSE;
	m_bMouseKeyCtrl=FALSE;
	m_bDevType=CT_USER;
	m_pISnapPlayManage=NULL;
	m_pISendMouseEvent=NULL;
}

CUserVideoWnd::~CUserVideoWnd()
{
}

//初始化视频窗口
bool CUserVideoWnd::InitUserVideoWnd(ISendMouseEvent *pISendMouseEvent)
{
	m_pISendMouseEvent=pISendMouseEvent;

	m_bMouseKeyCtrl=g_GlobalData.bIsHaveMouseCtrl;

	if(m_SnapPlayManageHelper.CreateInstance()==false)
	{
		g_pILogOutput->OutputLogToFile(m_SnapPlayManageHelper.GetErrorMessage());
		return false;
	}
	m_pISnapPlayManage=m_SnapPlayManageHelper.GetInterface();

	return true;
}

//绘制信息
void CUserVideoWnd::OnDrawUserInfo(CDC *pMemDC,int cx,int cy)
{
	if(m_bReadering)
		return;

	RECT txtRc={0};
	txtRc.top=cy/2-15;
	txtRc.bottom=txtRc.top+30;
	txtRc.right=cx;
	txtRc.bottom=cy;
	TCHAR szDefText[]=TEXT("视频没有连接");
	pMemDC->SetTextColor(MID_BG_COLOR);
	pMemDC->DrawText(szDefText,-1,&txtRc,DT_CENTER|DT_VCENTER);
}

//窗口创建消息
int CUserVideoWnd::OnSubCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(m_pISnapPlayManage!=NULL)
	{
		HWND hWnd=GetSafeHwnd();
		m_pISnapPlayManage->SetOpenglDrawFlag(g_GlobalData.bOpenglDraw);	//不用OpenGL
		m_pISnapPlayManage->InitSnapPlayManage(hWnd,SST_H264,hWnd,WM_GL_RENDER_ID);
	}

	return 0;
}

//窗口大小
void CUserVideoWnd::OnSubSize(UINT nType,int cx,int cy)
{
	//if(cx==0 || cy==0) return;

	if(m_bReadering)
	{
		m_pISnapPlayManage->ResetRenderWnd(m_hWnd);
	}
}

//开始播放视频
void CUserVideoWnd::StartPlayVideo()
{
	if(m_pISnapPlayManage!=NULL)
		m_pISnapPlayManage->StartPlayService();
}

//停止播放视频
void CUserVideoWnd::StopPlayVideo()
{
	if(m_pISnapPlayManage!=NULL)
		m_pISnapPlayManage->StopPlayService();
}

//设置H264SpsPps数据
void CUserVideoWnd::SetH264SpsPpsData(const BYTE *pSpsPps,WORD wDataSize)
{
	if(pSpsPps==NULL) return;

	WORD wSpsSize=0;
	WORD wPpsSize=0;
	BYTE *pSpsData=NULL;
	BYTE *pPpsData=NULL;
	BYTE *pData=(BYTE *)pSpsPps;
	//00 00 00 01
	if(pData[0]==0 && pData[1]==0 && pData[2]==0 && pData[3]==1)
	{
		pData+=10;
		wSpsSize=pData[0];
		pSpsData=(BYTE *)pData+1;
		pPpsData=pSpsData+wSpsSize+2;
		wPpsSize=pPpsData[0];
		pPpsData+=1;
		m_bDevType=CT_USER;		//PC端
	}
	else
	{
		wSpsSize=pData[0];
		pData++;
		pSpsData=pData;
		pData+=wSpsSize;
		wPpsSize=pData[0];
		pData++;
		pPpsData=pData;
		m_bDevType=CT_USER_IOS;		//移动端
	}
	if(wSpsSize==0 || wSpsSize>256) return;
	if(wPpsSize==0 || wPpsSize>128) return;
	if(m_pISnapPlayManage!=NULL)
	{
		m_pISnapPlayManage->SetSpsPpsData(pSpsData,wSpsSize,pPpsData,wPpsSize);
		DWORD dwVideoSize=m_pISnapPlayManage->GetVideoSize();
		m_wVideoCX=LOWORD(dwVideoSize);
		m_wVideoCY=HIWORD(dwVideoSize);
		CountEventWndSize();
	}
}

//获取事件区大小
void CUserVideoWnd::GetEventAreaSize(int &nOffsetX,int &nOffsetY,int &nVideoCX,int &nWidthCY,int &nWndCX,int &nWndCY)
{
	nVideoCX=m_nEventCX;
	nWidthCY=m_nEventCY;
	nOffsetX=m_nOffsetX;
	nOffsetY=m_nOffsetY;
	nWndCX=m_nWndCX;
	nWndCY=m_nWndCY;
}

//计算事件窗口大小
void CUserVideoWnd::CountEventWndSize()
{
	if(m_wVideoCX==0 || m_wVideoCY==0)
		return;

	RECT wndRc={0};
	::GetClientRect(m_hWnd,&wndRc);
	int wndCX=wndRc.right-wndRc.left;
	int wndCY=wndRc.bottom-wndRc.top;
	if(wndCX==0 || wndCY==0) return;
	m_nWndCX=wndCX;
	m_nWndCY=wndCY;
	int nVideoCX=wndCX;
	int nVideoCY=wndCY;
		
	int xOffset=0,yOffset=0;
	FLOAT imgRate=(FLOAT)m_wVideoCX/m_wVideoCY;
	FLOAT wndRate=(FLOAT)wndCX/wndCY;
	if(wndRate>imgRate)
	{
		nVideoCX=(int)(wndCY*imgRate);
		xOffset=(wndCX-nVideoCX)/2;
	}
	else if(wndRate<imgRate)
	{
		nVideoCY=(int)(wndCX/imgRate);
		yOffset=(wndCY-nVideoCY)/2;
	}

	m_nEventCX=nVideoCX;
	m_nEventCY=nVideoCY;
	m_nOffsetX=xOffset;
	m_nOffsetY=yOffset;
}

//播放视频数据
void CUserVideoWnd::PlayVideoData(const BYTE *pVideoData,DWORD dwDataSize)
{
	if(m_pISnapPlayManage!=NULL)
		m_pISnapPlayManage->PlayOneFrame(pVideoData,dwDataSize);
}

//OpenGL渲染消息
LRESULT CUserVideoWnd::OnOpenGLRender(WPARAM wParam,LPARAM lParam)
{
	if(m_pISnapPlayManage!=NULL)
	{
		m_pISnapPlayManage->OnOpenglRender(lParam);
		m_bReadering=TRUE;
	}

	return 0;
}

//客户退出通知
void CUserVideoWnd::CustomExitNotify()
{
	m_bReadering=FALSE;
	InvalidateRect(NULL,FALSE);
	g_pIConfirmToWnd->OutputLogToWnd(TEXT("等待客户接入..."));
}

//鼠标右键按下消息
void CUserVideoWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_bMouseKeyCtrl && m_bDevType==CT_USER)
	{
		m_pISendMouseEvent->SendMouseEventData(WM_LBUTTONDOWN,point.x,point.y);
	}

	__super::OnLButtonDown(nFlags,point);
}

//鼠标右键弹起消息
void CUserVideoWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bMouseKeyCtrl && m_bDevType==CT_USER)
	{
		m_pISendMouseEvent->SendMouseEventData(WM_LBUTTONUP,point.x,point.y);

		SetFocus();
	}
	__super::OnLButtonUp(nFlags,point);
}

//鼠标移动消息
void CUserVideoWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bMouseKeyCtrl && m_bDevType==CT_USER)
		m_pISendMouseEvent->SendMouseEventData(WM_MOUSEMOVE,point.x,point.y);
	
}

//鼠标右键按下消息
void CUserVideoWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	if(m_bMouseKeyCtrl && m_bDevType==CT_USER)
		m_pISendMouseEvent->SendMouseEventData(WM_RBUTTONDOWN,point.x,point.y);
}

//鼠标右键按下消息
void CUserVideoWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	if(m_bMouseKeyCtrl && m_bDevType==CT_USER)
		m_pISendMouseEvent->SendMouseEventData(WM_RBUTTONUP,point.x,point.y);
}

//鼠标左键双击消息
void CUserVideoWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if(m_bMouseKeyCtrl && m_bDevType==CT_USER)
		m_pISendMouseEvent->SendMouseEventData(WM_LBUTTONDBLCLK,point.x,point.y);
}

