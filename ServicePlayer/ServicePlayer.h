
// ServicePlayer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CServicePlayerApp:
// �йش����ʵ�֣������ ServicePlayer.cpp
//

class CServicePlayerApp : public CWinApp
{
private:
	ULONG_PTR			m_gdiToken;

public:
	CServicePlayerApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CServicePlayerApp theApp;

///////////////////////////////////////////////////////////////////////////////////
//���ҽ���
DWORD FindProcessByName(LPCTSTR lpszAppName,DWORD dwProcessID);
//�رս���
BOOL TerminateProcessFromId(DWORD dwId);
//��������
void ParseRunParameter(LPCTSTR lpszCmdLine);