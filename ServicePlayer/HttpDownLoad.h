////////////////////////////////////////////////////////////////////////////////////
//		Http数据下载类头文件 HttpDownLoad.h
////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "localdef.h"

////////////////////////////////////////////////////////////////////////////////////
class CHttpDownLoad:public CServiceThread, public IFileTransportSink
{
private:
	WPARAM						m_wDownParam;					//下载参数
	IHttpDataSink				*m_pIHttpDataSink;				//Http数据回调接口
	HANDLE						m_hCompletePort;				//完成端口
	HANDLE						m_hWaitDown;					//下载等待事件

private:
	CTransportManageHelper		m_TransManageHelper;			//传输管理辅助类对象
	ITransportManage			*m_pITransportManage;			//传输接口

public:
	CHttpDownLoad(void);
	~CHttpDownLoad(void);

public:
	//释放对象
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//是否有效
	virtual bool __cdecl IsValid() { return this!=NULL; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

public:
	//初始化组件
	bool InitHttpDown(HANDLE hCompletePort,IHttpDataSink *pIHttpDataSink);

public:
	//传输进度
	virtual void __cdecl OnTransportProgress(float fProgress,TRANSPORTTYPE tt);
	//传输完成通知
	virtual void __cdecl OnTransportFinish(int nErrCode,LPCSTR lpszUrl,TRANSPORTTYPE tt,WPARAM wParam);

protected:
	//开始事件
	virtual bool OnThreadStartEvent();
	//停止事件
	virtual bool OnThreadStopEvent();

	//内部函数
private:
	//运行函数
	virtual bool RepetitionRun();


};

