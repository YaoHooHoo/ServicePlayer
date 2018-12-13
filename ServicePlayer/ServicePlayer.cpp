
// ServicePlayer.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "ServicePlayer.h"
#include "ServicePlayerDlg.h"
#include "ToolsFunction.h"
#include "LoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HINSTANCE g_hInstance;
#pragma comment(lib, "gdiplus.lib")

////////////////////////////////////////////////////////////////////////////////////
// CServicePlayerApp

BEGIN_MESSAGE_MAP(CServicePlayerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CServicePlayerApp 构造

CServicePlayerApp::CServicePlayerApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CServicePlayerApp 对象

CServicePlayerApp theApp;


// CServicePlayerApp 初始化

BOOL CServicePlayerApp::InitInstance()
{
	CWinApp::InitInstance();

		//判断是否重入
//#ifndef _DEBUG
	bool bExistClient=false;
	CMutex Mutex(FALSE,TEXT("Oit-Vss-Service-Player"),NULL);
	if (Mutex.Lock(0)==FALSE) bExistClient=true;
	if (bExistClient==true)
	{
		//停止旧进程
		DWORD dwProcessID=::GetCurrentProcessId();
		TCHAR szAppName[WNAME_LEN]=TEXT("");
		mysprintf(szAppName,WNAME_LEN,TEXT("%s.exe"),m_pszAppName);
		DWORD dwFindID=FindProcessByName(szAppName,dwProcessID);
		if(dwFindID!=dwProcessID)
		{
			TerminateProcessFromId(dwFindID);
			Sleep(1);
		}
	}
//#endif

	g_hInstance=m_hInstance;

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	//TCHAR szKey[]=TEXT("PASH_00042");			//生成Lisence
	//TCHAR szLisence[WNAME_LEN]=TEXT("");
	//BuildDateLisence1(NULL,2018,3,30,2,szLisence,WNAME_LEN);

	LoadParameter();

	Gdiplus::GdiplusStartupInput startInput;
	Gdiplus::GdiplusStartup(&m_gdiToken,&startInput,NULL);

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	ParseRunParameter(m_lpCmdLine);	//解析参数

	CServicePlayerDlg dlg;
	CLoginDlg	loginDlg;

	dlg.StartTimerService();

	INT_PTR nResult=loginDlg.DoModal();
	if(nResult==IDCANCEL)
	{
		return FALSE;	//退出
	}
	
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	dlg.StopTimerSerivce();

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

int CServicePlayerApp::ExitInstance()
{
	Gdiplus::GdiplusShutdown(m_gdiToken);
	return __super::ExitInstance();
}

//查找进程
DWORD FindProcessByName(LPCTSTR lpszAppName,DWORD dwProcessID)
{
	HANDLE hProcessSnap = NULL; 
	PROCESSENTRY32 pe32 = {0}; 

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return dwProcessID; 
	}

	TCHAR szProccName[MAX_PATH]=TEXT("");
	TCHAR szSrcName[MAX_PATH]=TEXT("");
	lstrcpy(szSrcName,lpszAppName);
	_strupr(szSrcName);
	pe32.dwSize = sizeof(PROCESSENTRY32); 
	
	if(Process32First(hProcessSnap, &pe32)) 
	{ 
		do 
		{
			lstrcpy(szProccName,pe32.szExeFile);
			_strupr(szProccName);
			if(!strcmp(szProccName,szSrcName))
			{
				if(dwProcessID!=pe32.th32ProcessID)
				{
					dwProcessID=pe32.th32ProcessID;
					break;
				}
			}
		} 
		while (Process32Next(hProcessSnap, &pe32)); 
	}

	CloseHandle (hProcessSnap); 
	return dwProcessID;
}

//关闭进程
BOOL TerminateProcessFromId(DWORD dwId)
{
	BOOL bRet = FALSE;
	// 打开目标进程，取得进程句柄
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwId);
	if(hProcess != NULL)
	{
		// 终止进程
		bRet = ::TerminateProcess(hProcess, 0);
	}
	CloseHandle(hProcess);
	return bRet;
}

//分析参数
void ParseRunParameter(LPCTSTR lpszCmdLine)
{
	int nCmdLen=lstrlen(lpszCmdLine);
	if(nCmdLen<=0 || nCmdLen>=MAX_PATH) return;
	TCHAR szCmdLine[MAX_PATH]=TEXT("");
	lstrcpy(szCmdLine,lpszCmdLine);
	CStringVec slist;
	int nCount=ParseSubString(szCmdLine," ",slist);
	if(nCount>0)
	{
		string strJobNum=slist[0];
		if(strJobNum.length()>=NAME_LEN) return;
		lstrcpy(g_GlobalData.mDevSnData.szJobNum,strJobNum.c_str());
		g_GlobalData.mDevSnData.bAutoLogin=TRUE;
		if(nCount>1)
		{
			string strPasword=slist[1];
			lstrcpy(g_GlobalData.mDevSnData.szPassword,strPasword.c_str());
		}
	}
}