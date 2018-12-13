////////////////////////////////////////////////////////////////////////////////////
//		Http数据下载管理类实现文件 HttpDataManage.cpp
//		2016-08-04 Bowen.hu
////////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "HttpDataManage.h"

IHttpDownManage *g_pIHttpDownManage;
////////////////////////////////////////////////////////////////////////////////////
CHttpDataManage::CHttpDataManage(void)
{
	m_hCompletePort=NULL;
	g_pIHttpDownManage=this;
}

CHttpDataManage::~CHttpDataManage(void)
{
	//关闭完成端口
	if (m_hCompletePort!=NULL)
	{
		CloseHandle(m_hCompletePort);
		m_hCompletePort=NULL;
	}
}

//初始化Http管理
bool CHttpDataManage::InitHttpManage()
{
	//建立完成端口
	m_hCompletePort=CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,1);
	if (m_hCompletePort==NULL) return false;
	IHttpDataSink *pIHttpDataSink=(IHttpDataSink *)this;
	CHttpDownLoad *pDownThread=NULL;
	for(int i=0;i<HTTP_DOWN_NUM;i++)
	{
		pDownThread=new CHttpDownLoad();
		ASSERT(pDownThread!=NULL);
		if(pDownThread==NULL) return false;
		pDownThread->InitHttpDown(m_hCompletePort,pIHttpDataSink);
		m_HttpDownList.push_back(pDownThread);
	}

	return true;
}

//开始下载服务
bool CHttpDataManage::StartDownService()
{
	if(m_hCompletePort==NULL) return false;

	CHttpDownLoad *pDownThread=NULL;
	for(CHttpDownListIt it=m_HttpDownList.begin();it!=m_HttpDownList.end();it++)
	{
		pDownThread=(*it);
		pDownThread->StartThread();
	}
	return true;
}

//停止下载服务
bool CHttpDataManage::StopDownService()
{
	for(int i=0;i<HTTP_DOWN_NUM;i++)
	{
		//停止线程
		if (m_hCompletePort!=NULL)
			PostQueuedCompletionStatus(m_hCompletePort,0,NULL,NULL); 
	}

	CHttpDownLoad *pDownThread=NULL;
	for(CHttpDownListIt it=m_HttpDownList.begin();it!=m_HttpDownList.end();it++)
	{
		pDownThread=(*it);
		pDownThread->StopThread(500);
	}

	return true;
}

//数据传输完成接口
bool __cdecl CHttpDataManage::HttpCompleteNotify(int nStatus,LPCSTR lpszResult,WPARAM wParam,const void * pDownObject)
{
	DOWNPARAM * pDownParam=(DOWNPARAM *)wParam;
	if(pDownParam==NULL) return false;

	WPARAM userParma=pDownParam->wParam;
	m_LogicDataManage.ParseHttpApiJson((OITLDT)pDownParam->ldt,nStatus,lpszResult,userParma,pDownParam->pIHttpEventSink);

	FREE_MY_STR(pDownParam->mJson);	//释放内存
	m_HttpDownMem.FreeAssoc(pDownParam);

	return true;
}

//获取要传输的数据
DOWNPARAM * __cdecl CHttpDataManage::GetHttpTransData()
{
	DOWNPARAM * pDownParam=NULL;
	CThreadLockHandle LockHandle(&m_DownLock);
	if(m_DownEventList.size()==0) return NULL;
	pDownParam=m_DownEventList.front();
	m_DownEventList.pop_front();
	LockHandle.UnLock();

	return pDownParam;
}

//添加下载课程列表事件
bool __cdecl CHttpDataManage::AddHttpApiEvent(const int ldt,WPARAM wParam,void *pHttpSink)
{
	string strUrl,strJson;
	g_pILogOutput->OutputFormateLog(TEXT("AddHttpApiEvent事件,ldt=%d"),ldt);
	if(!m_LogicDataManage.GetHttpApiParam((OITLDT)ldt,strUrl,strJson,wParam))
		return false;

	DOWNPARAM * pDownParam=m_HttpDownMem.NewAssoc();
	if(pDownParam==NULL) return false;

	int nUrlLength=strUrl.length();
	if(nUrlLength>=URL_LEN)
	{
		g_pILogOutput->OutputFormateLog(TEXT("AddHttpApiEvent,Url[%s]"),strUrl.c_str());
		AfxMessageBox("请求的URL太长，可能会请求失败");
		nUrlLength=URL_LEN-1;
	}

	DWORD dwCount=0;
	memcpy(pDownParam->szDownUrl,strUrl.c_str(),nUrlLength);
	pDownParam->szDownUrl[nUrlLength]=0;
	pDownParam->wParam=wParam;
	pDownParam->ldt=ldt;
	pDownParam->pIHttpEventSink=pHttpSink;
	SET_MY_STR(pDownParam->mJson,strJson.c_str(),strJson.length());

	CThreadLockHandle LockHandle(&m_DownLock);
	m_DownEventList.push_back(pDownParam);
	dwCount=m_HttpDownList.size();
	LockHandle.UnLock();

	g_pILogOutput->OutputFormateLog(TEXT("AddHttpApiEvent,开始投递Http请求参数,dwCount=%u"),dwCount);

	IHttpDataSink * pIHttpDataSink=(IHttpDataSink *)this;
	::PostQueuedCompletionStatus(m_hCompletePort,dwCount,(ULONG_PTR)pIHttpDataSink,NULL);

	return true;
}

//清除未下载数据
bool __cdecl CHttpDataManage::ClearHttpApiEvent()
{
	DOWNPARAM * pDownParam=NULL;
	CThreadLockHandle LockHandle(&m_DownLock);
	if(m_DownEventList.size()==0) return NULL;
	for(CDownEventListIt it=m_DownEventList.begin();it!=m_DownEventList.end();it++)
	{
		pDownParam=(*it);
		FREE_MY_STR(pDownParam->mJson);	//释放内存
		m_HttpDownMem.FreeAssoc(pDownParam);
		pDownParam=NULL;
	}
	m_DownEventList.clear();
	LockHandle.UnLock();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////
IHttpDownManage *AfxGetHttpDownManage()
{
	return g_pIHttpDownManage;
}
