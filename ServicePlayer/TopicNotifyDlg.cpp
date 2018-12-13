/////////////////////////////////////////////////////////////////////////////////////////
//		ȷ�Ͽͻ�������Ƶ�����Ի�����ͷ�ļ� TopicNotifyDlg.cpp
//		2017-09-12		Bowen
//////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ServicePlayer.h"
#include "TopicNotifyDlg.h"
#include "afxdialogex.h"
#include "resource.h"

/////////////////////////////////////////////////////
const int BTNHIGHT=30;
const int BTNWIDTH=70;
const int TITLE_CY=55;						//�����
const int BTNBOTOOM=15;						//�ײ���
IShowLogToWnd *g_pTopicNotify=NULL;
//////////////////////////////////////////////////////////////////////////////////////////
// CTopicNotifyDlg �Ի���
CTopicNotifyDlg::CTopicNotifyDlg()
{
	m_titleFont.CreateFont(14,0,0,0,600,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH|FF_DONTCARE,TEXT("����"));

	m_cbAlphaNum=98;
	m_bIsAlpha=FALSE;

	m_dwTimeSpace=500;
	m_dwEndTime=15;
	g_pTopicNotify=(IShowLogToWnd *)this;
}

CTopicNotifyDlg::~CTopicNotifyDlg()
{
	m_titleFont.DeleteObject();
}


//��ʼ���Ի���
void CTopicNotifyDlg::OnSubCreate()
{
	RECT rc={0};
	IMouseEvent *pIMouseEvent=(IMouseEvent *)this;
	m_btnAccept.Create(NULL,WS_VISIBLE|WS_CHILD,rc,this,BTN_ACCEPT_ID);
	m_btnAccept.SetButtonImage(IDB_PNG_BTN_ACCECPT30,g_hInstance,false,true);
	m_btnAccept.SetMouseEventCallBack(pIMouseEvent);

	m_btnReject.Create(NULL,WS_VISIBLE|WS_CHILD,rc,this,BTN_REJECT_ID);
	m_btnReject.SetButtonImage(IDB_PNG_BTN_REJECT30,g_hInstance,false,true);
	m_btnReject.SetMouseEventCallBack(pIMouseEvent);
	
	lstrcpy(m_szLogMsg,TEXT("�ȴ�ȷ����Ƶ����..."));
}

//���ڴ�С��Ϣ
void CTopicNotifyDlg::OnSubSize(UINT nType, int cx, int cy)
{
	if(cx==0 || cy==0) return;

	const int LOGWNDCX=500;
	const int LOGWNDCY=16;
	int x=(cx-(BTNWIDTH*2+10))/2;
	int y=cy-BTNHIGHT-BTNBOTOOM;
	//�ƶ��ؼ�
	const UINT uFlags=SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOZORDER|SWP_NOSIZE;
 
	HDWP hDwp=BeginDeferWindowPos(2);
	DeferWindowPos(hDwp,m_btnAccept,NULL,x,y,0,0,uFlags);
	x+=80;
	DeferWindowPos(hDwp,m_btnReject,NULL,x,y,0,0,uFlags);

	m_bltRect.bottom=y-2;
 
	EndDeferWindowPos(hDwp);
}

//������־�ı�
void CTopicNotifyDlg::DrawLogText(CDC &memDC,int cx,int cy)
{
	if(!m_bShowLogTxt) return;

	int nTxtLen=lstrlen(m_szLogMsg);
	if(nTxtLen==0) return;

	const int BSIZE=18;
	RECT txtRect={BSIZE,TITLE_CY,cx-BSIZE,cy-BTNHIGHT-BTNBOTOOM};
	CFont *pOldFont=memDC.SelectObject(&m_txtFont12);
	COLORREF oldColor=memDC.SetTextColor(m_LogColor);
	UINT nFormate=DT_LEFT|DT_WORDBREAK;
	memDC.DrawText(m_szLogMsg,-1,&txtRect,nFormate);
	memDC.SelectObject(pOldFont);
	memDC.SetTextColor(oldColor);
}

//���Ʊ���
void CTopicNotifyDlg::DrawTitleTxt(CDC &memDC,int cx,int cy)
{
	const int TXTTOP=20;
	TCHAR szTitle[]=TEXT("��������ȷ��");
	RECT txtRect={0,TXTTOP,cx,TITLE_CY};
	CFont *pOldFont=memDC.SelectObject(&m_titleFont);
	COLORREF oldColor=memDC.SetTextColor(WND_BG_COLOR);
	memDC.DrawText(szTitle,-1,&txtRect,DT_CENTER|DT_VCENTER);
	memDC.SelectObject(pOldFont);
	memDC.SetTextColor(oldColor);
}

//��˸���
void CTopicNotifyDlg::OnFlushFinished()
{
	OutputLogToWnd(TEXT("�ȴ��ͻ�����..."));
	ShowWindow(SW_HIDE);
}

//�����Ϣ������
void __cdecl CTopicNotifyDlg::OutputLogAndValueToWnd(LPCTSTR lpszMsg,UNDWORD udwValue,LOGLEVEL ll,BOOL bFlashWnd)
{
	ShowWindow(SW_SHOW);
	::Sleep(100);
	__super::OutputLogAndValueToWnd(lpszMsg,udwValue,ll,bFlashWnd);
}

//��갴ť
HRESULT __cdecl CTopicNotifyDlg::OnEventLButtonUp(UINT uControlID, UINT nFlags, CPoint &point)
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
		::PostMessage(g_GlobalData.hMainWnd,WM_ACCEPT_NOTIFY_MSG,0,0);
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
	ShowWindow(SW_HIDE);
	
	return 0;
}

//��갴ť
HRESULT __cdecl CTopicNotifyDlg::OnEventRButtonUp(UINT uControlID, UINT nFlags, CPoint &point)
{
	return 0;
}
