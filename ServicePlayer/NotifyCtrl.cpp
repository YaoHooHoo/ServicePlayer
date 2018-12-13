/////////////////////////////////////////////////////////////////////////////////////////
//		���̿�����ʵ���ļ� NotifyCtrl.cpp
/////////////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "NotifyCtrl.h"
#include "localdef.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////////////////
CNotifyCtrl::CNotifyCtrl(void)
{
}

CNotifyCtrl::~CNotifyCtrl(void)
{
}

//��������״̬
void CNotifyCtrl::SetNotifyIcon(HINSTANCE hInstance,HWND hWnd)
{
	//��������
	NOTIFYICONDATA nd;
	nd.cbSize=sizeof(NOTIFYICONDATA);
	nd.hWnd=hWnd;
	nd.uID=IDR_MAINFRAME;
	nd.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP;
	nd.uCallbackMessage=WM_NOTIFYICONDATA_MSG;
	nd.hIcon=::LoadIcon(hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));

	lstrcpy(nd.szTip,SAVB_APP_TIPIC);
	
	Shell_NotifyIcon(NIM_ADD,&nd);

	::ShowWindow(hWnd,SW_HIDE);
	g_GlobalData.bIsMinWnd=TRUE;
	g_GlobalData.bIsNotifyIcon=TRUE;
	return;
}

//ɾ������
void CNotifyCtrl::DeleteNotifyIcon(HWND hWnd)
{
	//ȡ������
	NOTIFYICONDATA nd;
	nd.cbSize=sizeof(NOTIFYICONDATA);
	nd.hWnd=hWnd;
	nd.uID=IDR_MAINFRAME;
	Shell_NotifyIcon(NIM_DELETE,&nd);
	g_GlobalData.bIsNotifyIcon=FALSE;
}

//������Ϣ
LRESULT CNotifyCtrl::OnNotifyIconMessage(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	UINT uID;				//ͼ��ID
	UINT uMouseMsg;			//�����Ϣ
	POINT pt;
	uID=(UINT)wParam;
	uMouseMsg=(UINT)lParam;
	if(uID==IDR_MAINFRAME)
	{
		if(uMouseMsg==WM_RBUTTONDOWN)	//�һ�
		{
            //�����˵�
			HMENU hMenu=::CreatePopupMenu();
			if(hMenu==NULL) return 0;

			AppendMenu(hMenu,0,IDM_CLOSE_APP,TEXT("�˳�"));

			//��ȡ���λ��
			::GetCursorPos(&pt);
			//��ʾ�˵�
			::TrackPopupMenu(hMenu,TPM_LEFTBUTTON, pt.x,pt.y,0,hWnd,NULL);
		}
		else if(uMouseMsg==WM_LBUTTONDBLCLK)
		{
			::ShowWindow(hWnd,SW_SHOW);
			DeleteNotifyIcon(hWnd);
			g_GlobalData.bIsMinWnd=FALSE;
		}
	}

    return 0;
}

//�˵��������
BOOL CNotifyCtrl::OnMenuCommand(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	UINT uMenuID=LOWORD(wParam);
	switch(uMenuID)
    {
    case IDM_CLOSE_APP:
		DeleteNotifyIcon(hWnd);
		g_GlobalData.bExitApp=TRUE;
		::PostMessage(hWnd,WM_CLOSE,0,0);
		break;
    }
	return TRUE;
}
