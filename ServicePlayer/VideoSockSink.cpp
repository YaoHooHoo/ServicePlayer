//////////////////////////////////////////////////////////////////////////////////////////
//		��Ļ�����ص���ʵ���ļ� VideoSockSink.h.cpp
/////////////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "VideoSockSink.h"
#include "TimerEngine.h"

////////////////////////////////////////////////////////////////////
extern IShowLogToWnd *g_pIShowLogToWnd;
extern IShowLogToWnd *g_pIConfirmToWnd;
extern IShowLogToWnd *g_pTopicNotify;
/////////////////////////////////////////////////////////////////////////////////////////
CVideoSockSink::CVideoSockSink(void)
{
	m_bInited=true;
	m_nOffsetX=0;
	m_nOffsetY=0;
	m_nVideoCX=0;
	m_nVideoCY=0;
	m_nWndCX=0;
	m_nWndCY=0;
	m_pSpsData=NULL;
	m_dwSpsSize=0;
	m_bLogined=FALSE;
	m_pIVideoManage=NULL;
	m_dwFailNum=0;
	m_pIAudioModuleManage=NULL;
}

CVideoSockSink::~CVideoSockSink(void)
{
}

//�ӿڲ�ѯ
void * __cdecl CVideoSockSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IClientSocketSink,Guid,dwQueryVer);
	QUERYINTERFACE(IAudioDataSink,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IClientSocketSink,Guid,dwQueryVer);
	
	return NULL;
}

//��ʼ����Ƶ�������
bool CVideoSockSink::InitVideoSockSink(IVideoManage *pIVideoManage)
{
	if(!m_SocketClientHelper.CreateInstance())
	{
		g_pILogOutput->OutputLogToFile(m_SocketClientHelper.GetErrorMessage());
		return false;
	}

	m_pIClientSocket=m_SocketClientHelper.GetInterface();
	if(m_pIClientSocket==NULL)
	{
		g_pILogOutput->OutputLogToFile(TEXT("��ȡm_pIClientSocket�ӿ�ʧ��"));
		return false;
	}

	m_pIVideoManage=pIVideoManage;

	IUnknownEx *pSockIUnknown=GET_MYSELF_INTERFACE(IUnknownEx);
	m_pIClientSocket->SetSocketSink(pSockIUnknown);

	m_pIClientSocket->StartSocketThread();

	m_bInited=(m_pIVideoManage!=NULL);

	return m_bInited;
}

//������Ƶ����ӿ�
bool CVideoSockSink::SetAudioModuleManage(IAudioModuleManage *pIAudioModuleManage)
{
	m_pIAudioModuleManage=pIAudioModuleManage;
	return (m_pIAudioModuleManage!=NULL);
}

//���ӵ�LB������
bool CVideoSockSink::ConnectToServer(CONNECTSVRTYPE cst)
{
	ASSERT(m_pIClientSocket!=NULL);
	if(m_pIClientSocket==NULL) return false;
	CHAR szSvrUrl[64]="";
	WORD wSvrPort=0;
	DWORD dwServerIP=0;
	m_cst=cst;
	switch(cst)
	{
	case CST_LBSVR:
		m_pIClientSocket->CloseSocket(false);
		dwServerIP=g_GlobalData.dwLBSvrIP;
		wSvrPort=g_GlobalData.wLBSvrPort;
		g_pIShowLogToWnd->OutputLogToWnd(TEXT("��ʼ���ӵ�LB������..."));
		break;
	case CST_SWTSVR:
		m_bLogined=FALSE;
		dwServerIP=g_GlobalData.dwAvSvrIP;
		wSvrPort=g_GlobalData.wAvSvrPort;
		g_pIShowLogToWnd->OutputLogToWnd(TEXT("��ʼ���ӵ�ý����������..."));
		break;
	default:
		return false;
	}

	return m_pIClientSocket->ConnectToServer(dwServerIP,wSvrPort);
}

//��ʼ��Ƶ��������
bool CVideoSockSink::StartVideoSocket()
{
	if(m_pIClientSocket==NULL)
		return false;
	
	#if WRITE_FILE
	m_vDataFile.SetExName(TEXT(".vdt"));
	m_vDataFile.SetLinePreFile(0);
	m_vDataFile.SetLogPath(g_GlobalData.szSavePath);
	#endif
	
	g_GlobalData.bServicing=FALSE;
	m_dwDataSize=0;
	m_bLogined=FALSE;
	CONNECTSVRTYPE cst=CST_LBSVR;
	if(g_GlobalData.nConnectIpType==CIPT_AVS)
		cst=CST_SWTSVR;		//ֱ�����ӵ�avs������

	return ConnectToServer(cst);
}

//ֹͣ��Ƶ��������
bool CVideoSockSink::StopVideoSocket()
{
	if(m_pIClientSocket==NULL)
		return false;

	KILLTIMER(IDT_SHOW_DATA_RATE);
	m_pIClientSocket->CloseSocket(false);

	#if WRITE_FILE
	m_vDataFile.CloseOutFile();
	#endif

	return true;
}

//H264SpsPps����
bool __cdecl CVideoSockSink::OnH264SpsPpsData(const void * pHeadData,DWORD dwDataSize)
{
	if(m_dwSpsSize!=dwDataSize)
	{
		if(m_pSpsData!=NULL) delete [] m_pSpsData;
		m_pSpsData=new BYTE[dwDataSize+1];
		m_dwSpsSize=dwDataSize;
		if(m_pSpsData==NULL) return false;
		memcpy(m_pSpsData,pHeadData,dwDataSize);
	}

	if(m_bLogined)
	{
		DWORD dwCmdID=BuildCommandID(ALL_MAIN_CMD,TSVR_S_H264_SPS);
		m_pIClientSocket->SendData(dwCmdID,0,m_pSpsData,(WORD)dwDataSize);
	}
	
	return true;
}

//������Ƶ���ݻص�
bool __cdecl CVideoSockSink::OnSnapVideoData(const void * pImageData,DWORD dwDataSize,VENCODETYPE sst)
{
	if(m_bLogined)
	{
		DWORD dwCmdID=BuildCommandID(ALL_MAIN_CMD,TSVR_USER_VIDEO);
		m_pIClientSocket->SendData(dwCmdID,0,(void *)pImageData,(WORD)dwDataSize);
	}
	
	return true;
}

//¼�����ݻص�
bool __cdecl CVideoSockSink::OnRecordData(const void * pAudioData,DWORD dwDataSize)
{	
	if(m_bLogined && !g_GlobalData.bHaveVoice) return true;

	DWORD dwCmdID=BuildCommandID(AVS_MAIN_CMD,TSVR_SERVICE_AUDIO);
	m_pIClientSocket->SendData(dwCmdID,0,(void *)pAudioData,dwDataSize);
	
	return true;
}

//����������Ϣ
bool __cdecl CVideoSockSink::OnSocketConnect(int iErrorCode, LPCTSTR pszErrorDesc, IClientSocket * pIClientSocket)
{
	if(iErrorCode!=0)
	{
		switch(m_cst)
		{
		case CST_LBSVR:
			{
				SwitchLoadServerIpPort();	//�л�ip
				SETTIMER(IDT_CONN_SERVER,5000,1,CST_LBSVR,(ITimerSink *)this);
				g_pIShowLogToWnd->OutputFormateToWnd(LL_WARNING,FALSE,TEXT("���Ӹ��ؾ��������ʧ��,%s,5s�����ԣ�"),pszErrorDesc);
			}
			break; 
		case CST_SWTSVR:
			{
				int nConnType=CST_SWTSVR;
				if(g_GlobalData.nConnectIpType==CIPT_AVS)
				{
					SwitchLoadServerIpPort();	//�л�ip
				}
				else
				{
					if(++m_dwAvsConnNum>=3)
					{
						nConnType=CST_LBSVR;
						m_dwAvsConnNum=0;
					}
				}
				//��������
				SETTIMER(IDT_CONN_SERVER,5000,1,CST_SWTSVR,(ITimerSink *)this);		//���Ӷ�ʱ��
				g_pIShowLogToWnd->OutputFormateToWnd(LL_WARNING,FALSE,TEXT("��%u��������Ϣ������ʧ��,%s,5s�����ԣ�"),m_dwAvsConnNum,pszErrorDesc);
			}
			break;
		}
		
		return false;
	}

	bool bSuccess=true;
	switch(m_cst)
	{
	case CST_LBSVR:
		bSuccess=RequestSvrIPPort();
		break;
	case CST_SWTSVR:
		bSuccess=LoginToAvsSvr();
		break;
	}

	return bSuccess;
}

//�����ȡ��Ϣ
bool __cdecl CVideoSockSink::OnSocketRead(CMD_Command Command, void * pBuffer, DWORD dwDataSize, IClientSocket * pIClientSocket)
{
	switch(Command.wSubCmd)
	{
	case TSVR_USER_AUDIO:
		return OnProcUserAudioData(Command.dwSequenceID,pBuffer,dwDataSize);
	case TSVR_USER_VIDEO:
		return OnProcH264VideoData(Command.dwSequenceID,pBuffer,dwDataSize);
	case TSVR_S_H264_SPS:
		return OnProcH264SpsPpsData(pBuffer,dwDataSize);
	case TSVR_HELPER_CONFIRM:
		return OnProcReqHelperConfirm(Command.dwSequenceID,pBuffer,dwDataSize);
	case TSVR_SERVICE_LOGIN_RESP:
		return OnProcLoginToServerResp(Command.dwSequenceID,pBuffer,dwDataSize);
	case LBS_SC_REQ_SVRIP_RESP:
		return OnProcRequestSvrIPPortResp(Command.dwSequenceID,pBuffer,dwDataSize);
	case LBS_CC_EXIT_NOTIFY:
		return OnProcCustomExitNotify(Command.dwSequenceID,pBuffer,dwDataSize);
	case TSVR_STOP_HELPER_RESP:
		return OnProcStopHelperResp(pBuffer,dwDataSize);
	case ACITVE_TEST:
		return OnProcActiveTest(pIClientSocket);
	}

	return true;
}

//����ر���Ϣ
bool __cdecl CVideoSockSink::OnSocketClose(IClientSocket * pIClientSocket, bool bCloseByServer)
{
	if(m_cst==CST_SWTSVR)
	{
		m_bLogined=FALSE;
		g_pIShowLogToWnd->OutputLogToWnd(TEXT("���ӵ����������ӹر�,5s������..."),LL_WARNING);
		SETTIMER(IDT_CONN_SERVER,5000,1,CST_SWTSVR,(ITimerSink *)this);
	}

	return true;
}

//������������
bool CVideoSockSink::OnProcActiveTest(IClientSocket * pIClientSocket)
{
	DWORD dwCmdID=BuildCommandID(ALL_MAIN_CMD,ACITVE_TEST_RESP);
	pIClientSocket->SendData(dwCmdID,0,NULL,0);
	return true;
}

//�����������ַ
bool CVideoSockSink::RequestSvrIPPort()
{
	g_pIShowLogToWnd->OutputLogToWnd(TEXT("���Ӹ��ؾ���������ɹ�������ý�����������..."));

	BYTE cbBuffer[12];
	BYTE *pData=cbBuffer;

	WORD wSendSize=0;
	pData=WriteDword(pData,ST_SWTSVR,wSendSize);
	pData=WriteDword(pData,g_GlobalData.mDevSnData.dwServiceCode,wSendSize);

	DWORD dwCmdID=BuildCommandID(ALL_MAIN_CMD,LBS_SC_REQ_SVRIP);
	
	return m_pIClientSocket->SendData(dwCmdID,0,cbBuffer,wSendSize);
}

//���������������ַResponse
bool CVideoSockSink::OnProcRequestSvrIPPortResp(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize)
{
	ASSERT(dwDataSize==10);
	if(dwDataSize!=10)
	{
		g_pILogOutput->OutputFormateLog(TEXT("��ȡý����������ַ���ݴ�С�Ƿ�[%u]"),dwDataSize);
		return false;
	}

	IN_ADDR addrin;
	DWORD dwSvrType=0;
	DWORD dwSvrIp=0;
	BYTE *pData=(BYTE *)pDataBuffer;
	pData=ReadDword(pData,dwSvrType);
	pData=ReadDword(pData,dwSvrIp);
	pData=ReadWord(pData,g_GlobalData.wAvSvrPort);
	addrin.S_un.S_addr=dwSvrIp;
	g_GlobalData.dwAvSvrIP=dwSvrIp;

	CHAR *pTmpIP=inet_ntoa(addrin);
	g_pILogOutput->OutputFormateLog(TEXT("��ȡý����������ַ:[%s:%d]"),pTmpIP,g_GlobalData.wAvSvrPort);

	if(dwSvrIp==0)
	{ 
		g_pIShowLogToWnd->OutputFormateToWnd(LL_WARNING,FALSE,TEXT("��%u�λ�ȡý������������ַʧ�ܣ�5s������..."),++m_dwFailNum);	
		SETTIMER(IDT_CONN_SERVER,5000,1,CST_LBSVR,(ITimerSink *)this);
		return false;
	}
	if(g_GlobalData.wAvSvrPort==0)
	{
		g_pIShowLogToWnd->OutputFormateToWnd(LL_WARNING,FALSE,TEXT("��%u�λ�ȡý�����������˿�ʧ�ܣ�5s������..."),++m_dwFailNum);
		SETTIMER(IDT_CONN_SERVER,5000,1,CST_LBSVR,(ITimerSink *)this);
		return false;		 	
	}
	m_dwFailNum=0;
	g_pIShowLogToWnd->OutputLogToWnd(TEXT("�����������ַ�ɹ�"));

	SETTIMER(IDT_CONN_SERVER,25,1,ST_SWTSVR,(ITimerSink *)this);		//���ӵ�ý������������ʱ��
	return false;
}

//��¼��ý����������
bool CVideoSockSink::LoginToAvsSvr()
{
	BYTE cbBuffer[128];
	BYTE *pData=cbBuffer;

	WORD wSendSize=0;
	pData=WriteDword(pData,g_GlobalData.mDevSnData.dwServiceCode,wSendSize);
	BYTE cbJogLength=(BYTE)lstrlen(g_GlobalData.mDevSnData.szJobNum);
	pData=WriteByte(pData,cbJogLength,wSendSize);
	pData=WriteFromBuffer(pData,g_GlobalData.mDevSnData.szJobNum,cbJogLength,wSendSize);
	pData=WriteByte(pData,(BYTE)g_GlobalData.nBusinessType,wSendSize);

	DWORD dwCmdID=BuildCommandID(ALL_MAIN_CMD,TSVR_SERVICE_LOGIN);
	return m_pIClientSocket->SendData(dwCmdID,0,cbBuffer,wSendSize);
}

//�����¼������Response
bool CVideoSockSink::OnProcLoginToServerResp(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize)
{
	__int32 nResult=0;
	DWORD dwRoomIndex=0;

	BYTE *pData=(BYTE *)pDataBuffer;
	pData=ReadInt32(pData,nResult);
	if(nResult!=0)
	{
		g_pIShowLogToWnd->OutputFormateToWnd(LL_WARNING,FALSE,TEXT("��¼��ý����������ʧ��,�����룺%d"),nResult);
		return false;
	}

	m_bLogined=TRUE;
	g_pIShowLogToWnd->OutputLogToWnd(TEXT("��¼��ý�����������ɹ�"));

	DWORD *pDataAddr=&m_dwDataSize;
	SETTIMER(IDT_SHOW_DATA_RATE,1000,TIMER_REPEAT_TIMER,(WPARAM)pDataAddr,NULL);

	#if READ_TEST_FILE
	SETTIMER(IDT_TEST_VIDEO_FILES,2000,1,NULL,(ITimerSink *)this);
	#endif

	return true;
}

//��ʱ���¼�
bool __cdecl CVideoSockSink::OnEventTimer(WORD wTimerID,WPARAM wBindParam)
{
	if(wTimerID==IDT_CONN_SERVER)
	{
		CONNECTSVRTYPE ct=(CONNECTSVRTYPE)wBindParam;
		ConnectToServer(ct);
	}
	else if(wTimerID==IDT_SNAPSHOT_SHOW)
	{
		DWORD dwClientCode=882311;
		TCHAR szMsg[MAX_PATH]=TEXT("");
		mysprintf(szMsg,MAX_PATH,TEXT("�ͻ�[%u]������Ƶ����,�Ƿ�ͬ��?"),dwClientCode);
		g_pIConfirmToWnd->OutputLogAndValueToWnd(szMsg,dwClientCode,LL_NORMAL,TRUE);
		g_pTopicNotify->OutputLogAndValueToWnd(szMsg,dwClientCode,LL_NORMAL,TRUE);
	}
	#if READ_TEST_FILE	
	else if(IDT_TEST_VIDEO_FILES==wTimerID)
	{
		ReadVideoTestData();
	}
	#endif
	return true;
}

//����H264SpsPps����
bool CVideoSockSink::OnProcH264SpsPpsData(void *pDataBuffer,DWORD dwDataSize)
{
	#if WRITE_FILE
	m_vDataFile.OutputDataToFile(&dwDataSize,sizeof(dwDataSize));
	m_vDataFile.OutputDataToFile(pDataBuffer,(int)dwDataSize);
	#endif
	if(m_pIVideoManage!=NULL)
	{
		m_pIVideoManage->SetH264SpsPpsData((const BYTE *)pDataBuffer,(WORD)dwDataSize);
		m_pIVideoManage->GetEventAreaSize(m_nOffsetX,m_nOffsetY,m_nVideoCX,m_nVideoCY,m_nWndCX,m_nWndCY);	//��ȡ��Ƶ��С
		if(!g_GlobalData.bServicing)
		{
			SeriralClientCode(false);
			g_GlobalData.bServicing=TRUE;	//����������
			g_pIConfirmToWnd->OutputLogAndValueToWnd(TEXT("������Ƶ������..."),g_GlobalData.udwClientCode,LL_NORMAL,FALSE);
		}
	}

	g_pILogOutput->OutputLogToFile(TEXT("���յ�H264SpsPps���ݣ��ȴ���Ƶ�ؼ�֡..."));
	return true;
}

//����H264��Ƶ����
bool CVideoSockSink::OnProcH264VideoData(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize)
{
	#if WRITE_FILE
	m_vDataFile.OutputDataToFile(&dwDataSize,sizeof(dwDataSize));
	m_vDataFile.OutputDataToFile(pDataBuffer,(int)dwDataSize);
	#endif

	BYTE *pTmpData=(BYTE *)pDataBuffer;
	if(m_pIVideoManage!=NULL && g_GlobalData.bServicing)
		m_pIVideoManage->PlayVideoStream((const BYTE *)pTmpData,dwDataSize);

	//TRACE(TEXT("---Video data size:%u\n"),dwDataSize);

	m_dwDataSize+=dwDataSize;

	return true;
}

DWORD dwLastTime=0;
DWORD dwPackCount=0;
//����ͻ���Ƶ����
bool CVideoSockSink::OnProcUserAudioData(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize)
{
	if(m_pIAudioModuleManage!=NULL && g_GlobalData.bServicing)
	{
		m_pIAudioModuleManage->PlayAudioData(pDataBuffer,dwDataSize);
	}
	DWORD dwTime=::GetTickCount();
	dwLastTime=dwTime-dwLastTime;
	 
	TRACE(TEXT("---Audio data size:%u,Time:%u,Count:%u\n"),dwDataSize,dwLastTime,dwPackCount++);
	dwLastTime=dwTime;
	return true;
}

//��������¼�
bool __cdecl CVideoSockSink::SendMouseEventData(int nEventType,int x,int y)
{
	if(!m_bLogined) return false;

	//m_nOffsetX,m_nOffsetY,m_nVideoCX,m_nVideoCY
	if(m_nVideoCX==0 || m_nVideoCY==0)
		return false;
	if(m_nWndCX==0 || m_nWndCY==0)
		return false;
	//������
	x-=m_nOffsetX;
	y-=m_nOffsetY;

	BYTE cbBuffer[64];
	BYTE *pData=cbBuffer;

	WORD wSendSize=0;
	pData=WriteInt32(pData,x,wSendSize);
	pData=WriteInt32(pData,y,wSendSize);
	pData=WriteInt32(pData,m_nVideoCX,wSendSize);
	pData=WriteInt32(pData,m_nVideoCY,wSendSize);

	DWORD dwCmdID=BuildCommandID(ALL_MAIN_CMD,TSVR_T_BB_EVENT);
	return m_pIClientSocket->SendData(dwCmdID,nEventType,cbBuffer,wSendSize);
}

//���ͼ����¼�
bool __cdecl CVideoSockSink::SendKeyboardEvent(int nKeyValue,int nKeyType)
{
	if(!m_bLogined) return false;

	int nEventType=-1;

	BYTE cbBuffer[12];
	BYTE *pData=cbBuffer;

	WORD wSendSize=0;
	pData=WriteInt32(pData,nKeyValue,wSendSize);
	pData=WriteInt32(pData,nKeyType,wSendSize);
	
	DWORD dwCmdID=BuildCommandID(ALL_MAIN_CMD,TSVR_T_BB_EVENT);
	return m_pIClientSocket->SendData(dwCmdID,(DWORD)nEventType,cbBuffer,wSendSize);
}

//����ѡ����
void __cdecl CVideoSockSink::SendSelectedResult(UNDWORD udwClientCode,int nOptValue)
{
	BYTE cbBuffer[16];
	BYTE *pData=cbBuffer;
	WORD wSendSize=0;
	int nResult=0;
	DWORD dwCmdID=0;
	if(nOptValue==HOPT_STOP)
	{
		pData=WriteUNDword(pData,udwClientCode,wSendSize);
		pData=WriteDword(pData,g_GlobalData.mDevSnData.dwServiceCode,wSendSize);
		dwCmdID=BuildCommandID(ALL_MAIN_CMD,TSVR_STOP_HELPER);
		g_GlobalData.bServicing=FALSE;
		m_pIAudioModuleManage->SetPostRecordData(FALSE);
		m_pIVideoManage->OnCustomExitNotify();
		g_pIShowLogToWnd->OutputFormateToWnd(LL_NORMAL,FALSE,TEXT("ֹͣ��ͻ�%llu��Ƶ����"),udwClientCode);		
	}
	else 
	{
		if(nOptValue<0 || nOptValue==HOPT_REJECT)
		{
			nResult=SC_HOPT_REJECT;
			m_pIAudioModuleManage->SetPostRecordData(FALSE);
			m_pIVideoManage->OnCustomExitNotify();
			g_pIShowLogToWnd->OutputFormateToWnd(LL_NORMAL,FALSE,TEXT("�ܾ��ͻ�%llu��Ƶ��������"),udwClientCode);
		}
		else
		{
			g_GlobalData.udwClientCode=udwClientCode;
			if(g_GlobalData.bHaveVoice)
			{
				m_pIAudioModuleManage->SetPostRecordData(TRUE);
			}
			else
			{
				g_pIShowLogToWnd->OutputFormateToWnd(LL_NORMAL,FALSE,TEXT("��ͻ�%llu��Ƶ����,��û����Ƶ����"),udwClientCode);
			}
			g_pIConfirmToWnd->OutputFormateToWnd(LL_NORMAL,FALSE,TEXT("������Ƶ������..."));
			g_pIShowLogToWnd->OutputFormateToWnd(LL_NORMAL,FALSE,TEXT("��ʼ��ͻ�%llu������Ƶ����"),udwClientCode);
			SeriralClientCode(true);	//���л�
		}
		pData=WriteInt32(pData,nResult,wSendSize);
		pData=WriteUNDword(pData,udwClientCode,wSendSize);
		pData=WriteInt32(pData,g_GlobalData.bHaveVoice,wSendSize);
		dwCmdID=BuildCommandID(ALL_MAIN_CMD,TSVR_HELPER_CONFIRM_RESP);
		g_GlobalData.bServicing=TRUE;
	}
		
	if(dwCmdID>0)
	{
		m_pIClientSocket->SendData(dwCmdID,0,cbBuffer,wSendSize);
	}
	g_GlobalData.bConfirming=FALSE;
}

//�л�����IP
void CVideoSockSink::SwitchLoadServerIpPort()
{
	WORD wConnNum=0xFFFF;
	DWORD dwServerIP=0;
	WORD wServerPort=0;
 
	if(g_GlobalData.wLbsCount<=1) return;

	WORD wIndex=0;
	for(WORD i=0;i<g_GlobalData.wLbsCount;i++)
	{
		WORD wMinuNum=g_GlobalData.mLbsList[i].wConnCount; 
		if(wMinuNum<wConnNum)
		{
			wConnNum=wMinuNum;
			wIndex=i;
			dwServerIP=g_GlobalData.mLbsList[i].dwServerIp;
			wServerPort=g_GlobalData.mLbsList[i].wPort;	
		}
	}
	 
	if(dwServerIP!=0)
	{
		g_GlobalData.dwLBSvrIP=dwServerIP;
		g_GlobalData.wLBSvrPort=wServerPort;
		g_GlobalData.mLbsList[wIndex].wConnCount++;
	}
	if(g_GlobalData.nConnectIpType==CIPT_AVS)
	{
		g_GlobalData.dwAvSvrIP=dwServerIP;
		g_GlobalData.wAvSvrPort=wServerPort;
	}
}

//����������ȷ����Ϣ
bool CVideoSockSink::OnProcReqHelperConfirm(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize)
{
	if(dwDataSize<sizeof(UNDWORD))
		return true;

	UNDWORD udwClientCode=0;
	BYTE *pData=(BYTE *)pDataBuffer;
	pData=ReadUNDword(pData,udwClientCode);

	TRACE(TEXT("----�յ��ͻ�[%llu]������Ƶ����\n"),udwClientCode);
	int nResult=SERVICE_WORKING;
	if(g_GlobalData.bConfirming==TRUE)
	{
		SendSelectedResult(udwClientCode,nResult);		//����û�����ȷ����,�ڶ�������ֱ�ӷ���
		g_pILogOutput->OutputFormateLog(TEXT("�û�[%llu]������ȷ��,��ǰ�ͻ�����ȷ����һ���û�[%llu]��."),udwClientCode,m_udwLastConfirm);
		return true;
	}
	
	if(g_GlobalData.bServicing || udwClientCode==0)	//������
	{
		if(g_GlobalData.udwClientCode==udwClientCode)
		{
			nResult=0;
			g_pIShowLogToWnd->OutputFormateToWnd(LL_NORMAL,FALSE,TEXT("������ͻ�%llu����ͨѶ,�ͻ�����"),udwClientCode);
		}
		else
		{
			g_pIShowLogToWnd->OutputFormateToWnd(LL_WARNING,FALSE,TEXT("������,�ܾ��ͻ�%llu��������"),udwClientCode);
		}
		SendSelectedResult(udwClientCode,nResult);
	}
	else
	{
		g_GlobalData.bConfirming=TRUE;
		TCHAR szMsg[MAX_PATH]=TEXT("");
		mysprintf(szMsg,MAX_PATH,TEXT("�ͻ�[%llu]������Ƶ����,�Ƿ�ͬ��?"),udwClientCode);
		if(!g_GlobalData.bIsMinWnd && !g_GlobalData.bWndNotActive)
			g_pIConfirmToWnd->OutputLogAndValueToWnd(szMsg,udwClientCode,LL_NORMAL,TRUE);
		else
			g_pTopicNotify->OutputLogAndValueToWnd(szMsg,udwClientCode,LL_NORMAL,TRUE);
	}
	return true;
}

//����ͻ��˳�֪ͨ
bool CVideoSockSink::OnProcCustomExitNotify(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize)
{
	if(dwDataSize<sizeof(UNDWORD))
		return true;

	UNDWORD udwClientCode=0;
	BYTE *pData=(BYTE *)pDataBuffer;
	pData=ReadUNDword(pData,udwClientCode);
	
	if(g_GlobalData.udwClientCode==udwClientCode)
	{
		//��ʾ�ͻ��˳�
		g_GlobalData.bServicing=FALSE;
		m_pIVideoManage->OnCustomExitNotify();		
		m_pIAudioModuleManage->SetPostRecordData(FALSE);
	}
	g_pIShowLogToWnd->OutputFormateToWnd(LL_WARNING,FALSE,TEXT("�ͻ�%llu�˳���Ƶ����"),udwClientCode);
	return true;
}

//����ֹͣ����Response
bool CVideoSockSink::OnProcStopHelperResp(void *pDataBuffer,DWORD dwDataSize)
{
	m_pIVideoManage->OnCustomExitNotify();
	return true;
}

//���л����ͻ���
void CVideoSockSink::SeriralClientCode(bool bWrite)
{
	TCHAR szFileName[MAX_PATH]=TEXT("");
	mysprintf(szFileName,MAX_PATH,TEXT("%sccode.dat"),g_GlobalData.szWorkPath);
	
	CMyFile file;
	if(bWrite)
	{
		if(!file.Open(szFileName,CMyFile::CREATE|CMyFile::WRITE))
			return;

		file.Write(&g_GlobalData.udwClientCode,sizeof(g_GlobalData.udwClientCode));
	}
	else
	{
		if(!file.Open(szFileName,CMyFile::READ))
			return;

		file.Read(&g_GlobalData.udwClientCode,sizeof(g_GlobalData.udwClientCode));
	}
	file.Close();
}

//////////////////////////////////////////////////////////////////////////////////////
//��ȡ��������
void CVideoSockSink::ReadVideoTestData()
{
	DWORD dwDataSize=0;
	CMyFile myFile;
	TCHAR *pTestFile=TEXT("E:\\projects\\record_screen_demo\\��������\\0003\\27308_0605192058_1438368.tmp");
	if(!myFile.Open(pTestFile,CMyFile::READ)) 
		return;
	int iRetCode=0;
	int nIndex=0;
	while(true)
	{
		iRetCode=myFile.Read(&dwDataSize,sizeof(dwDataSize));
		if(iRetCode<=0) break;
		BYTE *pDataBuffer=new BYTE[dwDataSize+1];
		iRetCode=myFile.Read(pDataBuffer,dwDataSize);
		if(iRetCode!=(int)dwDataSize) break;
		if(nIndex==0)
		{
			m_pIVideoManage->SetH264SpsPpsData((const BYTE *)pDataBuffer,(WORD)dwDataSize);
		}
		else
		{
			m_pIVideoManage->PlayVideoStream(pDataBuffer,dwDataSize);
		}
		nIndex++;
		//m_pIVideoManage->PlayVideoStream(pDataBuffer,dwDataSize);
		delete [] pDataBuffer;
		::Sleep(200);
	}
}
