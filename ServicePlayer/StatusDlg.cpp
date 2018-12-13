////////////////////////////////////////////////////////////////////////////////////////////
//		状态对话框类实现文件 StatusDlg.cpp
////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ServicePlayer.h"
#include "StatusDlg.h"
#include "afxdialogex.h"

#define MINLISENCEDAY			5					//提醒Lisence天数
////////////////////////////////////////////////////////////////////////////////////////////
// CStatusDlg 对话框

IMPLEMENT_DYNAMIC(CStatusDlg, CDialogEx)
BEGIN_MESSAGE_MAP(CStatusDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

CStatusDlg::CStatusDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_STATUSDLG, pParent)
{
	m_bFlushing=FALSE;
	m_bCreated=FALSE;
	m_dwFlushCount=0;
	m_LogColor=LOG_NOR_COLOR;
	m_bShowLogTxt=TRUE;
	m_szLogMsg[0]=0;
	m_bIsAlpha=TRUE;
	m_cbAlphaNum=70;
	m_bDrawFull=TRUE;
	m_txtFont12.CreateFont(12,0,0,0,400,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH|FF_DONTCARE,TEXT("宋体"));
	m_gbColor=RGB(66,66,66);
	m_udwParam=0;
	m_dwTimeSpace=200;
	m_dwEndTime=30;

	m_pISendMouseEvent=NULL;
}

CStatusDlg::~CStatusDlg()
{
	m_txtFont12.DeleteObject();
}

void CStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

//初始化对话框
BOOL CStatusDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	lstrcpy(m_szLogMsg,TEXT("没有连接到流媒体服务器"));

	bInvalidLisence=g_GlobalData.bInvalidLisence;
	nLisenceDays=g_GlobalData.nLisenceDays;

	OnSubCreate();

	m_bCreated=TRUE;

	//设置透明度
	if(m_bIsAlpha)
	{
		SetWindowLong(m_hWnd,GWL_EXSTYLE,GetWindowLong(m_hWnd,GWL_EXSTYLE)|WS_EX_LAYERED);
		SetLayeredWindowAttributes(0,(BYTE)((255*(DWORD)m_cbAlphaNum)/100),LWA_ALPHA);
	}
	return TRUE;
}

//窗口大小消息
void CStatusDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType,cx,cy);

	if(!m_bCreated) return;

	m_bltRect.left=0;
	m_bltRect.right=cx;
	m_bltRect.bottom=cy;
	m_bltRect.top=0;

	OnSubSize(nType,cx,cy);
}

void CStatusDlg::OnPaint()
{
	RECT rc;
	::GetClientRect(m_hWnd,&rc);

	int cx=RECTWIDTH(rc);
	int cy=RECTHEIGHT(rc);
	int x=8;
	int y=6;
	const int CIRSIZE=12;

	CPaintDC dc(this);

	CDC memDC;
	CBitmap memBmp;
	memDC.CreateCompatibleDC(&dc);
	memBmp.CreateCompatibleBitmap(&dc,cx,cy);
	CBitmap *pOldBmp=memDC.SelectObject(&memBmp);
	memDC.FillSolidRect(0,0,cx,cy,m_gbColor);
	memDC.SetBkMode(TRANSPARENT);
	memDC.SetBkColor(RGB(0,0,0));
	DrawTitleTxt(memDC,cx,cy);		//绘制标题
	DrawLogText(memDC,cx,cy);		//绘制内容
	if(m_bDrawFull && !m_bFlushing)
		dc.BitBlt(0,0,cx,cy,&memDC,0,0,SRCCOPY);
	else
		dc.BitBlt(m_bltRect.left,m_bltRect.top,m_bltRect.right,m_bltRect.bottom,&memDC,0,0,SRCCOPY);
	memDC.SelectObject(pOldBmp);
	memBmp.DeleteObject();
	memDC.DeleteDC();
	m_bDrawFull=TRUE;
}

//绘制日志文本
void CStatusDlg::DrawLogText(CDC &memDC,int cx,int cy)
{
	if(!m_bShowLogTxt) return;

	int nTxtLen=lstrlen(m_szLogMsg);
	if(nTxtLen==0) return;

	const int TXTTOP=6;
	const int BSIZE=10;
	RECT txtRect={BSIZE,TXTTOP,cx-BSIZE,cy};
	CFont *pOldFont=memDC.SelectObject(&m_txtFont12);
	COLORREF oldColor=memDC.SetTextColor(m_LogColor);
	memDC.DrawText(m_szLogMsg,-1,&txtRect,DT_LEFT|DT_VCENTER);
	memDC.SelectObject(pOldFont);
	memDC.SetTextColor(oldColor);
}

//定时器消息
void CStatusDlg::OnTimer(UINT_PTR nTimerID)
{
	if(nTimerID==IDT_LOOP_PLAY_CW)
	{
		if(++m_dwFlushCount>m_dwEndTime)
		{
			StopFlushShowMsg();
			OnFlushFinished();
			if(g_GlobalData.bConfirming)
			{
				g_GlobalData.bConfirming=FALSE;
			}
			return;
		}
		m_bShowLogTxt=!m_bShowLogTxt;
		::InvalidateRect(m_hWnd,NULL,FALSE);
	}
}

//设置发送事件接口
bool CStatusDlg::SetSendEventInterface(ISendMouseEvent *pISendMouseEvent)
{
	m_pISendMouseEvent=pISendMouseEvent;
	return (m_pISendMouseEvent!=NULL);
}

//停止闪烁
void CStatusDlg::StopFlushShowMsg()
{
	m_bFlushing=FALSE;
	m_dwFlushCount=0;
	m_bShowLogTxt=TRUE;
	::KillTimer(m_hWnd,IDT_LOOP_PLAY_CW);
}

//输出消息到窗口
void __cdecl CStatusDlg::OutputLogToWnd(LPCTSTR lpszMsg,LOGLEVEL ll,BOOL bFlashWnd)
{
	g_pILogOutput->OutputLogToFile(lpszMsg);

	const int MAXMSG=128;
	CThreadLockHandle LockHandle(&m_LogMsgLock);
	if(m_bFlushing) return;

	if(!bInvalidLisence)
	{
		lstrcpy(m_szLogMsg,TEXT("无效Lisence,请及时申请更新,谢谢!"));
		bFlashWnd=TRUE;
	}
	else if(nLisenceDays<=MINLISENCEDAY)
	{
		mysprintf(m_szLogMsg,MAX_PATH,TEXT("Lisence还有%d天到期，请及时申请，谢谢！"),g_GlobalData.nLisenceDays);
		bFlashWnd=TRUE;
	}
	else
	{
		int nSrcLen=lstrlen(lpszMsg);
		if(nSrcLen>=MAXMSG)
		{
			WCHAR wszDstTxt[MAX_PATH]=L"";
			int nMaxWcharLen=MAX_PATH;
			WCHAR *pwDstTxt=wszDstTxt;
			int nWidLength=::MultiByteToWideChar(CP_ACP,0,lpszMsg,-1,NULL,0);
			if(nWidLength>=MAX_PATH)
			{
				pwDstTxt=new WCHAR[nWidLength+1];
				nMaxWcharLen=nWidLength+1;
			}
			::MultiByteToWideChar(CP_ACP,0,lpszMsg,-1,pwDstTxt,nMaxWcharLen);
	
			int nShowLen=min(nWidLength,MAXMSG);
			pwDstTxt[nShowLen]=0;
			::WideCharToMultiByte(CP_ACP,0,pwDstTxt,-1,m_szLogMsg,MAX_PATH,NULL,NULL);
			if(nWidLength>=MAX_PATH)
			{
				delete []pwDstTxt;
			}
		}
		else
		{
			lstrcpy(m_szLogMsg,lpszMsg);
		}
	}
	m_LogColor=LOG_NOR_COLOR;
	if(ll==LL_WARNING)
		m_LogColor=LOG_WARN_COLOR;
	else if(ll==LL_EXCEPT)
		m_LogColor=LOG_EXCP_COLOR;
	else if(ll==LL_DEBUG)
		m_LogColor=LOG_DEBUG_COLOR;

	m_bDrawFull=FALSE;
	::InvalidateRect(m_hWnd,NULL,FALSE);

	if(bFlashWnd)
	{
		m_LogColor=LOG_FLUSH_COLOR;
		m_bFlushing=TRUE;
		::SetTimer(m_hWnd,IDT_LOOP_PLAY_CW,m_dwTimeSpace,NULL);
	}

	LockHandle.UnLock();
}

//输出消息到窗口
void __cdecl CStatusDlg::OutputFormateToWnd(LOGLEVEL ll,BOOL bFlashWnd,LPCTSTR lpszFormate,...)
{
	const int MAXLOG=2048;
	TCHAR szOutMsg[MAXLOG]=TEXT("");
	va_list argList;
	va_start(argList,lpszFormate);
	_vsnprintf_c(szOutMsg,MAXLOG,lpszFormate,argList);
	va_end(argList);

	OutputLogToWnd(szOutMsg,ll,bFlashWnd);
}

//输出消息到窗口
void __cdecl CStatusDlg::OutputLogAndValueToWnd(LPCTSTR lpszMsg,UNDWORD udwValue,LOGLEVEL ll,BOOL bFlashWnd)
{
	if(m_bFlushing) return;
	m_udwParam=udwValue;
	OutputLogToWnd(lpszMsg,ll,bFlashWnd);
}
