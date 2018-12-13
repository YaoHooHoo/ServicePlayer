#include "StdAfx.h"
#include "LogFileManage.h"
#include "localdef.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
CLogFileManage::CLogFileManage(void)
{
	m_dwFileIndex=0;
	m_nRecordLine=5000;
	m_bLogTime=FALSE;
	m_hMyFile=INVALID_HANDLE_VALUE;
	m_dwProccesID=::GetCurrentProcessId();
	lstrcpy(m_szExName,TEXT(".log"));
	lstrcpy(m_szTmpExName,TEXT("tmp"));
	memset(m_szPrevName,0,sizeof(m_szPrevName));
	memset(m_szLogPath,0,sizeof(m_szLogPath));
}

CLogFileManage::~CLogFileManage(void)
{
}

//设置扩展名
void CLogFileManage::SetExName(LPCTSTR lpszExName)
{
	if(lpszExName==NULL || lpszExName[0]==0) return;

	lstrcpy(m_szExName,lpszExName);
}

//设置前缀
void CLogFileManage::SetPrevName(LPCTSTR lpszPrevName)
{
	if(lpszPrevName==NULL) return;
	int nLength=lstrlen(lpszPrevName);
	if(nLength==0 || nLength>=12) return;

	lstrcpy(m_szPrevName,lpszPrevName);
}

//输出消息到窗口
void CLogFileManage::OutputLogToFile(LPCTSTR lpszMsg,emLogLevel ll)
{
	CThreadLockHandle LockHandle(&m_ThreadLock);

	if(m_hMyFile==INVALID_HANDLE_VALUE)
	{
		BuildFileName();
		m_hMyFile=::CreateFile(m_szFileName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(m_hMyFile==INVALID_HANDLE_VALUE)
		{				
			return;
		}
		m_dwStartTime=::GetTickCount();
	}
	DWORD dwWrited=0;
	int nLength=0;
	if(m_bLogTime)
	{
		SYSTEMTIME st={0};
		TCHAR szTime[64]=TEXT("");
		::GetLocalTime(&st);
		mysprintf(szTime,CountArray(szTime),TEXT("[%d-%02d-%02d %02d:%02d:%02d]"),
			st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
		dwWrited=0;
		nLength=lstrlen(szTime);
		::WriteFile(m_hMyFile,szTime,nLength,&dwWrited,NULL);
		#ifdef _DEBUG
		std::cout<<szTime;
		#endif
	}
	dwWrited=0;
	nLength=lstrlen(lpszMsg);
	nLength*=sizeof(TCHAR);
	::WriteFile(m_hMyFile,lpszMsg,nLength,&dwWrited,NULL);
	TCHAR szEndMsg[128]=TEXT("");
	lstrcpy(szEndMsg,TEXT("\r\n"));
	nLength=lstrlen(szEndMsg);
	nLength*=sizeof(TCHAR);
	::WriteFile(m_hMyFile,szEndMsg,nLength,&dwWrited,NULL);
	#ifdef _DEBUG
	std::cout<<lpszMsg<<std::endl;
	#endif
	if(m_dwOutCount++>(DWORD)m_nRecordLine)
	{
		m_dwOutCount=0;
		CloseOutFile();
	}
}

//输出消息到文件
void CLogFileManage::OutputLogMsg(emLogLevel ll,LPCTSTR lpszMsg,...)
{
	const int MAXLOG=2048+512;
	TCHAR szOutMsg[MAXLOG]=TEXT("");
	va_list argList;
	va_start(argList,lpszMsg);
	_vsnprintf_c(szOutMsg,MAXLOG,lpszMsg,argList);
	va_end(argList);

	OutputLogToFile(szOutMsg);
}

//输入数据到文件
void CLogFileManage::OutputDataToFile(const void * pDstData,int nDataSize)
{
	CThreadLockHandle LockHandle(&m_ThreadLock);

	if(m_hMyFile==INVALID_HANDLE_VALUE)
	{
		BuildFileName();
		m_hMyFile=::CreateFile(m_szFileName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(m_hMyFile==INVALID_HANDLE_VALUE)
		{				
			return;
		}
		m_dwStartTime=::GetTickCount();
	}
	DWORD dwWrited=0;

	::WriteFile(m_hMyFile,pDstData,nDataSize,&dwWrited,NULL);

	if(m_nRecordLine>0 && ++m_dwOutCount>=(DWORD)m_nRecordLine)
	{
		m_dwOutCount=0;
		CloseOutFile();
	}
}

//生成文件名
void CLogFileManage::BuildFileName()
{
	SYSTEMTIME st;
	::GetLocalTime(&st);
	DWORD dwSequence=m_dwFileIndex++;
		
	if(m_szPrevName[0]==0)
	{
		mysprintf(m_szFileName,MAX_PATH,TEXT("%s\\%u_%02d%02d%02d%02d%02d_%04d.%s"),m_szLogPath,m_dwProccesID,
			st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,dwSequence,m_szTmpExName);
	}
	else
	{
		mysprintf(m_szFileName,MAX_PATH,TEXT("%s\\%u_%s_%02d%02d%02d%02d%02d_%04d.%s"),m_szLogPath,m_dwProccesID,
			m_szPrevName,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,dwSequence,m_szTmpExName);
	}
}

//关闭文件
void CLogFileManage::CloseOutFile()
{
	if(m_hMyFile==INVALID_HANDLE_VALUE) return;

	TCHAR szDstName[MAX_PATH]=TEXT("");
	::CloseHandle(m_hMyFile);
	m_hMyFile=INVALID_HANDLE_VALUE;
	lstrcpy(szDstName,m_szFileName);
	int nNameLen=lstrlen(m_szFileName);
	if(nNameLen<=4) return;
	szDstName[nNameLen-4]=0;
	lstrcat(szDstName,m_szExName);
	::MoveFile(m_szFileName,szDstName);
}

//////////////////////////////////////////////////////////////////////////////////////
 