//////////////////////////////////////////////////////////////////////////
//		��ʱ��������ͷ�ļ� TimerEngine.h
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <map>
#include "localdef.h"

/////////////////////////////////////////////////////////////////////////////
#define TIMER_REPEAT_TIMER			DWORD(-1)			//�ظ�����

//��˵��
class CTimerEngine;

///////////////////////////////////////////////////////////////////////////
interface ITimerSink
{
	//��ʱ���¼�
	virtual bool __cdecl OnEventTimer(WORD wTimerID,WPARAM wBindParam)=0;
};

//��ʱ���ص��ӿ�
interface ITimerEngineSink
{
	//��ʱ���¼�
	virtual bool __cdecl OnEventTimer(WORD wTimerID,WPARAM wBindParam,void * pTimerSink)=0;
};

/////////////////////////////////////////////////////////////////////////
interface ITimerEngine
{
	//���ö�ʱ��
	virtual bool __cdecl SetTimer(WORD wTimerID,DWORD dwElapse,DWORD dwRepeat,WPARAM wParam)=0;
	//���ö�ʱ��
	virtual bool __cdecl SetTimer(WORD wTimerID,DWORD dwElapse,DWORD dwRepeat,WPARAM wParam,void * pTimerSink)=0;
	//ɾ����ʱ��
	virtual bool __cdecl KillTimer(WORD wTimerID)=0;
	//ɾ�����ж�ʱ��
	virtual bool __cdecl KillAllTimer()=0;
};

interface ITimerEngineManager
{
	//��ʼ����
	virtual bool __cdecl StartService()=0;
	//ֹͣ����
	virtual bool __cdecl StopService()=0;
	//���ýӿ�
	virtual bool __cdecl SetTimerEngineSink(ITimerEngineSink * pITimerEngineSink)=0;
};

//////////////////////////////////////////////////////////////////////////

//��ʱ���߳�
class CTimerThread : public CServiceThread
{
	//��������
protected:
	DWORD								m_dwTimerSpace;					//ʱ����
	CTimerEngine						* m_pTimerEngine;				//��ʱ������

	//��������
public:
	//���캯��
	CTimerThread(void);
	//��������
	virtual ~CTimerThread(void);

	//���ܺ���
public:
	//���ú���
	bool InitThread(CTimerEngine * pTimerEngine, DWORD dwTimerSpace);

	//���غ���
private:
	//���к���
	virtual bool RepetitionRun();
};

//////////////////////////////////////////////////////////////////////////

//��ʱ������
struct tagTimerItem
{
	WORD								wTimerID;						//��ʱ�� ID
	DWORD								dwElapse;						//��ʱʱ��
	DWORD								dwTimeLeave;					//����ʱ��
	DWORD								dwRepeatTimes;					//�ظ�����
	WPARAM								wBindParam;						//�󶨲���
	void								*pTimerSink;					//�ص��ӿ�
};

//��ʱ���������鶨��
typedef std::map<WORD,tagTimerItem *>					CTimerItemMap;
typedef std::map<WORD,tagTimerItem *>::iterator			CTimerItemMapIt;
typedef std::vector<tagTimerItem *>						CTimerItemVector;
typedef std::vector<tagTimerItem *>::iterator			CTimerItemVectorIt;
typedef std::map<ITimerEngineSink *,DWORD>				CTimerSinkMap;
typedef std::map<ITimerEngineSink *,DWORD>::iterator	CTimerSinkMapIt;
//////////////////////////////////////////////////////////////////////////

//��ʱ������
class CTimerEngine :public CServiceThread ,public ITimerEngine, public ITimerEngineManager
{
	friend class CTimerThread;

	//���ö���
protected:
	DWORD								m_dwTimerSpace;					//ʱ����
	HANDLE								m_hComplatePort;				//��ɶ˿ھ��

	//״̬����
protected:
	bool								m_bService;						//���б�־
	DWORD								m_dwTimePass;					//����ʱ��
	DWORD								m_dwTimeLeave;					//����ʱ��
	CTimerItemVector					m_TimerItemFree;				//��������
	CTimerItemMap						m_TimerItemActive;				//�����

	//�������
protected:
	CThreadLock							m_ThreadLock;					//�߳���
	CTimerThread						m_TimerThread;					//��ʱ���߳�
	CTimerSinkMap						m_TimerSinkMap;					//�ص��ӿ�map����

	//��������
public:
	//���캯��
	CTimerEngine(void);
	//��������
	virtual ~CTimerEngine(void);


	//�ӿں���
public:
	//���ö�ʱ��
	virtual bool __cdecl SetTimer(WORD wTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wParam);
	//���ö�ʱ��
	virtual bool __cdecl SetTimer(WORD wTimerID,DWORD dwElapse,DWORD dwRepeat,WPARAM wParam,void * pTimerSink);
	//ɾ����ʱ��
	virtual bool __cdecl KillTimer(WORD wTimerID);
	//ɾ����ʱ��
	virtual bool __cdecl KillAllTimer();

	//����ӿ�
public:
	//��ʼ����
	virtual bool __cdecl StartService();
	//ֹͣ����
	virtual bool __cdecl StopService();
	//���ýӿ�
	virtual bool __cdecl SetTimerEngineSink(ITimerEngineSink * pITimerEngineSink);

	//�ڲ�����
private:
	//��ʱ��֪ͨ
	void OnTimerThreadSink();
	//��ʱ���ص�
	void OnTimerEventSink(WORD wTimerID,WPARAM wParam,void * pTimerSink);

	//���غ���
private:
	//���к���
	virtual bool RepetitionRun();
};

extern ITimerEngine *g_pITimerEngine;		//��ʱ���ӿ�

