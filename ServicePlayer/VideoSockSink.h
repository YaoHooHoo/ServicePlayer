//////////////////////////////////////////////////////////////////////////////////////////
//		��Ļ�����ص���ͷ�ļ� VideoSockSink.h.h
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "localdef.h"
#include "SkinImage.h"
#include "TimerEngine.h"
#include "LogFileManage.h"

////////////////////////////////////////////////////////////////////////////
#define DIS_READ		0					//ֱ�Ӷ�ȡav�ļ�
#define DIS_REC			1					//ֱ��¼��
//////////////////////////////////////////////////////////////////////////////////////////
class CVideoSockSink:public IClientSocketSink,public ITimerSink,public ISendMouseEvent,public IAudioDataSink
{
private:
	bool						m_bInited;						//��ʼ����־
	BOOL						m_bLogined;						//������־
	int							m_nOffsetX;						//Xƫ����
	int							m_nOffsetY;						//Yƫ����
	int							m_nVideoCX;						//��Ƶ��
	int							m_nVideoCY;						//��Ƶ��
	int							m_nWndCX;						//���ڿ�
	int							m_nWndCY;						//���ڸ�
	DWORD						m_dwAvsConnNum;					//Avserver���Ӵ���
	CONNECTSVRTYPE				m_cst;							//���ӷ���������
	DWORD						m_dwFailNum;					//ʧ�ܴ���
	UNDWORD						m_udwLastConfirm;				//���һ��ȷ��

private:
	IVideoManage				*m_pIVideoManage;				//��Ƶ����ӿ�
	IAudioModuleManage			*m_pIAudioModuleManage;			//��Ƶ����ӿ�

private:
	CSingleSockClientHelper		m_SocketClientHelper;			//�������
	IClientSocket				*m_pIClientSocket;				//����ӿ�

private:
	BYTE						*m_pSpsData;					//Sps����
	DWORD						m_dwSpsSize;					//Sps���ݴ�С
	DWORD						m_dwDataSize;					//���ݴ�С
	
#if WRITE_FILE
private:
	CLogFileManage				m_vDataFile;					//��Ƶ�ļ�
#endif

public:
	CVideoSockSink(void);
	~CVideoSockSink(void);

public:
	//�ͷŶ���
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//�Ƿ���Ч
	virtual bool __cdecl IsValid() { return this!=NULL; }
	//�ӿڲ�ѯ
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

public:
	//��ʼ����Ƶ�������
	bool InitVideoSockSink(IVideoManage *pIVideoManage);
	//������Ƶ����ӿ�
	bool SetAudioModuleManage(IAudioModuleManage *pIAudioModuleManage);
	//���ӵ�������
	bool ConnectToServer(CONNECTSVRTYPE cst);
	//��ʼ��Ƶ��������
	bool StartVideoSocket();
	//ֹͣ��Ƶ��������
	bool StopVideoSocket();
	//��ȡ���ݴ�С
	DWORD GetVideoDataSize() const {return m_dwDataSize;}

public:
	//H264SpsPps����
	virtual bool __cdecl OnH264SpsPpsData(const void * pHeadData,DWORD dwDataSize);
	//������Ƶ���ݻص�
	virtual bool __cdecl OnSnapVideoData(const void * pImageData,DWORD dwDataSize,VENCODETYPE sst);
	//¼�����ݻص�
	virtual bool __cdecl OnRecordData(const void * pAudioData,DWORD dwDataSize);

public:
	//����������Ϣ
	virtual bool __cdecl OnSocketConnect(int iErrorCode, LPCTSTR pszErrorDesc, IClientSocket * pIClientSocket);
	//�����ȡ��Ϣ
	virtual bool __cdecl OnSocketRead(CMD_Command Command, void * pBuffer, DWORD dwDataSize, IClientSocket * pIClientSocket);
	//����ر���Ϣ
	virtual bool __cdecl OnSocketClose(IClientSocket * pIClientSocket, bool bCloseByServer);
	//��ʱ���¼�
	virtual bool __cdecl OnEventTimer(WORD wTimerID,WPARAM wBindParam);
	//��������¼�
	virtual bool __cdecl SendMouseEventData(int nEventType,int x,int y);
	//���ͼ����¼�
	virtual bool __cdecl SendKeyboardEvent(int nKeyValue,int nKeyType);
	//����ѡ����
	virtual void __cdecl SendSelectedResult(UNDWORD udwClientCode,int nOptValue);

private:
	//�����������ַ
	bool RequestSvrIPPort();
	//��¼�����÷�����
	bool LoginToAvsSvr();
	//������Ƶ�ļ�
	bool WriteVideoToFile(const void *pVideoData,DWORD dwDataSize,AVCPTYPE avType);
	//�л�����IP
	void SwitchLoadServerIpPort();

private:
	//���л����ͻ���
	void SeriralClientCode(bool bWrite);
	//��ȡ��������
	void ReadVideoTestData();

private:
	//������������
	bool OnProcActiveTest(IClientSocket * pIClientSocket);
	//���������������ַResponse
	bool OnProcRequestSvrIPPortResp(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	//�����¼������Response
	bool OnProcLoginToServerResp(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	//����H264SpsPps����
	bool OnProcH264SpsPpsData(void *pDataBuffer,DWORD dwDataSize);
	//����H264��Ƶ����
	bool OnProcH264VideoData(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	//����������ȷ����Ϣ
	bool OnProcReqHelperConfirm(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	//����ͻ��˳�֪ͨ
	bool OnProcCustomExitNotify(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	//����ͻ���Ƶ����
	bool OnProcUserAudioData(DWORD dwSequenceID,void *pDataBuffer,DWORD dwDataSize);
	//����ֹͣ����Response
	bool OnProcStopHelperResp(void *pDataBuffer,DWORD dwDataSize);
};

