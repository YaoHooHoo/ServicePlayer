
// ServicePlayerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ServicePlayer.h"
#include "ServicePlayerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////////////////////////////
IShowLogToWnd *g_pIShowLogToWnd=NULL;
IShowLogToWnd *g_pIConfirmToWnd=NULL;
GLOBALDATA g_GlobalData;
ILogOutput *g_pILogOutput=NULL;
/////////////////////////////////////////////////////////////////////////////////////////////
// CServicePlayerDlg �Ի���
BEGIN_MESSAGE_MAP(CServicePlayerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ACTIVATE()
	ON_WM_ACTIVATEAPP()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_NOTIFYICONDATA_MSG,OnNotifyIconMessage)
	ON_MESSAGE(WM_ACCEPT_NOTIFY_MSG,OnAcceptNotifyMessage)
	ON_MESSAGE(WM_CHECK_WND_ACTIVE,OnCheckWndActiveMessage)
	ON_MESSAGE(WM_PROCESS_NOTIFY,OnProcessCtrlNotifyMsg)
END_MESSAGE_MAP()

CServicePlayerDlg::CServicePlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServicePlayerDlg::IDD, pParent)
{
	m_bCreated=false;
	g_pILogOutput=(ILogOutput *)this;

	m_LogFileManage.SetLogPath(g_GlobalData.szLogPath);
	m_LogFileManage.SetPrevName(TEXT("sc"));
	m_LogFileManage.SetLinePreFile(5000);

	m_TimerEngine.StartService();

	g_pIShowLogToWnd=(IShowLogToWnd *)&m_StatusDlg;
	g_pIConfirmToWnd=(IShowLogToWnd *)&m_ConfirmDlg;
}

void CServicePlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

// CServicePlayerDlg ��Ϣ�������

BOOL CServicePlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	RECT wndRc={0};
	ISendMouseEvent *pISendMouseEvent=(ISendMouseEvent *)&m_VideoSockSink;
	m_VideoWnd.InitUserVideoWnd(pISendMouseEvent);

	m_VideoWnd.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,wndRc,this,LWID_VIDEO_WND);
	m_StatusDlg.Create(IDD_STATUSDLG,this);
	m_ConfirmDlg.Create(IDD_STATUSDLG,this);

	m_VideoWnd.SetFocus();
	m_TopicNotifyDlg.SetSendEventInterface(pISendMouseEvent);

	struct in_addr inaddr;
	inaddr.S_un.S_addr=g_GlobalData.dwAvSvrIP;
	TCHAR *pSvrIP=inet_ntoa(inaddr);

	IVideoManage *pIVideoManage=(IVideoManage *)this;
	m_VideoSockSink.InitVideoSockSink(pIVideoManage);

	m_TopicNotifyDlg.Create(IDD_TOPICNOTIFYDLG);

	if(m_AudioModuleHelper.CreateInstance()==false)
	{
		OutputLogToFile(m_AudioModuleHelper.GetErrorMessage());
		AfxMessageBox(m_AudioModuleHelper.GetErrorMessage());
		return false;
	}
	m_pIAudioModuleManage=m_AudioModuleHelper.GetInterface();
	ASSERT(m_pIAudioModuleManage!=NULL);
	IUnknownEx *pAudioIUnknownEx=GET_OBJECT_INTERFACE(m_VideoSockSink,IUnknownEx);
	if(!m_pIAudioModuleManage->SetAudioDataSink(pAudioIUnknownEx))
	{
		LPCTSTR lpszErr=TEXT("������Ƶ�����ҽӿ�ʧ��");
		g_pILogOutput->OutputLogToFile(lpszErr);
		AfxMessageBox(lpszErr);
		return false;
	}

	m_pIAudioModuleManage->SetAudioProcess(TRUE);
	m_pIAudioModuleManage->SetAuidoAecFlag(TRUE);
	m_VideoSockSink.SetAudioModuleManage(m_pIAudioModuleManage);

	m_ConfirmDlg.SetSendEventInterface(pISendMouseEvent);

	m_bCreated=true;

	SetWindowSize();

	SETTIMER(IDT_START_CONNN,100,1,CST_SWTSVR,(ITimerSink *)this);

	g_GlobalData.bExitApp=FALSE;
	g_GlobalData.bIsMinWnd=FALSE;
	g_GlobalData.bWndNotActive=FALSE;
	g_GlobalData.hMainWnd=m_hWnd;
		
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

//��Ϣ������
BOOL CServicePlayerDlg::PreTranslateMessage(MSG * pMsg)
{
	//TRACE(TEXT("----message:0x%x\n"),pMsg->message);

	if(pMsg->message==WM_KEYDOWN || pMsg->message==WM_KEYUP)
	{		
		int key=(int)pMsg->wParam;
		SendKeyboardEvent(key,pMsg->message);

		return FALSE;
	}
	else if(pMsg->message==WM_SYSCOMMAND)
	{
		if(pMsg->lParam==SC_MINIMIZE)
		{
			g_GlobalData.bIsMinWnd=TRUE;
		}else if (pMsg->lParam==SC_MAXIMIZE)
		{
			g_GlobalData.bIsMinWnd=FALSE;
			g_GlobalData.bWndNotActive=FALSE;
		}
	}

	return __super::PreTranslateMessage(pMsg);
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CServicePlayerDlg::OnPaint()
{
	CPaintDC dc(this); // ���ڻ��Ƶ��豸������
}

//���ڴ�С��Ϣ
void CServicePlayerDlg::OnSize(UINT nType, int cx, int cy)
{
	 if(nType== SIZE_MINIMIZED)
	 {
		 g_GlobalData.bIsMinWnd=TRUE;
	 }
	 else if(nType==SIZE_RESTORED)
	 {
		 g_GlobalData.bIsMinWnd=FALSE;
	 }

	if(cx==0 || cy==0) return;
	if(!m_bCreated) return;
	
	//�ƶ��ؼ�
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;
	//�ƶ��ؼ�
	const UINT uFlags1=SWP_NOZORDER|SWP_NOCOPYBITS|SWP_SHOWWINDOW;
	
	const int LOGWNDCY=24;
	const int RIGHTCX=400;
	const int RIGHTCY=24;
	

	int x=VIDEO_BORDER;
	int y=VIDEO_BORDER;
	int vcy=cy-VIDEO_BORDER*2-LOGWNDCY;
	int vcx=cx-VIDEO_BORDER*2;
	int LOGWNDCX=vcx-RIGHTCX;

	HDWP hDwp=BeginDeferWindowPos(3);
	DeferWindowPos(hDwp,m_VideoWnd,NULL,x,y,vcx,vcy-4,uFlags);

	DeferWindowPos(hDwp,m_StatusDlg,NULL,x,vcy,LOGWNDCX,LOGWNDCY,uFlags1);
	x+=LOGWNDCX;
	DeferWindowPos(hDwp,m_ConfirmDlg,NULL,x,vcy,RIGHTCX,RIGHTCY,uFlags1);
	EndDeferWindowPos(hDwp);
	
	__super::OnSize(nType,cx,cy);
}

//����ʧȥ����
void CServicePlayerDlg::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus(pNewWnd);
}

//������Ϣ
void CServicePlayerDlg::OnActivate(UINT nState,CWnd* pWndOther, BOOL bMinined)
{
	__super::OnActivate(nState,pWndOther,bMinined);
}

//������Ϣ
void CServicePlayerDlg::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	m_TimerEngine.SetTimer(IDT_CHECK_WND_ACTIVE,1000,1,NULL,NULL);
	__super::OnActivateApp(bActive,dwThreadID);
}

//��ⴰ�ڻ��Ϣ
LRESULT CServicePlayerDlg::OnCheckWndActiveMessage(WPARAM wParam,LPARAM lParam)
{
	g_GlobalData.bWndNotActive=CheckMainWndVisible(GetSafeHwnd());	//�Ƿ񱻸���

	TRACE(TEXT("------ActivateApp,IsKillFocus:%d\n"),g_GlobalData.bWndNotActive);
	return 0;
}
//�ر���Ϣ
void CServicePlayerDlg::OnClose()
{
	__super::OnClose();
}

//�˳���ť�¼�
void CServicePlayerDlg::OnCancel()
{
	if(g_GlobalData.bExitApp)
	{
		OnStopConnnect();
		CDialogEx::OnCancel();
	}
	else
	{
		//��������
		m_NotifyCtrl.SetNotifyIcon(g_hInstance,m_hWnd);
	}
}

//ϵͳ�����
BOOL CServicePlayerDlg::OnCommand(WPARAM wParam,LPARAM lParam)
{
	m_NotifyCtrl.OnMenuCommand(m_hWnd,wParam,lParam);

	return __super::OnCommand(wParam,lParam);
}

//��ʼ��ʱ������
void CServicePlayerDlg::StartTimerService()
{
	ITimerEngineSink *pITimerEngineSink=(ITimerEngineSink *)this;
	m_TimerEngine.SetTimerEngineSink(pITimerEngineSink);

	m_TimerEngine.StartService();
}

//ֹͣ��ʱ������
void CServicePlayerDlg::StopTimerSerivce()
{
	m_TimerEngine.StopService();
}

//��ʼ����
void CServicePlayerDlg::OnStartConnect()
{
	m_VideoWnd.StartPlayVideo();
	if(g_GlobalData.bHaveVoice)
	{
		g_GlobalData.bHaveVoice=StartAudioModule();		//����ͷ���û����Ƶ�豸,�Կͻ��˲�������Ƶ���ܡ�
	}

	if(!m_VideoSockSink.StartVideoSocket())
	{
		AfxMessageBox(TEXT("��������Ƶ����������ʧ�ܣ���鿴��־��"));
		return;
	}
	
	m_dwStartTime=0;
	m_dwLastSize=0;	
}

//ֹͣ����
void CServicePlayerDlg::OnStopConnnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_VideoSockSink.StopVideoSocket();
	m_VideoWnd.StopPlayVideo();
	if(g_GlobalData.bHaveVoice && m_pIAudioModuleManage!=NULL)
	{
		m_pIAudioModuleManage->StopAudioService();
	}
	
	OutputLogToFile(TEXT("�����������ӷ���ֹͣ"));
	m_VideoWnd.InvalidateRect(NULL,FALSE);
}


//��ʱ���¼�
bool __cdecl CServicePlayerDlg::OnEventTimer(WORD wTimerID,WPARAM wBindParam,void * pTimerSink)
{
	if(wTimerID==IDT_SHOW_DATA_RATE)
	{
		DWORD *pSizeData=(DWORD *)wBindParam;
		DWORD dwDataSize=*pSizeData;
		DWORD dwDiffSize=dwDataSize-m_dwLastSize;
		m_dwLastSize=dwDataSize;
		DWORD dwTime=::GetTickCount();
		DWORD dwDiffTime=dwTime-m_dwStartTime;
		m_dwStartTime=dwTime;
		if(dwDiffTime>0)
		{
			double dbRate=(double)dwDiffSize*1000/(double)dwDiffTime;
			dbRate/=1024.0;	//K
			TCHAR szRate[MAX_PATH]=TEXT("");
			mysprintf(szRate,MAX_PATH,TEXT("���մ���:%.02fK/s"),dbRate);
			OutputLogToFile(szRate);
		}
	}
	else if(wTimerID==IDT_START_CONNN)
	{
		OnStartConnect();
	}
	else if(wTimerID==IDT_CHECK_WND_ACTIVE)
	{
		::PostMessage(m_hWnd,WM_CHECK_WND_ACTIVE,0,0);
	}
	else if(pTimerSink!=NULL)
	{
		((ITimerSink *)pTimerSink)->OnEventTimer(wTimerID,wBindParam);
	}
	

	return true;
}

//�����־��Ϣ
void __cdecl CServicePlayerDlg::OutputLogToFile(LPCTSTR lpszMsg)
{
	m_LogFileManage.OutputLogToFile(lpszMsg);
}

//�����־��Ϣ
void __cdecl CServicePlayerDlg::OutputFormateLog(LPCTSTR lpszFormate,...)
{
	const int MAXLOG=2048+256;
	TCHAR szOutMsg[MAXLOG]=TEXT("");
	va_list argList;
	va_start(argList,lpszFormate);
	_vsnprintf_c(szOutMsg,MAXLOG,lpszFormate,argList);
	va_end(argList);

	m_LogFileManage.OutputLogToFile(szOutMsg);
}

//����H264SpsPps����
bool __cdecl CServicePlayerDlg::SetH264SpsPpsData(const BYTE * pSpsPps,WORD wDataSize)
{
	m_VideoWnd.SetH264SpsPpsData(pSpsPps,wDataSize);
	return true;
}

//������Ƶ��
bool __cdecl CServicePlayerDlg::PlayVideoStream(const BYTE *pVideoData,DWORD dwDataSize)
{
	m_VideoWnd.PlayVideoData(pVideoData,dwDataSize);
	return true;
}

//��ȡ�¼�����С
void __cdecl CServicePlayerDlg::GetEventAreaSize(int &nOffsetX,int &nOffsetY,int &nVideoCX,int &nWidthCY,int &nWndCX,int &nWndCY)
{
	m_VideoWnd.GetEventAreaSize(nOffsetX,nOffsetY,nVideoCX,nWidthCY,nWndCX,nWndCY);
}

//�û��˳�֪ͨ
void __cdecl CServicePlayerDlg::OnCustomExitNotify()
{
	m_VideoWnd.CustomExitNotify();
}

//���ô��ڴ�С
void CServicePlayerDlg::SetWindowSize()
{
	const int nTPNCX=200;
	const int nTPNCY=150;
	APPBARDATA ad;
	ZeroMemory(&ad,sizeof APPBARDATA);
	ad.cbSize=sizeof APPBARDATA;
	SHAppBarMessage(ABM_GETTASKBARPOS, &ad);
	int nBarHeight=ad.rc.bottom-ad.rc.top;
	nBarHeight-=2;

	int nWndWidth=0;
	int nWndHeight=0;
	int x=0,y=0;
	int nWidth=::GetSystemMetrics(SM_CXSCREEN);
	int nHeight=::GetSystemMetrics(SM_CYSCREEN);
	int nTopicX=(nWidth-nTPNCX);	//ȷ�����Ѵ�������x
	int nTopicY=(nHeight-nTPNCY);	//ȷ�����Ѵ�������y
	nWndWidth=nWidth;
	nWndHeight=nHeight;

	double dbRate=0.8;
	if(nWndWidth>MAX_WND_CX)
	{
		nWndWidth=(int)(nWndWidth*dbRate);
	}
	if(nWndHeight>MAX_WND_CY)
	{
		nWndHeight=(int)(nWndHeight*dbRate);
	}

	nWndHeight-=nBarHeight;
	nHeight-=nBarHeight;
	
	x=(nWidth-nWndWidth)/2;
	y=(nHeight-nWndHeight)/2;
	::SetWindowPos(m_hWnd,HWND_TOPMOST,x,y,nWndWidth,nWndHeight,SWP_SHOWWINDOW);
	::SetForegroundWindow(m_hWnd);

	nTopicY-=nBarHeight;
	::SetWindowPos(m_TopicNotifyDlg.m_hWnd,HWND_TOPMOST,nTopicX,nTopicY,nTPNCX,nTPNCY,SWP_HIDEWINDOW);	//SWP_HIDEWINDOW--SWP_SHOWWINDOW
}

//���ͼ����¼�
void CServicePlayerDlg::SendKeyboardEvent(int nKeyValue,int nKeyType)
{
	if(m_VideoWnd.m_bMouseKeyCtrl && m_VideoWnd.m_bDevType==CT_USER)
	{
		m_VideoSockSink.SendKeyboardEvent(nKeyValue,nKeyType);
	}
}

void CServicePlayerDlg::OnBnClickedChkMouseKey()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	/*BOOL bCtrlFlag=IsDlgButtonChecked(IDC_CHK_MOUSE_KEY);
	m_VideoWnd.SetMouseCtrlFlag(bCtrlFlag);*/
}

//������Ϣ
LRESULT CServicePlayerDlg::OnNotifyIconMessage(WPARAM wParam,LPARAM lParam)
{
	return m_NotifyCtrl.OnNotifyIconMessage(m_hWnd,wParam,lParam);
}

//������Ƶ������Ϣ
LRESULT CServicePlayerDlg::OnAcceptNotifyMessage(WPARAM wParam,LPARAM lParam)
{
	if(g_GlobalData.bIsNotifyIcon)
	{
		m_NotifyCtrl.DeleteNotifyIcon(m_hWnd);
		ShowWindow(SW_SHOW);
	}
	else
	{
		ShowWindow(SW_SHOWNORMAL);
	}
	return 0;
}

//��ʼ��Ƶģ��
bool CServicePlayerDlg::StartAudioModule()
{
	ASSERT(m_pIAudioModuleManage!=NULL);
	if(m_pIAudioModuleManage==NULL) return false;

	DWORD dwResult=m_pIAudioModuleManage->StartAudioService(AST_ALL);//AST_PLAY
	LPCTSTR lpszMsg=NULL;
	if(dwResult&AST_NO_RECORD)
	{
		lpszMsg=TEXT("�Ҳ���¼���豸,���QQ������,��ر�QQ������");
	}
	else if(dwResult&AST_NO_PLAY)
	{
		lpszMsg=TEXT("�Ҳ��������豸���޷�����ͨ��");
	}
	else if(dwResult&AST_CODEC_ERR)
	{
		lpszMsg=TEXT("��Ƶ�����������ʧ��");
	}
	else if(dwResult&AST_AP_ERROR)
	{
		lpszMsg=TEXT("��Ƶ�����������ʧ��");
	}

	if(lpszMsg!=NULL)
	{
		g_pIShowLogToWnd->OutputLogToWnd(lpszMsg);
	}
	else
	{
		g_pIShowLogToWnd->OutputLogToWnd(TEXT("��Ƶ���������ɹ�"));
	}
	
	return true;
}

//���̼����֪ͨ��Ϣ
LRESULT CServicePlayerDlg::OnProcessCtrlNotifyMsg(WPARAM wParam,LPARAM lParam)
{
	if(wParam==PCC_SHOW)
	{
		if(g_GlobalData.bIsNotifyIcon)
		{
			m_NotifyCtrl.DeleteNotifyIcon(m_hWnd);			 
		}
		ShowWindow(SW_SHOWNORMAL);
		SetWindowSize();
	}
	else if(wParam==PSS_HIDE)
	{
		ShowWindow(SW_SHOWMINIMIZED);
	}
	else if(wParam==PSS_HIDE)
	{
		OnCancel();
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////��֤�����ڲ��ɼ�
BOOL CheckMainWndVisible(HWND hMainWnd)
{
	TCHAR szTitle[MAX_PATH]=TEXT("");
	RECT mainRect,currRect;
	::GetWindowRect(hMainWnd,&mainRect);
	POINT p3={mainRect.left,mainRect.top};
	POINT p4={mainRect.right,mainRect.bottom};
	HWND hDeskTopWnd=::GetDesktopWindow();
	HWND hCurrWnd=hMainWnd;
	while((hCurrWnd=::GetNextWindow(hCurrWnd,GW_HWNDPREV))!=NULL)		//��ǰ���Ҵ���
	{
		if(hDeskTopWnd==hCurrWnd) break;
		if(!::IsWindowVisible(hCurrWnd)) continue;
		::GetWindowRect(hCurrWnd,&currRect);
		POINT p1={currRect.left,currRect.top};
		POINT p2={currRect.right,currRect.bottom};
		//if(p2.y>p3.y && p1.y<p4.y && p2.x>p3.x && p1.x<p4.x)
		if(!((p4.x<=p1.x) || (p4.y<=p1.y) || (p3.x>=p2.x) ||(p3.y>=p4.y)))
		{
			::GetWindowText(hCurrWnd,szTitle,MAX_PATH);
			TRACE(TEXT("-->>>%s\n"),szTitle);
			return TRUE;
		}
	}
	return FALSE;
}
