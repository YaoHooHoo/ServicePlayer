//////////////////////////////////////////////////////////////////////////////////////////
//		屏幕截屏回调类实现文件 VideoSockSink.h.cpp
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

//接口查询
void * __cdecl CVideoSockSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IClientSocketSink,Guid,dwQueryVer);
	QUERYINTERFACE(IAudioDataSink,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IClientSocketSink,Guid,dwQueryVer);
	
	return NULL;
}

//初始化视频网络调类
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
		g_pILogOutput->OutputLogToFile(TEXT("获取m_pIClientSocket接口失败"));
		return false;
	}

	m_pIVideoManage=pIVideoManage;

	IUnknownEx *pSockIUnknown=GET_MYSELF_INTERFACE(IUnknownEx);
	m_pIClientSocket->SetSocketSink(pSockIUnknown);

	m_pIClientSocket->StartSocketThread();

	m_bInited=(m_pIVideoManage!=NULL);

	return m_bInited;
}

//设置音频管理接口
bool CVideoSockSink::SetAudioModuleManage(IAudioModuleManage *pIAudioModuleManage)
{
	m_pIAudioModuleManage=pIAudioModuleManage;
	return (m_pIAudioModuleManage!=NULL);
}

//连接到LB服务器
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
		g_pIShowLogToWnd->OutputLogToWnd(TEXT("开始连接到LB服务器..."));
		break;
	case CST_SWTSVR:
		m_bLogined=FALSE;
		dwServerIP=g_GlobalData.dwAvSvrIP;
		wSvrPort=g_GlobalData.wAvSvrPort;
		g_pIShowLogToWnd->OutputLogToWnd(TEXT("开始连接到媒体流服务器..."));
		break;
	default:
		return false;
	}

	return m_pIClientSocket->ConnectToServer(dwServerIP,wSvrPort);
}

//开始视频网络连接
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
		cst=CST_SWTSVR;		//直接连接到avs服务器

	return ConnectToServer(cst);
}

//停止视频网络连接
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

//H264SpsPps数据
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

//截屏视频数据回调
bool __cdecl CVideoSockSink::OnSnapVideoData(const void * pImageData,DWORD dwDataSize,VENCODETYPE sst)
{
	if(m_bLogined)
	{
		DWORD dwCmdID=BuildCommandID(ALL_MAIN_CMD,TSVR_USER_VIDEO);
		m_pIClientSocket->SendData(dwCmdID,0,(void *)pImageData,(WORD)dwDataSize);
	}
	
	return true;
}

//录音数据回调
bool __cdecl CVideoSockSink::OnRecordData(const void * pAudioData,DWORD dwDataSize)
{	
	if(m_bLogined && !g_GlobalData.bHaveVoice) return true;

	DWORD dwCmdID=BuildCommandID(AVS_MAIN_CMD,TSVR_SERVICE_AUDIO);
	m_pIClientSocket->SendData(dwCmdID,0,(void *)pAudioData,dwDataSize);
	
	return true;
}

//网络连接消息
bool __cdecl CVideoSockSink::OnSocketConnect(int iErrorCode, LPCTSTR pszErrorDesc, IClientSocket * pIClientSocket)
{
	if(iErrorCode!=0)
	{
		switch(m_cst)
		{
		case CST_LBSVR:
			{
				SwitchLoadServerIpPort();	//切换ip
				SETTIMER(IDT_CONN_SERVER,5000,1,CST_LBSVR,(ITimerSink *)this);
				g_pIShowLogToWnd->OutputFormateToWnd(LL_WARNING,FALSE,TEXT("连接负载均衡服务器失败,%s,5s后重试！"),pszErrorDesc);
			}
			break; 
		case CST_SWTSVR:
			{
				int nConnType=CST_SWTSVR;
				if(g_GlobalData.nConnectIpType==CIPT_AVS)
				{
					SwitchLoadServerIpPort();	//切换ip
				}
				else
				{
					if(++m_dwAvsConnNum>=3)
					{
						nConnType=CST_LBSVR;
						m_dwAvsConnNum=0;
					}
				}
				//重新请求
				SETTIMER(IDT_CONN_SERVER,5000,1,CST_SWTSVR,(ITimerSink *)this);		//连接定时器
				g_pIShowLogToWnd->OutputFormateToWnd(LL_WARNING,FALSE,TEXT("第%u次连接消息服务器失败,%s,5s后重试！"),m_dwAvsConnNum,pszErrorDesc);
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

//网络读取消息
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

//网络关闭消息
bool __cdecl CVideoSockSink::OnSocketClose(IClientSocket * pIClientSocket, bool bCloseByServer)
{
	if(m_cst==CST_SWTSVR)
	{
		m_bLogined=FALSE;
		g_pIShowLogToWnd->OutputLogToWnd(TEXT("连接到服务器连接关闭,5s后重连..."),LL_WARNING);
		SETTIMER(IDT_CONN_SERVER,5000,1,CST_SWTSVR,(ITimerSink *)this);
	}

	return true;
}

//处理心跳测试
bool CVideoSockSink::OnProcActiveTest(IClientSocket * pIClientSocket)
{
	DWORD dwCmdID=BuildCommandID(ALL_MAIN_CMD,ACITVE_TEST_RESP);
	pIClientSocket->SendData(dwCmdID,0,NULL,0);
	return true;
}

//请求服务器地址
bool CVideoSockSink::RequestSvrIPPort()
{
	g_pIShowLogToWnd->OutputLogToWnd(TEXT("连接负载均衡服务器成功，请求媒体服务器数据..."));

	BYTE cbBuffer[12];
	BYTE *pData=cbBuffer;

	WORD wSendSize=0;
	pData=WriteDword(pData,ST_SWTSVR,wSendSize);
	pData=WriteDword(pData,g_GlobalData.mDevSnData.dwServiceCode,wSendSize);

	DWORD dwCmdID=BuildCommandID(ALL_MAIN_CMD,LBS_SC_REQ_SVRIP);
	
	return m_pIClientSocket->SendData(dwCmdID,0,cbBuffer,wSendSize);
}

//处理请求服务器地址Response
bool CVideoSockSink::OnProcRequestSvrIPPortResp(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize)
{
	ASSERT(dwDataSize==10);
	if(dwDataSize!=10)
	{
		g_pILogOutput->OutputFormateLog(TEXT("获取媒体流服器地址数据大小非法[%u]"),dwDataSize);
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
	g_pILogOutput->OutputFormateLog(TEXT("获取媒体流服器地址:[%s:%d]"),pTmpIP,g_GlobalData.wAvSvrPort);

	if(dwSvrIp==0)
	{ 
		g_pIShowLogToWnd->OutputFormateToWnd(LL_WARNING,FALSE,TEXT("第%u次获取媒体流服务器地址失败，5s后重试..."),++m_dwFailNum);	
		SETTIMER(IDT_CONN_SERVER,5000,1,CST_LBSVR,(ITimerSink *)this);
		return false;
	}
	if(g_GlobalData.wAvSvrPort==0)
	{
		g_pIShowLogToWnd->OutputFormateToWnd(LL_WARNING,FALSE,TEXT("第%u次获取媒体流服务器端口失败，5s后重试..."),++m_dwFailNum);
		SETTIMER(IDT_CONN_SERVER,5000,1,CST_LBSVR,(ITimerSink *)this);
		return false;		 	
	}
	m_dwFailNum=0;
	g_pIShowLogToWnd->OutputLogToWnd(TEXT("请求服务器地址成功"));

	SETTIMER(IDT_CONN_SERVER,25,1,ST_SWTSVR,(ITimerSink *)this);		//连接到媒体流服务器定时器
	return false;
}

//登录到媒体流服务器
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

//处理登录服务器Response
bool CVideoSockSink::OnProcLoginToServerResp(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize)
{
	__int32 nResult=0;
	DWORD dwRoomIndex=0;

	BYTE *pData=(BYTE *)pDataBuffer;
	pData=ReadInt32(pData,nResult);
	if(nResult!=0)
	{
		g_pIShowLogToWnd->OutputFormateToWnd(LL_WARNING,FALSE,TEXT("登录到媒体流服务器失败,错误码：%d"),nResult);
		return false;
	}

	m_bLogined=TRUE;
	g_pIShowLogToWnd->OutputLogToWnd(TEXT("登录到媒体流服务器成功"));

	DWORD *pDataAddr=&m_dwDataSize;
	SETTIMER(IDT_SHOW_DATA_RATE,1000,TIMER_REPEAT_TIMER,(WPARAM)pDataAddr,NULL);

	#if READ_TEST_FILE
	SETTIMER(IDT_TEST_VIDEO_FILES,2000,1,NULL,(ITimerSink *)this);
	#endif

	return true;
}

//定时器事件
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
		mysprintf(szMsg,MAX_PATH,TEXT("客户[%u]请求视频辅助,是否同意?"),dwClientCode);
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

//处理H264SpsPps数据
bool CVideoSockSink::OnProcH264SpsPpsData(void *pDataBuffer,DWORD dwDataSize)
{
	#if WRITE_FILE
	m_vDataFile.OutputDataToFile(&dwDataSize,sizeof(dwDataSize));
	m_vDataFile.OutputDataToFile(pDataBuffer,(int)dwDataSize);
	#endif
	if(m_pIVideoManage!=NULL)
	{
		m_pIVideoManage->SetH264SpsPpsData((const BYTE *)pDataBuffer,(WORD)dwDataSize);
		m_pIVideoManage->GetEventAreaSize(m_nOffsetX,m_nOffsetY,m_nVideoCX,m_nVideoCY,m_nWndCX,m_nWndCY);	//获取视频大小
		if(!g_GlobalData.bServicing)
		{
			SeriralClientCode(false);
			g_GlobalData.bServicing=TRUE;	//继续服务中
			g_pIConfirmToWnd->OutputLogAndValueToWnd(TEXT("正在视频辅助中..."),g_GlobalData.udwClientCode,LL_NORMAL,FALSE);
		}
	}

	g_pILogOutput->OutputLogToFile(TEXT("接收到H264SpsPps数据，等待视频关键帧..."));
	return true;
}

//处理H264视频数据
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
//处理客户音频数据
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

//发送鼠标事件
bool __cdecl CVideoSockSink::SendMouseEventData(int nEventType,int x,int y)
{
	if(!m_bLogined) return false;

	//m_nOffsetX,m_nOffsetY,m_nVideoCX,m_nVideoCY
	if(m_nVideoCX==0 || m_nVideoCY==0)
		return false;
	if(m_nWndCX==0 || m_nWndCY==0)
		return false;
	//计算坐
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

//发送键盘事件
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

//发送选择结果
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
		g_pIShowLogToWnd->OutputFormateToWnd(LL_NORMAL,FALSE,TEXT("停止与客户%llu视频辅助"),udwClientCode);		
	}
	else 
	{
		if(nOptValue<0 || nOptValue==HOPT_REJECT)
		{
			nResult=SC_HOPT_REJECT;
			m_pIAudioModuleManage->SetPostRecordData(FALSE);
			m_pIVideoManage->OnCustomExitNotify();
			g_pIShowLogToWnd->OutputFormateToWnd(LL_NORMAL,FALSE,TEXT("拒绝客户%llu视频辅助请求"),udwClientCode);
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
				g_pIShowLogToWnd->OutputFormateToWnd(LL_NORMAL,FALSE,TEXT("与客户%llu视频辅助,但没有音频服务"),udwClientCode);
			}
			g_pIConfirmToWnd->OutputFormateToWnd(LL_NORMAL,FALSE,TEXT("正在视频辅助中..."));
			g_pIShowLogToWnd->OutputFormateToWnd(LL_NORMAL,FALSE,TEXT("开始与客户%llu进行视频辅助"),udwClientCode);
			SeriralClientCode(true);	//序列化
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

//切换负载IP
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

//处理请求辅助确认消息
bool CVideoSockSink::OnProcReqHelperConfirm(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize)
{
	if(dwDataSize<sizeof(UNDWORD))
		return true;

	UNDWORD udwClientCode=0;
	BYTE *pData=(BYTE *)pDataBuffer;
	pData=ReadUNDword(pData,udwClientCode);

	TRACE(TEXT("----收到客户[%llu]请求视频辅助\n"),udwClientCode);
	int nResult=SERVICE_WORKING;
	if(g_GlobalData.bConfirming==TRUE)
	{
		SendSelectedResult(udwClientCode,nResult);		//如果用户正在确认中,第二个请求直接返回
		g_pILogOutput->OutputFormateLog(TEXT("用户[%llu]请求辅助确认,当前客户正在确认上一个用户[%llu]中."),udwClientCode,m_udwLastConfirm);
		return true;
	}
	
	if(g_GlobalData.bServicing || udwClientCode==0)	//服务中
	{
		if(g_GlobalData.udwClientCode==udwClientCode)
		{
			nResult=0;
			g_pIShowLogToWnd->OutputFormateToWnd(LL_NORMAL,FALSE,TEXT("正在与客户%llu进行通讯,客户重入"),udwClientCode);
		}
		else
		{
			g_pIShowLogToWnd->OutputFormateToWnd(LL_WARNING,FALSE,TEXT("服务中,拒绝客户%llu辅助请求"),udwClientCode);
		}
		SendSelectedResult(udwClientCode,nResult);
	}
	else
	{
		g_GlobalData.bConfirming=TRUE;
		TCHAR szMsg[MAX_PATH]=TEXT("");
		mysprintf(szMsg,MAX_PATH,TEXT("客户[%llu]请求视频辅助,是否同意?"),udwClientCode);
		if(!g_GlobalData.bIsMinWnd && !g_GlobalData.bWndNotActive)
			g_pIConfirmToWnd->OutputLogAndValueToWnd(szMsg,udwClientCode,LL_NORMAL,TRUE);
		else
			g_pTopicNotify->OutputLogAndValueToWnd(szMsg,udwClientCode,LL_NORMAL,TRUE);
	}
	return true;
}

//处理客户退出通知
bool CVideoSockSink::OnProcCustomExitNotify(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize)
{
	if(dwDataSize<sizeof(UNDWORD))
		return true;

	UNDWORD udwClientCode=0;
	BYTE *pData=(BYTE *)pDataBuffer;
	pData=ReadUNDword(pData,udwClientCode);
	
	if(g_GlobalData.udwClientCode==udwClientCode)
	{
		//提示客户退出
		g_GlobalData.bServicing=FALSE;
		m_pIVideoManage->OnCustomExitNotify();		
		m_pIAudioModuleManage->SetPostRecordData(FALSE);
	}
	g_pIShowLogToWnd->OutputFormateToWnd(LL_WARNING,FALSE,TEXT("客户%llu退出视频辅助"),udwClientCode);
	return true;
}

//处理停止辅助Response
bool CVideoSockSink::OnProcStopHelperResp(void *pDataBuffer,DWORD dwDataSize)
{
	m_pIVideoManage->OnCustomExitNotify();
	return true;
}

//序列化最后客户号
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
//读取测试数据
void CVideoSockSink::ReadVideoTestData()
{
	DWORD dwDataSize=0;
	CMyFile myFile;
	TCHAR *pTestFile=TEXT("E:\\projects\\record_screen_demo\\测试数据\\0003\\27308_0605192058_1438368.tmp");
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
