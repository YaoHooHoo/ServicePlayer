
// ServicePlayer.cpp : ����Ӧ�ó��������Ϊ��
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

// CServicePlayerApp ����

CServicePlayerApp::CServicePlayerApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CServicePlayerApp ����

CServicePlayerApp theApp;


// CServicePlayerApp ��ʼ��

BOOL CServicePlayerApp::InitInstance()
{
	CWinApp::InitInstance();

		//�ж��Ƿ�����
//#ifndef _DEBUG
	bool bExistClient=false;
	CMutex Mutex(FALSE,TEXT("Oit-Vss-Service-Player"),NULL);
	if (Mutex.Lock(0)==FALSE) bExistClient=true;
	if (bExistClient==true)
	{
		//ֹͣ�ɽ���
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

	//TCHAR szKey[]=TEXT("PASH_00042");			//����Lisence
	//TCHAR szLisence[WNAME_LEN]=TEXT("");
	//BuildDateLisence1(NULL,2018,3,30,2,szLisence,WNAME_LEN);

	LoadParameter();

	Gdiplus::GdiplusStartupInput startInput;
	Gdiplus::GdiplusStartup(&m_gdiToken,&startInput,NULL);

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	ParseRunParameter(m_lpCmdLine);	//��������

	CServicePlayerDlg dlg;
	CLoginDlg	loginDlg;

	dlg.StartTimerService();

	INT_PTR nResult=loginDlg.DoModal();
	if(nResult==IDCANCEL)
	{
		return FALSE;	//�˳�
	}
	
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	dlg.StopTimerSerivce();

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

int CServicePlayerApp::ExitInstance()
{
	Gdiplus::GdiplusShutdown(m_gdiToken);
	return __super::ExitInstance();
}

//���ҽ���
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

//�رս���
BOOL TerminateProcessFromId(DWORD dwId)
{
	BOOL bRet = FALSE;
	// ��Ŀ����̣�ȡ�ý��̾��
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwId);
	if(hProcess != NULL)
	{
		// ��ֹ����
		bRet = ::TerminateProcess(hProcess, 0);
	}
	CloseHandle(hProcess);
	return bRet;
}

//��������
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