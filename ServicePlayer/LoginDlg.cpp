// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ServicePlayer.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "localdef.h"
#include "TimerEngine.h"
#include "ToolsFunction.h"
#include "resource.h"

#define MAXPROCESSPOS			(MAXFAILCOUNT*20)						//最大位置
///////////////////////////////////////////////////////////////////////////////////
#define WM_DOWN_FINISHED		(WM_APP+100)							//下载完成消息
#define WM_RELOGIN_CONFIGSVR	(WM_APP+101)							//重新登录配置服务器
///////////////////////////////////////////////////////////////////////////////////////////
// CLoginDlg 对话框

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)
BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CLoginDlg::OnBnClickedCancel)
	ON_MESSAGE(WM_DOWN_FINISHED,OnDownConfigFinished)
	ON_MESSAGE(WM_RELOGIN_CONFIGSVR,OnReloginConfigSvr)
	ON_WM_TIMER()
END_MESSAGE_MAP()

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoginDlg::IDD, pParent)
{
	m_dwLoginCount=0;
}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_PROGRESS1,m_ProgressCtrl);
}

//初始化对话框
BOOL CLoginDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//SetMicrPhoneVolume(0);

	::SetForegroundWindow(m_hWnd);
	::SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE);

	m_nProcessPos=0;

	if(!m_HttpDataManage.InitHttpManage())
	{
		AfxMessageBox(TEXT("初始化Http管理服务失败"));
		return FALSE;
	}
	if(!m_HttpDataManage.StartDownService())
	{
		AfxMessageBox(TEXT("启动Http管理服务失败"));
		return FALSE;
	}
	int nNameLen=lstrlen(g_GlobalData.mDevSnData.szTermSN);
	int nPassLen=lstrlen(g_GlobalData.mDevSnData.szPassword);

	if(nNameLen>0)
	{
		::SetDlgItemText(m_hWnd,IDC_ST_DEV_CODE,g_GlobalData.mDevSnData.szTermSN);
	}
	else
	{
		::SetDlgItemText(m_hWnd,IDC_ST_DEV_CODE,TEXT("000000"));
	}
	nNameLen=lstrlen(g_GlobalData.mDevSnData.szJobNum);
	if(nNameLen>0)
	{
		::SetDlgItemText(m_hWnd,IDC_ED_SERVICE_CODE,g_GlobalData.mDevSnData.szJobNum);
	}
	if(nPassLen>0)
	{
		::SetDlgItemText(m_hWnd,IDC_ED_PASSWORD,g_GlobalData.mDevSnData.szPassword);
	}
	
	m_ProgressCtrl.SetRange32(0,MAXPROCESSPOS);
	m_ProgressCtrl.SetStep(1);
		
#if TEST_DEBUG
	::SetDlgItemText(m_hWnd,IDC_ED_SERVICE_CODE,TEXT("SZ_00006"));
	::SetDlgItemText(m_hWnd,IDC_ED_PASSWORD,TEXT("123456"));
#endif

	int nSeqLen=lstrlen(g_GlobalData.mDevSnData.szSequenceID);
	if(nSeqLen>0)
	{
		int nValidDays=CheckSerialNumber(g_GlobalData.mDevSnData.szSequenceID);
		if(nValidDays<LISENCE_DELAY_DAYS)
		{
			AfxMessageBox(TEXT("非安装版本,无法运行，请重新安装"));
			CDialogEx::OnCancel();
			return FALSE;
		}
	}
	
#if TEST_DEBUG
	g_GlobalData.mDevSnData.bAutoLogin=FALSE;
#endif
	
	if(g_GlobalData.mDevSnData.bAutoLogin && nPassLen>0 && nNameLen>0)
	{
		m_dwHttpStartTime=::GetTickCount();

		#if IS_TEST_HTTP
		g_pILogOutput->OutputFormateLog(TEXT("开始自动请求配置服务器,DevSN:%s"),g_GlobalData.mDevSnData.szTermSN);
		#endif
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);

		IHttpApiEventSink *pIHttpSink=(IHttpApiEventSink *)this;
		//请求终端编号
		m_HttpDataManage.AddHttpApiEvent(LDT_REQ_CONFIG,NULL,(void *)pIHttpSink);
		SetTimer(IDT_LOGIN_PROCESS,1000,NULL);
	}

	return TRUE;
}

//定时器消息
void CLoginDlg::OnTimer(UINT_PTR nIDTime)
{
	if(IDT_REQUEST_CONFIG==nIDTime)
	{
		KillTimer(IDT_REQUEST_CONFIG);
		::PostMessage(m_hWnd,WM_RELOGIN_CONFIGSVR,0,0);
	}
	else
	{
		if(++m_nProcessPos>MAXPROCESSPOS)
			m_nProcessPos=MAXPROCESSPOS;
		m_ProgressCtrl.SetPos(m_nProcessPos);
	}
}

//下载完成消息事件
LRESULT CLoginDlg::OnDownConfigFinished(WPARAM wParam,LPARAM lParam)
{
	m_HttpDataManage.StopDownService();
	
	CheckLisenceValid();
	
	m_ProgressCtrl.SetPos(14);
	int nResult=(int)wParam;
	if(nResult==0 && g_GlobalData.nLisenceDays>LISENCE_DELAY_DAYS)
	{
		if(g_GlobalData.mDevSnData.dwServiceCode==0)
		{
			AfxMessageBox(TEXT("无效服务号,请确认工号对应的客服号有效"));
			CDialogEx::OnCancel();
		}
		else
		{
			CDialogEx::OnOK();
		}
	}
	else
	{
		CDialogEx::OnCancel();
	}

	return 0;
}

//重新登录配置服务器事件
LRESULT CLoginDlg::OnReloginConfigSvr(WPARAM wParam,LPARAM lParam)
{
	m_dwHttpStartTime=::GetTickCount();
	g_pILogOutput->OutputFormateLog(TEXT("开始Http配置服务器请求,StartTime:%u"),m_dwHttpStartTime);
	IHttpApiEventSink *pIHttpSink=(IHttpApiEventSink *)this;
	//请求终端编号
	m_HttpDataManage.AddHttpApiEvent(LDT_REQ_CONFIG,NULL,(void *)pIHttpSink);

	return 0;
}

//验证Lisence
void CLoginDlg::CheckLisenceValid()
{
	/*
	TCHAR szLisence1[WNAME_LEN]=TEXT("");
	BuildDateLisence(2021,11,30,2,szLisence1,WNAME_LEN);
	BuildDateLisence1(TEXT("SZ_00001"),2020,1,15,2,szLisence,WNAME_LEN);*/
	//验证Lisence
	WORD wYear=0,wMonth=0,wDay=0,wFunVal=0;
	bool bSuccess=ReadLisenceData1(g_GlobalData.mDevSnData.szLisence,g_GlobalData.mDevSnData.szTermSN,wYear,wMonth,wDay,wFunVal);
	if(!bSuccess)
	{
		bSuccess=ReadLisenceData(g_GlobalData.mDevSnData.szLisence,wYear,wMonth,wDay,wFunVal);
	}
	if(bSuccess)
	{
		SYSTEMTIME st;
		::GetLocalTime(&st);
		DWORD dwCurrTime=st.wYear*10000+st.wMonth*100+st.wDay;
		DWORD dwLisenceTime=wYear*10000+wMonth*100+wDay;
		g_GlobalData.bInvalidLisence=(dwLisenceTime>=dwCurrTime);
		#if SUPPORT_MOUSE
		if(g_GlobalData.bIsHaveMouseCtrl)
		{
			g_GlobalData.bIsHaveMouseCtrl=(wFunVal>=LFV_1);
		}
		#else
		g_GlobalData.bIsHaveMouseCtrl=FALSE;
		#endif
		g_GlobalData.bHaveVoice=(wFunVal>=LFV_2);
		g_GlobalData.nLisenceDays=(int)dwLisenceTime-(int)dwCurrTime;
		if(g_GlobalData.nLisenceDays<LISENCE_DELAY_DAYS)
		{
			AfxMessageBox(TEXT("Lisence过期无法继续使用,请重新申请有效Lisence"));
			CDialogEx::OnCancel();
			return;
		}
		else if(g_GlobalData.nLisenceDays<0) g_GlobalData.nLisenceDays=0;
	}
	else
	{
		g_GlobalData.bInvalidLisence=FALSE;
	}
}

// CLoginDlg 消息处理程序
void CLoginDlg::OnBnClickedOk()
{
	TCHAR szJobNum[WNAME_LEN]=TEXT("");
	TCHAR szPassword[WNAME_LEN]=TEXT("");
	::GetDlgItemText(m_hWnd,IDC_ED_SERVICE_CODE,szJobNum,NAME_LEN);
	::GetDlgItemText(m_hWnd,IDC_ED_PASSWORD,szPassword,WNAME_LEN);
	if(szJobNum[0]==0)
	{
		AfxMessageBox(TEXT("请输入您的工号"));
		return;
	}
	if(szPassword[0]==0)
	{
		AfxMessageBox(TEXT("请输入密码"));
		return;
	}

	lstrcpy(g_GlobalData.mDevSnData.szJobNum,szJobNum);
	lstrcpy(g_GlobalData.mDevSnData.szPassword,szPassword);
	BOOL bAutoLogin=::IsDlgButtonChecked(m_hWnd,IDC_CHK_AUTO_LOGIN);
	if(bAutoLogin!=g_GlobalData.mDevSnData.bAutoLogin)
	{
		g_GlobalData.mDevSnData.bAutoLogin=bAutoLogin;
		SerailParameter(false);
	}
	int nSeqLen=lstrlen(g_GlobalData.mDevSnData.szSequenceID);
	if(nSeqLen==0)
	{
		GetSysSerialNum(g_GlobalData.mDevSnData.szSequenceID,SEQSIZE,10);
		SerailParameter(false);
	}
	#if IS_TEST_HTTP
	g_pILogOutput->OutputFormateLog(TEXT("开始手动请求配置服务器,DevSN:%s"),g_GlobalData.mDevSnData.szTermSN);
	#endif
	IHttpApiEventSink *pIHttpSink=(IHttpApiEventSink *)this;
	//请求终端编号
	m_HttpDataManage.AddHttpApiEvent(LDT_REQ_CONFIG,NULL,(void *)pIHttpSink);
	SetTimer(IDT_LOGIN_PROCESS,1000,NULL);
}

void CLoginDlg::OnBnClickedCancel()
{
	m_HttpDataManage.StopDownService();

	CDialogEx::OnCancel();
}

//数据传输完成接口
bool __cdecl CLoginDlg::HttpEventNotify(int nStatus,WPARAM wParam,const void * pDownData,WORD wDataSize,const int ldt)
{	
	DWORD dwCurrTime=::GetTickCount();
	dwCurrTime-=m_dwHttpStartTime;
	g_pILogOutput->OutputFormateLog(TEXT("Http配置服务器请求完成Status[%d],DataSize[%d],OITLDT[%d],Duration:%u"),nStatus,wDataSize,ldt,dwCurrTime);
	if(wDataSize==0)
	{
		const int MSGSIZE=512;
		TCHAR szMsg[MAX_PATH]=TEXT("");
		if(nStatus<0)
		{
			LPCSTR lpszErrMsg=(LPCSTR)pDownData;
			const int MAXERRMSG=1024;
			TCHAR szErrMsg[MAXERRMSG]="";			
			int nErrLength=lstrlen(lpszErrMsg);
			if(nErrLength>=MSGSIZE) nErrLength=MSGSIZE-1;
			memcpy(szMsg,lpszErrMsg,nErrLength);
			szMsg[nErrLength]=0;
			_snprintf_c(szErrMsg,CountArray(szErrMsg),"HttpApi请数据失败:%s",szMsg);
			#if IS_TEST_HTTP
			g_pILogOutput->OutputLogToFile(szMsg);
			#endif

			IHttpDownManage * pIHttpDownManage=AfxGetHttpDownManage();
			if(pIHttpDownManage!=NULL) pIHttpDownManage->ClearHttpApiEvent();
			if(++m_dwLoginCount<MAXFAILCOUNT && (nStatus<JSON_VAL_INVALID || nStatus>DB_OPT_FAILD))
			{
				g_pILogOutput->OutputFormateLog(TEXT("请求配置服务器失败，重新请求连接,连接次数[%d]..."),m_dwLoginCount);
				//::PostMessage(m_hWnd,WM_RELOGIN_CONFIGSVR,0,0);
				SetTimer(IDT_REQUEST_CONFIG,200,NULL);
				return true;
			}
			
			::MessageBoxA(m_hWnd,szErrMsg,"HttpApi失败",MB_OK);
			GetDlgItem(IDOK)->EnableWindow(TRUE);
			GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
		}

		if(ldt==LDT_REQ_DEV_CODE) lstrcpy(szMsg,TEXT("终端编号下载完成！"));
		else if(ldt==LDT_REQ_CONFIG)lstrcpy(szMsg,TEXT("配置信息下载完成！"));
		g_pILogOutput->OutputLogToFile(szMsg);

		if(ldt==LDT_REQ_CONFIG)
		{
			::PostMessage(m_hWnd,WM_DOWN_FINISHED,nStatus,0);
		}
		return false;
	}
	
	if(ldt==LDT_REQ_DEV_CODE)
	{

		ASSERT(wDataSize<NAME_LEN);
		if(wDataSize>=NAME_LEN)
		{
			g_pILogOutput->OutputFormateLog(TEXT("请求终端号失败，无效终端号,长度为:%d"),wDataSize);
			return false;
		}
		memcpy(g_GlobalData.mDevSnData.szTermSN,pDownData,wDataSize);
		g_GlobalData.mDevSnData.szTermSN[wDataSize]=0;
		SerailParameter(false);
	}
	else if(ldt==LDT_REQ_CONFIG)
	{
		ASSERT(wDataSize==sizeof(REQCONFDATA));
		if(wDataSize!=sizeof(REQCONFDATA)) return false;
		REQCONFDATA *pcwData=(REQCONFDATA *)pDownData;
		g_GlobalData.wVideoCX=pcwData->wVideoCX; 
		g_GlobalData.wVideoCY=pcwData->wVideoCY;
		g_GlobalData.mDevSnData.dwServiceCode=pcwData->dwSerivceCode;
		if(pcwData->nBusinessType>0)
		{
			g_GlobalData.nBusinessType=pcwData->nBusinessType;	//业务线类型20180509添加
		}
		if(pcwData->nConnectIpType>=0)
		{
			g_GlobalData.nConnectIpType=pcwData->nConnectIpType;
		}

		ParseLbsIpList(pcwData->szLbSvrIpPort);		//分析负载服务器地址
		bool bSaveData=false;
		int nNameLen=lstrlen(pcwData->szTermSN);
		if(nNameLen>0 && nNameLen<NAME_LEN && lstrcmp(g_GlobalData.mDevSnData.szTermSN,pcwData->szTermSN)!=0)
		{
			lstrcpy(g_GlobalData.mDevSnData.szTermSN,pcwData->szTermSN);
			bSaveData=true;
		}
		int nLiLen=lstrlen(pcwData->szLisence);
		if(nLiLen>0 && nLiLen<MAX_NAME && lstrcmp(g_GlobalData.mDevSnData.szLisence,pcwData->szLisence)!=0)
		{
			lstrcpy(g_GlobalData.mDevSnData.szLisence,pcwData->szLisence);
			bSaveData=true;
		}

		if(bSaveData) SerailParameter(false);
	}
	
	return true;
}

//获取以指定字符分隔的字符串
int ParseSubString(LPTSTR lpszList,const TCHAR *split,CStringVec & slist)
{
	if(lpszList==NULL) return 0;

	int nCount=0;
	TCHAR *pNext=NULL;
	TCHAR *pSplit = strtok_s(lpszList,split,&pNext); 	
	while(pSplit!=NULL)
	{
		string strSub=pSplit;
		slist.push_back(strSub);
		pSplit= strtok_s(NULL,split,&pNext); 
		nCount++;
	}

	return nCount;
}

//解析服务器IP地址
void ParseLbsIpList(LPTSTR lszTmpIP)
{
	CStringVec slist;
	int nCount=ParseSubString(lszTmpIP,",",slist);
	if(nCount>0)
	{
		TCHAR szSubStr[MAX_PATH];
		for(int i=0;i<nCount;i++)
		{
			CStringVec itemVec;
			string strItem=slist[i];
			lstrcpy(szSubStr,strItem.c_str());
			int nSubCount=ParseSubString(szSubStr,":",itemVec);
			if(nSubCount==2)
			{
				string strSvrIp=itemVec[0];
				string strPort=itemVec[1];
				DWORD dwServerIp=inet_addr(strSvrIp.c_str());
				WORD wPort=(WORD)atoi(strPort.c_str());
				g_GlobalData.mLbsList[g_GlobalData.wLbsCount].dwServerIp=dwServerIp;
				g_GlobalData.mLbsList[g_GlobalData.wLbsCount++].wPort=wPort;
			}
		}
	}
	if(g_GlobalData.wLbsCount>0)
	{	
		g_GlobalData.wLBSvrPort=g_GlobalData.mLbsList[0].wPort;
		g_GlobalData.dwLBSvrIP=g_GlobalData.mLbsList[0].dwServerIp;
		g_GlobalData.mLbsList[0].wConnCount=1;
	}
	if(g_GlobalData.nConnectIpType==CIPT_AVS)
	{
		g_GlobalData.dwAvSvrIP=g_GlobalData.dwLBSvrIP;
		g_GlobalData.wAvSvrPort=g_GlobalData.wLBSvrPort;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
#include "mmsystem.h"
#pragma comment(lib, "winmm.lib")

BOOL SetMicrPhoneVolume(DWORD dwVolume)
{
	dwVolume=50;
	BOOL bResult = FALSE;
	try
	{
		int mixerNum = (int)mixerGetNumDevs();
		for(int i = 0; i < mixerNum; i ++)
		{
			DWORD dwSource = 0;
			HMIXER m_hMixer = NULL;   
			MMRESULT mr = NULL;
			//打开Mixer设备   
			if(mixerOpen(&m_hMixer, i, 0, 0, MIXER_OBJECTF_MIXER) != MMSYSERR_NOERROR)
				continue; 
			MIXERLINE MixerLine,oneLine={0};   
			//结构体的大小   
			MixerLine.cbStruct = sizeof(MixerLine);   
			//录制设备总线   
			MixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;//MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;//MIXERLINE_COMPONENTTYPE_DST_WAVEIN;   
			 
			//得到录制总线中的连接数   
			if(mixerGetLineInfo((HMIXEROBJ)m_hMixer, &MixerLine,MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
				continue; 
			//将连接数保存   
			DWORD dwConnections = MixerLine.cConnections;   
			//准备获取麦克风设备的ID   
			DWORD dwLineID = MixerLine.dwLineID;   
			for(DWORD i = 0; i < dwConnections; i++ )   
			{   
				//枚举每一个设备，当Source的ID等于当前的迭代记数   
				oneLine=MixerLine;
				oneLine.dwSource = i;
				//根据SourceID获得连接的信息   
				if(mixerGetLineInfo((HMIXEROBJ)m_hMixer, &oneLine,MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_DESTINATION) != MMSYSERR_NOERROR) 
					//判断函数执行错误   
					break;  
				//如果当前设备类型是麦克风，则跳出循环。   
				if(oneLine.dwComponentType == MIXERLINE_COMPONENTTYPE_DST_WAVEIN)   
				{   
					dwLineID = oneLine.dwLineID;
					dwSource = oneLine.dwSource;
					break;   
				}   
			}
			//如果没有找到，返回失败。   
			if(dwLineID == 0) continue;   
			//get line id of destination line   
			//MIXERLINE lineinfo_Dest;   
			//::ZeroMemory(&lineinfo_Dest, sizeof(MIXERLINE));   
			//lineinfo_Dest.cbStruct = sizeof(MIXERLINE);   
			//lineinfo_Dest.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;   
			//if(::mixerGetLineInfo((HMIXEROBJ)m_hMixer,&lineinfo_Dest,MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
			//	continue;  
			////get id of specified connector   
			//MIXERLINE lineinfo_Connector;   
			//::ZeroMemory(&lineinfo_Connector, sizeof(MIXERLINE));  
			//lineinfo_Connector.cbStruct = sizeof(MIXERLINE);   
			//lineinfo_Connector.dwDestination = lineinfo_Dest.dwLineID;   
			//lineinfo_Connector.dwSource = dwSource;   
			//if(::mixerGetLineInfo((HMIXEROBJ)m_hMixer,&lineinfo_Connector,MIXER_GETLINEINFOF_SOURCE) != MMSYSERR_NOERROR)
			//	continue;  
			MIXERCONTROL mxc;   
			MIXERLINECONTROLS mxlc;   
			mxlc.cbStruct = sizeof(MIXERLINECONTROLS);   
			mxlc.dwLineID = dwLineID;   
			mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_MILLITIME;   //MIXERCONTROL_CONTROLTYPE_VOLUME
			mxlc.cControls = 1;   
			mxlc.cbmxctrl = sizeof(MIXERCONTROL);   
			mxlc.pamxctrl = &mxc;   
			if(::mixerGetLineControls(reinterpret_cast<HMIXEROBJ>(m_hMixer),&mxlc,MIXER_OBJECTF_HMIXER|MIXER_GETLINECONTROLSF_ONEBYTYPE)!= MMSYSERR_NOERROR)
				continue;  
			//get volume value range now   
			MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;   
			MIXERCONTROLDETAILS mxcd;   
			mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);   
			mxcd.dwControlID = mxc.dwControlID;   
			mxcd.cChannels = 1;   
			mxcd.cMultipleItems = 0;   
			mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);   
			mxcd.paDetails = &mxcdVolume;   
			if(mixerGetControlDetails(reinterpret_cast<HMIXEROBJ>(m_hMixer),&mxcd,MIXER_GETCONTROLDETAILSF_VALUE)!= MMSYSERR_NOERROR)
				continue;  
			//set volume   
			MIXERCONTROLDETAILS_UNSIGNED mxcdVolume_Set = { mxc.Bounds.dwMaximum * dwVolume / 100 };   
			MIXERCONTROLDETAILS mxcd_Set;   
			mxcd_Set.cbStruct = sizeof(MIXERCONTROLDETAILS);   
			mxcd_Set.dwControlID = mxc.dwControlID;
			mxcd_Set.cChannels = 1;   
			mxcd_Set.cMultipleItems = 0;   
			mxcd_Set.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);   
			mxcd_Set.paDetails = &mxcdVolume_Set;   
			if(mixerSetControlDetails(reinterpret_cast<HMIXEROBJ>(m_hMixer),&mxcd_Set,MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
				continue;   
			::mixerClose(m_hMixer);
			m_hMixer = NULL;
			bResult = TRUE;;
		}
	}
	catch(...){ return FALSE; }
	return bResult;
}

