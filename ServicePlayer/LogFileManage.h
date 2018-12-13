///////////////////////////////////////////////////////////////////////////////////
//			Log �ļ�������ͷ�ļ�  LogFileManage.h
///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "localdef.h"

///////////////////////////////////////////////////////////////////////////////////
class CLogFileManage
{
private:
	DWORD					m_dwProccesID;				//����ID
	int						m_nRecordLine;				//��¼����
	BOOL					m_bLogTime;					//��־ʱ���־
	HANDLE					m_hMyFile;					//�ļ����
	DWORD					m_dwFileIndex;				//�ļ�����
	DWORD					m_dwOutCount;				//�����ļ�����
	DWORD					m_dwStartTime;				//��ʼʱ��
	TCHAR					m_szFileName[MAX_PATH];		//��ǰ�ļ���
	TCHAR					m_szExName[8];				//��չ��
	TCHAR					m_szPrevName[12];			//ǰ׺
	TCHAR					m_szLogPath[MAX_PATH];		//
	TCHAR					m_szTmpExName[8];			//��ʱ��չ��

private:
	CThreadLock				m_ThreadLock;				//

public:
	CLogFileManage(void);
	~CLogFileManage(void);

public:
	//�����Ϣ���ļ�
	void OutputLogToFile(LPCTSTR lpszMsg,emLogLevel ll=LL_NORMAL);
	//�����Ϣ���ļ�
	void OutputLogMsg(emLogLevel ll,LPCTSTR lpszMsg,...);
	//�������ݵ��ļ�
	void OutputDataToFile(const void * pDstData,int nDataSize);
	//�ر��ļ�
	void CloseOutFile();
	//������չ��
	void SetExName(LPCTSTR lpszExName);
	//����ǰ׺
	void SetPrevName(LPCTSTR lpszPrevName);
	//����ÿ���ļ�����
	void SetLinePreFile(int nLine){m_nRecordLine=nLine;}
	//������־ʱ���־
	void SetLogTimeFlag(BOOL bOutTime){m_bLogTime=bOutTime;}
	//������־·��
	void SetLogPath(LPCTSTR lpszLogPath){lstrcpy(m_szLogPath,lpszLogPath);}
	//���ü�ʱ��չ��
	void SetTmpExName(LPCTSTR lpszTmpExName){lstrcpy(m_szTmpExName,lpszTmpExName);}

private:
	//�����ļ���
	void BuildFileName();
	
};
//////////////////////////////////////////////////////////////////////////////////////////////
extern CLogFileManage g_LogManage;
