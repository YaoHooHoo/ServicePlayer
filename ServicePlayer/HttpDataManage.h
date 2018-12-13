////////////////////////////////////////////////////////////////////////////////////
//		Http�������ع�����ͷ�ļ� HttpDataManage.h
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
	HANDLE					m_hCompletePort;				//��ɶ˿�
	CDownEventList			m_DownEventList;				//�����б�
	CHttpDownMem			m_HttpDownMem;					//���ػ���
	CThreadLock				m_DownLock;						//����������

private:
	CHttpDownList			m_HttpDownList;					//�����߳��б�

private:
	CLogicDataManage		m_LogicDataManage;				//�߼�����

public:
	CHttpDataManage(void);
	~CHttpDataManage(void);

public:
	//��ʼ��Http����
	bool InitHttpManage();
	//��ʼ���ط���
	bool StartDownService();
	//ֹͣ���ط���
	bool StopDownService();

public:

public:
	//��ȡҪ���������
	virtual DOWNPARAM * __cdecl GetHttpTransData();
	//���ݴ�����ɽӿ�
	virtual bool __cdecl HttpCompleteNotify(int nStatus,LPCSTR lpszResult,WPARAM wParam,const void * pDownObject);	
	//���HttpApi�¼�
	virtual bool __cdecl AddHttpApiEvent(const int ldt,WPARAM wParam,void *pHttpSink);
	//���δ��������
	virtual bool __cdecl ClearHttpApiEvent();

};

