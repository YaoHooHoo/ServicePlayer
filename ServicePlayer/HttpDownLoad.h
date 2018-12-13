////////////////////////////////////////////////////////////////////////////////////
//		Http����������ͷ�ļ� HttpDownLoad.h
////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "localdef.h"

////////////////////////////////////////////////////////////////////////////////////
class CHttpDownLoad:public CServiceThread, public IFileTransportSink
{
private:
	WPARAM						m_wDownParam;					//���ز���
	IHttpDataSink				*m_pIHttpDataSink;				//Http���ݻص��ӿ�
	HANDLE						m_hCompletePort;				//��ɶ˿�
	HANDLE						m_hWaitDown;					//���صȴ��¼�

private:
	CTransportManageHelper		m_TransManageHelper;			//��������������
	ITransportManage			*m_pITransportManage;			//����ӿ�

public:
	CHttpDownLoad(void);
	~CHttpDownLoad(void);

public:
	//�ͷŶ���
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//�Ƿ���Ч
	virtual bool __cdecl IsValid() { return this!=NULL; }
	//�ӿڲ�ѯ
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

public:
	//��ʼ�����
	bool InitHttpDown(HANDLE hCompletePort,IHttpDataSink *pIHttpDataSink);

public:
	//�������
	virtual void __cdecl OnTransportProgress(float fProgress,TRANSPORTTYPE tt);
	//�������֪ͨ
	virtual void __cdecl OnTransportFinish(int nErrCode,LPCSTR lpszUrl,TRANSPORTTYPE tt,WPARAM wParam);

protected:
	//��ʼ�¼�
	virtual bool OnThreadStartEvent();
	//ֹͣ�¼�
	virtual bool OnThreadStopEvent();

	//�ڲ�����
private:
	//���к���
	virtual bool RepetitionRun();


};

