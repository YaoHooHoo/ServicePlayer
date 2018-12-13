/////////////////////////////////////////////////////////////////////////////////////////
//		托盘控制类实现文件 NotifyCtrl.cpp
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

//进入托盘状态
void CNotifyCtrl::SetNotifyIcon(HINSTANCE hInstance,HWND hWnd)
{
	//进行托盘
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

//删除托盘
void CNotifyCtrl::DeleteNotifyIcon(HWND hWnd)
{
	//取消托盘
	NOTIFYICONDATA nd;
	nd.cbSize=sizeof(NOTIFYICONDATA);
	nd.hWnd=hWnd;
	nd.uID=IDR_MAINFRAME;
	Shell_NotifyIcon(NIM_DELETE,&nd);
	g_GlobalData.bIsNotifyIcon=FALSE;
}

//托盘消息
LRESULT CNotifyCtrl::OnNotifyIconMessage(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	UINT uID;				//图标ID
	UINT uMouseMsg;			//鼠标消息
	POINT pt;
	uID=(UINT)wParam;
	uMouseMsg=(UINT)lParam;
	if(uID==IDR_MAINFRAME)
	{
		if(uMouseMsg==WM_RBUTTONDOWN)	//右击
		{
            //创建菜单
			HMENU hMenu=::CreatePopupMenu();
			if(hMenu==NULL) return 0;

			AppendMenu(hMenu,0,IDM_CLOSE_APP,TEXT("退出"));

			//获取鼠标位置
			::GetCursorPos(&pt);
			//显示菜单
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

//菜单命令处理函数
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
