////////////////////////////////////////////////////////////////////////////////////
//		Http数据下载类实现文件 HttpDownLoad.cpp
////////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "HttpDownLoad.h"
#include "localdef.h"

////////////////////////////////////////////////////////////////////////////////////
CHttpDownLoad::CHttpDownLoad(void)
{
	m_wDownParam=0;
	m_pIHttpDataSink=NULL;
	m_hCompletePort=NULL;
	m_hWaitDown=NULL;
}

CHttpDownLoad::~CHttpDownLoad(void)
{
	if(m_hWaitDown!=NULL)
	{
		::CloseHandle(m_hWaitDown);
		m_hWaitDown=NULL;
	}
}

//接口查询
void * __cdecl CHttpDownLoad::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IFileTransportSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IFileTransportSink,Guid,dwQueryVer);
	return NULL;
}

//初始化组件
bool CHttpDownLoad::InitHttpDown(HANDLE hCompletePort,IHttpDataSink *pIHttpDataSink)
{
	m_pIHttpDataSink=pIHttpDataSink;
	if(!m_TransManageHelper.CreateInstance())
	{
		g_pILogOutput->OutputLogToFile(m_TransManageHelper.GetErrorMessage());
		return false;
	}
	m_hCompletePort=hCompletePort;
	m_pITransportManage=m_TransManageHelper.GetInterface();	

	if(m_pITransportManage!=NULL)
	{
		IUnknownEx *pIUnknownEx=GET_MYSELF_INTERFACE(IUnknownEx);
		m_pITransportManage->InitTransportManage(g_GlobalData.szSavePath,pIUnknownEx);
	}

	m_hWaitDown=::CreateEvent(NULL,FALSE,FALSE,NULL);
	if(m_hWaitDown==NULL) return false;
	
	return (m_pITransportManage!=NULL);
}

//传输进度
void __cdecl CHttpDownLoad::OnTransportProgress(float fProgress,TRANSPORTTYPE tt)
{
}

//传输完成通知
void __cdecl CHttpDownLoad::OnTransportFinish(int nErrCode,LPCSTR lpszUrl,TRANSPORTTYPE tt,WPARAM wParam)
{
	g_pILogOutput->OutputFormateLog(TEXT("CHttpDownLoad::OnTransportFinish下载完成,ErrCode:%d,Url:%s"),nErrCode,lpszUrl);

	void * pOwnObject=this;
	LPCSTR lpszJson=lpszUrl;
	if(nErrCode!=0 && lpszUrl==NULL)
	{
		pOwnObject=NULL;
		lpszJson=NULL;
	}
	
	ASSERT(wParam==m_wDownParam);
	if(m_pIHttpDataSink!=NULL)
	{
		m_pIHttpDataSink->HttpCompleteNotify(nErrCode,lpszJson,wParam,pOwnObject);
	}

	::SetEvent(m_hWaitDown);
}

//开始事件
bool CHttpDownLoad::OnThreadStartEvent()
{
	if(m_hCompletePort==NULL) return false;
	return true;
}
//停止事件
bool CHttpDownLoad::OnThreadStopEvent()
{
	return true;
}

//运行函数
bool CHttpDownLoad::RepetitionRun()
{
	//变量定义
	DWORD dwThancferred=0;
	OVERLAPPED * pOverLapped=NULL;
	IHttpDataSink * pIHttpDataSink=NULL;
	HANDLE hCompPort=m_hCompletePort;

	//等待完成端口
	BOOL bSuccess=GetQueuedCompletionStatus(hCompPort,&dwThancferred,(PULONG_PTR)&pIHttpDataSink,&pOverLapped,INFINITE);
	if((bSuccess==FALSE)&&(GetLastError()!=ERROR_NETNAME_DELETED)) return false;
	//判断退出
	if(pIHttpDataSink==NULL) return false;

	DOWNPARAM *pDownParam=NULL;
	pDownParam=m_pIHttpDataSink->GetHttpTransData();
	if(!pDownParam)	return m_bRun;

	m_wDownParam=(WPARAM)pDownParam;

	g_pILogOutput->OutputFormateLog(TEXT("CHttpDownLoad::RepetitionRun获取下载参数成功:0x%x"),pDownParam);

	if(m_pITransportManage!=NULL && m_bRun)
	{
		g_pILogOutput->OutputFormateLog(TEXT("CHttpDownLoad::RepetitionRun开始下载请求:%s"),pDownParam->szDownUrl);

		m_pITransportManage->StartHttpPost(pDownParam->szDownUrl,pDownParam->mJson.data,pDownParam->mJson.len,m_wDownParam,"application/json");
		::WaitForSingleObject(m_hWaitDown,INFINITE);	//等待下载完成
		//::WaitForSingleObject(m_hWaitDown,100);	//间隔100ms下载下一个请求
		g_pILogOutput->OutputFormateLog(TEXT("CHttpDownLoad::RepetitionRun下载请求完成:%s"),pDownParam->szDownUrl);
	}
	return m_bRun;
}