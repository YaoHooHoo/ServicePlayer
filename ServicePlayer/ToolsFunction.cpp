////////////////////////////////////////////////////////////////////////////////////
//		工具函数实现文件 ToolsFunction.cpp
/////////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "ToolsFunction.h"

/////////////////////////////////////////////////////////////
#define SERAIL_FILE		TEXT("sccore.dat")
////////////////////////////////////////////////////////////////////////////////////////
DWORD GetHexValue(LPCTSTR lpszTime)
{
	TCHAR szTime[64];
	lstrcpy(szTime,lpszTime);
	ToLowerCase(szTime);
	int nLength=lstrlen(szTime);
	if(nLength==0) return 0;
	if(nLength%2!=0) return 0;
	DWORD dwValue=0;
	TCHAR *pSrc=szTime;
	while(nLength>0)
	{
		int nHigh=*pSrc++;
		int nLow=*pSrc++;
		nLength-=2;
		if (nHigh >= 'a' && nHigh <= 'z')
		{
			nHigh -= 'a';
			nHigh += 10;
		}
		else
		{
			nHigh -= '0';
		}

		if (nLow >= 'a' && nLow <= 'z')
		{
			nLow -= 'a';
			nLow += 10;
		}
		else
		{
			nLow -= '0';
		}
		dwValue=(dwValue<<8)+(nHigh * 16 + nLow);
	}

    return dwValue;
}

//获取指定时间值的时间
void GetValueTime(DWORD dwTime,PSYSTEMTIME pst)
{
    ASSERT(pst!=NULL);
    if(pst==NULL) return;
    struct tm * pTp;
    time_t tTime=(time_t)dwTime;
	pTp=::localtime(&tTime);     
    if(pTp==NULL) return;
    pst->wYear=pTp->tm_year+1900;
    pst->wMonth=pTp->tm_mon+1;
    pst->wDay=pTp->tm_mday;
    pst->wHour=pTp->tm_hour;
    pst->wMinute=pTp->tm_min;
    pst->wSecond=pTp->tm_sec;     
}

//把时间字符串转DWORD
DWORD GetTimeFromStr(LPCTSTR lpszDateTime)
{
    if(lpszDateTime==NULL) return 0;
  
    TCHAR szTime[24]="";
    TCHAR *pDest=szTime;
    INT nIndex=0;
    while(*lpszDateTime && nIndex<24)
    {
        if(*lpszDateTime>='0' && *lpszDateTime<='9')
            *pDest++=*lpszDateTime;
        lpszDateTime++;
        nIndex++;
    }
    *pDest=0;
	int nLength=(int)(pDest-szTime);
	pDest=szTime;
	struct tm tDate;
	WORD wYear=2015;
	TCHAR szValue[8];
	memcpy(szValue,pDest,4);
	szValue[4]=0;
	wYear=atoi(szValue);
	pDest+=4;
	nLength-=4;
	if(nLength<=0) return 0;
	const int DATENUM=5;
	WORD waDate[DATENUM];
	memset(waDate,0,sizeof(waDate));
	int index=0;
	while(nLength>0 && index<DATENUM)
	{
		memcpy(szValue,pDest,2);
		szValue[2]=0;
		waDate[index++]=atoi(szValue);
		pDest+=2;
		nLength-=2;
	}
	index=0;
	tDate.tm_year=wYear-1900;
	tDate.tm_mon=waDate[index++]-1;
	tDate.tm_mday=waDate[index++];
	tDate.tm_hour=waDate[index++];
	tDate.tm_min=waDate[index++];
	tDate.tm_sec=waDate[index++];	    
    time_t timeT=mktime(&tDate);
    return (DWORD)timeT;
}

//转换为时间
void OnConvertToTime(DWORD dwTime,LPTSTR lpszTime,int nMaxSize)
{	 
	if(dwTime==0) return;
	SYSTEMTIME st;
	GetValueTime(dwTime,&st);
	_snprintf_c(lpszTime,nMaxSize,TEXT("%d-%02d-%02d %02d:%02d:%02d"),
		st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);	 
}


//获取配置文件名
bool GetConfigFile(LPCTSTR lszFile,LPTSTR lpszFullPath)
{
	::GetModuleFileName(NULL,lpszFullPath,MAX_PATH);
	TCHAR * pLast=strrchr(lpszFullPath,TEXT('\\'));
	if(pLast==NULL) return false;
	pLast++;
	*pLast=0;
	lstrcpy(g_GlobalData.szWorkPath,lpszFullPath);
	lstrcat(lpszFullPath,lszFile);
	return true;
}

//生成路径
void BuildFullPath(LPTSTR lpszPath,int MAX_SIZE)
{
	if(lpszPath[1]!=TEXT(':'))
	{		
		TCHAR szLog[MAX_PATH]=TEXT("");
		lstrcpy(szLog,lpszPath);
		mysprintf(lpszPath,MAX_SIZE,TEXT("%s%s"),g_GlobalData.szWorkPath,szLog);
	}
}

//加载参数
bool LoadParameter()
{	 
	TCHAR szFileName[MAX_PATH]=TEXT("");
	if(!GetConfigFile(TEXT("VideoPlayer.ini"),szFileName))
		return false;

	CReadIniFile initFile;
	if(!initFile.SetFileName(szFileName))
		return false;

	lstrcpy(szFileName,TEXT("LOCAL"));
	g_GlobalData.dwUserID=initFile.GetIntValue(szFileName,TEXT("UserID"));	 
	g_GlobalData.wVideoCX=initFile.GetIntValue(szFileName,TEXT("VideoWidth"));
	g_GlobalData.wVideoCY=initFile.GetIntValue(szFileName,TEXT("VideoHeight"));
	g_GlobalData.bIsHaveMouseCtrl=initFile.GetIntValue(szFileName,TEXT("IsHaveMouseCtrl"));
	g_GlobalData.nBusinessType=initFile.GetIntValue(szFileName,TEXT("BusinessType"));		//业务线类型
	g_GlobalData.bOpenglDraw=initFile.GetIntValue(szFileName,TEXT("NotOpenGLDraw"));	//不使用OpenGL绘制
	g_GlobalData.bOpenglDraw=!g_GlobalData.bOpenglDraw;
	
	initFile.GetStringValue(szFileName,TEXT("ApiUrl"),"",g_GlobalData.szApiUrl,CountArray(g_GlobalData.szApiUrl)-1);
	
	initFile.GetStringValue(szFileName,TEXT("logPath"),"",g_GlobalData.szLogPath,CountArray(g_GlobalData.szLogPath));
	initFile.GetStringValue(szFileName,TEXT("SaveDataPath"),"",g_GlobalData.szSavePath,CountArray(g_GlobalData.szSavePath));
			
	initFile.Close();

	BuildFullPath(g_GlobalData.szLogPath,MAX_PATH);
	BuildFullPath(g_GlobalData.szSavePath,MAX_PATH);

	SerailParameter(true);
	
	CreateSubDir(g_GlobalData.szLogPath);
	CreateSubDir(g_GlobalData.szSavePath);

	g_GlobalData.dwCurrTheadID=::GetCurrentThreadId();

	DWORD dwUNameLen=NAME_LEN;
	::gethostname(g_GlobalData.szHostName,NAME_LEN);
	::GetUserName(g_GlobalData.szUserName,&dwUNameLen);

	return true;
}

//解释服务器地址
DWORD GetLocalHostAddr()
{
	WSADATA wsaData;
	WORD wSockVer=MAKEWORD(2,2);
	WSAStartup(wSockVer,&wsaData);
	TCHAR szHostName[MAX_PATH]=TEXT("");
	int nRet=::gethostname(szHostName,MAX_PATH);
	//转化地址
	LPHOSTENT lpHost=gethostbyname(szHostName);
	::WSACleanup();
	if (lpHost==NULL) return 0;
	DWORD dwHostIP=((LPIN_ADDR)lpHost->h_addr)->s_addr;
	BYTE cbIP[4];
	memcpy(cbIP,&dwHostIP,4);
	printf("Local host ip:[%d.%d.%d.%d]\n",cbIP[0],cbIP[1],cbIP[2],cbIP[3]);
	return dwHostIP;
}

//保存参数
void SerailParameter(bool bRead)
{
	TCHAR szFileName[MAX_PATH]=TEXT("");
	mysprintf(szFileName,MAX_PATH,TEXT("%s%s"),g_GlobalData.szWorkPath,SERAIL_FILE);
	
	CMyFile file;
	if(!bRead)
	{
		if(!file.Open(szFileName,CMyFile::CREATE|CMyFile::WRITE))
			return;

		file.Write(&g_GlobalData.mDevSnData,sizeof(g_GlobalData.mDevSnData));
	}
	else
	{
		if(!file.Open(szFileName,CMyFile::READ))
			return;

		file.Read(&g_GlobalData.mDevSnData,sizeof(g_GlobalData.mDevSnData));
	}
	file.Close();
}