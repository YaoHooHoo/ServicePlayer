///////////////////////////////////////////////////////////////////////////////////
//			Log 文件管理类头文件  LogFileManage.h
///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "localdef.h"

///////////////////////////////////////////////////////////////////////////////////
class CLogFileManage
{
private:
	DWORD					m_dwProccesID;				//进程ID
	int						m_nRecordLine;				//记录行数
	BOOL					m_bLogTime;					//日志时间标志
	HANDLE					m_hMyFile;					//文件句柄
	DWORD					m_dwFileIndex;				//文件索引
	DWORD					m_dwOutCount;				//输入文件计数
	DWORD					m_dwStartTime;				//开始时间
	TCHAR					m_szFileName[MAX_PATH];		//当前文件名
	TCHAR					m_szExName[8];				//扩展名
	TCHAR					m_szPrevName[12];			//前缀
	TCHAR					m_szLogPath[MAX_PATH];		//
	TCHAR					m_szTmpExName[8];			//监时扩展名

private:
	CThreadLock				m_ThreadLock;				//

public:
	CLogFileManage(void);
	~CLogFileManage(void);

public:
	//输出消息到文件
	void OutputLogToFile(LPCTSTR lpszMsg,emLogLevel ll=LL_NORMAL);
	//输出消息到文件
	void OutputLogMsg(emLogLevel ll,LPCTSTR lpszMsg,...);
	//输入数据到文件
	void OutputDataToFile(const void * pDstData,int nDataSize);
	//关闭文件
	void CloseOutFile();
	//设置扩展名
	void SetExName(LPCTSTR lpszExName);
	//设置前缀
	void SetPrevName(LPCTSTR lpszPrevName);
	//设置每个文件行数
	void SetLinePreFile(int nLine){m_nRecordLine=nLine;}
	//设置日志时间标志
	void SetLogTimeFlag(BOOL bOutTime){m_bLogTime=bOutTime;}
	//设置日志路径
	void SetLogPath(LPCTSTR lpszLogPath){lstrcpy(m_szLogPath,lpszLogPath);}
	//设置监时扩展名
	void SetTmpExName(LPCTSTR lpszTmpExName){lstrcpy(m_szTmpExName,lpszTmpExName);}

private:
	//生成文件名
	void BuildFileName();
	
};
//////////////////////////////////////////////////////////////////////////////////////////////
extern CLogFileManage g_LogManage;
