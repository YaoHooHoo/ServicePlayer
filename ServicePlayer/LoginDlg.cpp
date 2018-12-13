// LoginDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ServicePlayer.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "localdef.h"
#include "TimerEngine.h"
#include "ToolsFunction.h"
#include "resource.h"

#define MAXPROCESSPOS			(MAXFAILCOUNT*20)						//���λ��
///////////////////////////////////////////////////////////////////////////////////
#define WM_DOWN_FINISHED		(WM_APP+100)							//���������Ϣ
#define WM_RELOGIN_CONFIGSVR	(WM_APP+101)							//���µ�¼���÷�����
///////////////////////////////////////////////////////////////////////////////////////////
// CLoginDlg �Ի���

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

//��ʼ���Ի���
BOOL CLoginDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//SetMicrPhoneVolume(0);

	::SetForegroundWindow(m_hWnd);
	::SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE);

	m_nProcessPos=0;

	if(!m_HttpDataManage.InitHttpManage())
	{
		AfxMessageBox(TEXT("��ʼ��Http�������ʧ��"));
		return FALSE;
	}
	if(!m_HttpDataManage.StartDownService())
	{
		AfxMessageBox(TEXT("����Http�������ʧ��"));
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
			AfxMessageBox(TEXT("�ǰ�װ�汾,�޷����У������°�װ"));
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
		g_pILogOutput->OutputFormateLog(TEXT("��ʼ�Զ��������÷�����,DevSN:%s"),g_GlobalData.mDevSnData.szTermSN);
		#endif
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);

		IHttpApiEventSink *pIHttpSink=(IHttpApiEventSink *)this;
		//�����ն˱��
		m_HttpDataManage.AddHttpApiEvent(LDT_REQ_CONFIG,NULL,(void *)pIHttpSink);
		SetTimer(IDT_LOGIN_PROCESS,1000,NULL);
	}

	return TRUE;
}

//��ʱ����Ϣ
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

//���������Ϣ�¼�
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
			AfxMessageBox(TEXT("��Ч�����,��ȷ�Ϲ��Ŷ�Ӧ�Ŀͷ�����Ч"));
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

//���µ�¼���÷������¼�
LRESULT CLoginDlg::OnReloginConfigSvr(WPARAM wParam,LPARAM lParam)
{
	m_dwHttpStartTime=::GetTickCount();
	g_pILogOutput->OutputFormateLog(TEXT("��ʼHttp���÷���������,StartTime:%u"),m_dwHttpStartTime);
	IHttpApiEventSink *pIHttpSink=(IHttpApiEventSink *)this;
	//�����ն˱��
	m_HttpDataManage.AddHttpApiEvent(LDT_REQ_CONFIG,NULL,(void *)pIHttpSink);

	return 0;
}

//��֤Lisence
void CLoginDlg::CheckLisenceValid()
{
	/*
	TCHAR szLisence1[WNAME_LEN]=TEXT("");
	BuildDateLisence(2021,11,30,2,szLisence1,WNAME_LEN);
	BuildDateLisence1(TEXT("SZ_00001"),2020,1,15,2,szLisence,WNAME_LEN);*/
	//��֤Lisence
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
			AfxMessageBox(TEXT("Lisence�����޷�����ʹ��,������������ЧLisence"));
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

// CLoginDlg ��Ϣ�������
void CLoginDlg::OnBnClickedOk()
{
	TCHAR szJobNum[WNAME_LEN]=TEXT("");
	TCHAR szPassword[WNAME_LEN]=TEXT("");
	::GetDlgItemText(m_hWnd,IDC_ED_SERVICE_CODE,szJobNum,NAME_LEN);
	::GetDlgItemText(m_hWnd,IDC_ED_PASSWORD,szPassword,WNAME_LEN);
	if(szJobNum[0]==0)
	{
		AfxMessageBox(TEXT("���������Ĺ���"));
		return;
	}
	if(szPassword[0]==0)
	{
		AfxMessageBox(TEXT("����������"));
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
	g_pILogOutput->OutputFormateLog(TEXT("��ʼ�ֶ��������÷�����,DevSN:%s"),g_GlobalData.mDevSnData.szTermSN);
	#endif
	IHttpApiEventSink *pIHttpSink=(IHttpApiEventSink *)this;
	//�����ն˱��
	m_HttpDataManage.AddHttpApiEvent(LDT_REQ_CONFIG,NULL,(void *)pIHttpSink);
	SetTimer(IDT_LOGIN_PROCESS,1000,NULL);
}

void CLoginDlg::OnBnClickedCancel()
{
	m_HttpDataManage.StopDownService();

	CDialogEx::OnCancel();
}

//���ݴ�����ɽӿ�
bool __cdecl CLoginDlg::HttpEventNotify(int nStatus,WPARAM wParam,const void * pDownData,WORD wDataSize,const int ldt)
{	
	DWORD dwCurrTime=::GetTickCount();
	dwCurrTime-=m_dwHttpStartTime;
	g_pILogOutput->OutputFormateLog(TEXT("Http���÷������������Status[%d],DataSize[%d],OITLDT[%d],Duration:%u"),nStatus,wDataSize,ldt,dwCurrTime);
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
			_snprintf_c(szErrMsg,CountArray(szErrMsg),"HttpApi������ʧ��:%s",szMsg);
			#if IS_TEST_HTTP
			g_pILogOutput->OutputLogToFile(szMsg);
			#endif

			IHttpDownManage * pIHttpDownManage=AfxGetHttpDownManage();
			if(pIHttpDownManage!=NULL) pIHttpDownManage->ClearHttpApiEvent();
			if(++m_dwLoginCount<MAXFAILCOUNT && (nStatus<JSON_VAL_INVALID || nStatus>DB_OPT_FAILD))
			{
				g_pILogOutput->OutputFormateLog(TEXT("�������÷�����ʧ�ܣ�������������,���Ӵ���[%d]..."),m_dwLoginCount);
				//::PostMessage(m_hWnd,WM_RELOGIN_CONFIGSVR,0,0);
				SetTimer(IDT_REQUEST_CONFIG,200,NULL);
				return true;
			}
			
			::MessageBoxA(m_hWnd,szErrMsg,"HttpApiʧ��",MB_OK);
			GetDlgItem(IDOK)->EnableWindow(TRUE);
			GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
		}

		if(ldt==LDT_REQ_DEV_CODE) lstrcpy(szMsg,TEXT("�ն˱��������ɣ�"));
		else if(ldt==LDT_REQ_CONFIG)lstrcpy(szMsg,TEXT("������Ϣ������ɣ�"));
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
			g_pILogOutput->OutputFormateLog(TEXT("�����ն˺�ʧ�ܣ���Ч�ն˺�,����Ϊ:%d"),wDataSize);
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
			g_GlobalData.nBusinessType=pcwData->nBusinessType;	//ҵ��������20180509���
		}
		if(pcwData->nConnectIpType>=0)
		{
			g_GlobalData.nConnectIpType=pcwData->nConnectIpType;
		}

		ParseLbsIpList(pcwData->szLbSvrIpPort);		//�������ط�������ַ
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

//��ȡ��ָ���ַ��ָ����ַ���
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

//����������IP��ַ
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
			//��Mixer�豸   
			if(mixerOpen(&m_hMixer, i, 0, 0, MIXER_OBJECTF_MIXER) != MMSYSERR_NOERROR)
				continue; 
			MIXERLINE MixerLine,oneLine={0};   
			//�ṹ��Ĵ�С   
			MixerLine.cbStruct = sizeof(MixerLine);   
			//¼���豸����   
			MixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;//MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;//MIXERLINE_COMPONENTTYPE_DST_WAVEIN;   
			 
			//�õ�¼�������е�������   
			if(mixerGetLineInfo((HMIXEROBJ)m_hMixer, &MixerLine,MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
				continue; 
			//������������   
			DWORD dwConnections = MixerLine.cConnections;   
			//׼����ȡ��˷��豸��ID   
			DWORD dwLineID = MixerLine.dwLineID;   
			for(DWORD i = 0; i < dwConnections; i++ )   
			{   
				//ö��ÿһ���豸����Source��ID���ڵ�ǰ�ĵ�������   
				oneLine=MixerLine;
				oneLine.dwSource = i;
				//����SourceID������ӵ���Ϣ   
				if(mixerGetLineInfo((HMIXEROBJ)m_hMixer, &oneLine,MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_DESTINATION) != MMSYSERR_NOERROR) 
					//�жϺ���ִ�д���   
					break;  
				//�����ǰ�豸��������˷磬������ѭ����   
				if(oneLine.dwComponentType == MIXERLINE_COMPONENTTYPE_DST_WAVEIN)   
				{   
					dwLineID = oneLine.dwLineID;
					dwSource = oneLine.dwSource;
					break;   
				}   
			}
			//���û���ҵ�������ʧ�ܡ�   
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

