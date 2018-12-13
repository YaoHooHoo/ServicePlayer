/////////////////////////////////////////////////////////////////////////////////////////
//		ȷ�Ͽͻ�������Ƶ�����Ի�����ʵ���ļ� ConfirmDlg.cpp
//		2017-09-12		Bowen
//////////////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "ConfirmDlg.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////
#define RBTNWIDTH			80						//�ұ߰�ť��
//////////////////////////////////////////////////////////////////////////////////////////
CConfirmDlg::CConfirmDlg(void)
{
	//m_gbColor=RGB(66,66,66);

	m_dwTimeSpace=400;
	m_dwEndTime=20;
}

CConfirmDlg::~CConfirmDlg(void)
{
}

//��ʼ���Ի���
void CConfirmDlg::OnSubCreate()
{
	RECT rc={0};
	IMouseEvent *pIMouseEvent=(IMouseEvent *)this;
	m_btnAccept.Create(NULL,WS_VISIBLE|WS_CHILD,rc,this,BTN_ACCEPT_ID);
	m_btnAccept.SetButtonImage(IDB_PNG_BTN_ACCEPT,g_hInstance,false,true);
	m_btnAccept.SetMouseEventCallBack(pIMouseEvent);

	m_btnReject.Create(NULL,WS_VISIBLE|WS_CHILD,rc,this,BTN_REJECT_ID);
	m_btnReject.SetButtonImage(IDB_PNG_BTN_REJECT,g_hInstance,false,true);
	m_btnReject.SetMouseEventCallBack(pIMouseEvent);

	m_btnStop.Create(NULL,WS_VISIBLE|WS_CHILD,rc,this,BTN_STOP_ID);
	m_btnStop.SetButtonImage(IDB_PNG_BTN_STOP,g_hInstance,false,true);
	m_btnStop.SetMouseEventCallBack(pIMouseEvent);

	bInvalidLisence=TRUE;
	nLisenceDays=10;
	
	lstrcpy(m_szLogMsg,TEXT("�ȴ���Ƶ��������..."));
}

//���ڴ�С��Ϣ
void CConfirmDlg::OnSubSize(UINT nType, int cx, int cy)
{
	if(cx==0 || cy==0) return;

	const int LOGWNDCX=500;
	const int LOGWNDCY=16;
	int x=cx-RBTNWIDTH+2;
	int y=2;
	//�ƶ��ؼ�
	const UINT uFlags=SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOZORDER|SWP_NOSIZE;
 
	HDWP hDwp=BeginDeferWindowPos(2);
	DeferWindowPos(hDwp,m_btnAccept,NULL,x,y,0,0,uFlags);
	x+=25;
	DeferWindowPos(hDwp,m_btnReject,NULL,x,y,0,0,uFlags);
	x+=25;
	DeferWindowPos(hDwp,m_btnStop,NULL,x,y,0,0,uFlags);
	
	//m_bltRect.right=cx-RBTNWIDTH;	//����ȫ��
 
	EndDeferWindowPos(hDwp);
}

//������־�ı�
void CConfirmDlg::DrawLogText(CDC &memDC,int cx,int cy)
{
	if(!m_bShowLogTxt) return;

	int nTxtLen=lstrlen(m_szLogMsg);
	if(nTxtLen==0) return;

	const int TXTTOP=6;
	const int BSIZE=10;
	RECT txtRect={BSIZE,TXTTOP,cx-BSIZE-RBTNWIDTH,cy};
	CFont *pOldFont=memDC.SelectObject(&m_txtFont12);
	COLORREF oldColor=memDC.SetTextColor(m_LogColor);
	memDC.DrawText(m_szLogMsg,-1,&txtRect,DT_LEFT|DT_VCENTER);
	memDC.SelectObject(pOldFont);
	memDC.SetTextColor(oldColor);
}

//��˸���
void CConfirmDlg::OnFlushFinished()
{
	OutputLogToWnd(TEXT("�ȴ��ͻ�����..."));
}

//��갴ť
HRESULT __cdecl CConfirmDlg::OnEventLButtonUp(UINT uControlID, UINT nFlags, CPoint &point)
{
	StopFlushShowMsg();

	if(BTN_ACCEPT_ID==uControlID)
	{	
		if(!g_GlobalData.bConfirming) return 0;
		//��������
		if(m_pISendMouseEvent!=NULL)
			m_pISendMouseEvent->SendSelectedResult(m_udwParam,HOPT_ACCEPT);
		g_GlobalData.bServicing=TRUE;
		g_GlobalData.bConfirming=FALSE;
	}
	else if(BTN_REJECT_ID==uControlID)
	{		
		if(!g_GlobalData.bConfirming) return 0;
		//�ܾ�����
		if(m_pISendMouseEvent!=NULL)
			m_pISendMouseEvent->SendSelectedResult(m_udwParam,HOPT_REJECT);
		g_GlobalData.bServicing=FALSE;
		g_GlobalData.bConfirming=FALSE;
		OnFlushFinished();
	}
	else if(BTN_STOP_ID==uControlID)
	{
		if(!g_GlobalData.bServicing && !g_GlobalData.bConfirming) return 0;
		if(m_pISendMouseEvent!=NULL)
			m_pISendMouseEvent->SendSelectedResult(m_udwParam,HOPT_STOP);
		g_GlobalData.bServicing=FALSE;
		g_GlobalData.bConfirming=FALSE;
		OnFlushFinished();
	}
	
	return 0;
}

//��갴ť
HRESULT __cdecl CConfirmDlg::OnEventRButtonUp(UINT uControlID, UINT nFlags, CPoint &point)
{
	return 0;
}
