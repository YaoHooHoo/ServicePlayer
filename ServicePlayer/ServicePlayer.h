
// ServicePlayer.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CServicePlayerApp:
// 有关此类的实现，请参阅 ServicePlayer.cpp
//

class CServicePlayerApp : public CWinApp
{
private:
	ULONG_PTR			m_gdiToken;

public:
	CServicePlayerApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
// 实现

	DECLARE_MESSAGE_MAP()
};

extern CServicePlayerApp theApp;

///////////////////////////////////////////////////////////////////////////////////
//查找进程
DWORD FindProcessByName(LPCTSTR lpszAppName,DWORD dwProcessID);
//关闭进程
BOOL TerminateProcessFromId(DWORD dwId);
//分析参数
void ParseRunParameter(LPCTSTR lpszCmdLine);