////////////////////////////////////////////////////////////////////////////////////
//		Http数据下载管理类头文件 HttpDataManage.h
//		2016-08-04 Bowen.hu
////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "localdef.h"
#include "HttpDownLoad.h"
#include "LogicDataManage.h"

////////////////////////////////////////////////////////////////////////////////////
typedef list<DOWNPARAM *>				CDownEventList;
typedef list<DOWNPARAM *>::iterator		CDownEventListIt;
typedef CMemManage<DOWNPARAM>			CHttpDownMem;
typedef list<CHttpDownLoad*>			CHttpDownList;
typedef list<CHttpDownLoad*>::iterator	CHttpDownListIt;
////////////////////////////////////////////////////////////////////////////////////
class CHttpDataManage: public IHttpDataSink,public IHttpDownManage
{
private:
	HANDLE					m_hCompletePort;				//完成端口
	CDownEventList			m_DownEventList;				//下载列表
	CHttpDownMem			m_HttpDownMem;					//下载缓冲
	CThreadLock				m_DownLock;						//下载锁对象

private:
	CHttpDownList			m_HttpDownList;					//下载线程列表

private:
	CLogicDataManage		m_LogicDataManage;				//逻辑管理

public:
	CHttpDataManage(void);
	~CHttpDataManage(void);

public:
	//初始化Http管理
	bool InitHttpManage();
	//开始下载服务
	bool StartDownService();
	//停止下载服务
	bool StopDownService();

public:

public:
	//获取要传输的数据
	virtual DOWNPARAM * __cdecl GetHttpTransData();
	//数据传输完成接口
	virtual bool __cdecl HttpCompleteNotify(int nStatus,LPCSTR lpszResult,WPARAM wParam,const void * pDownObject);	
	//添加HttpApi事件
	virtual bool __cdecl AddHttpApiEvent(const int ldt,WPARAM wParam,void *pHttpSink);
	//清除未下载数据
	virtual bool __cdecl ClearHttpApiEvent();

};

