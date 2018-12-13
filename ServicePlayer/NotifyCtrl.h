/////////////////////////////////////////////////////////////////////////////////////////
//		���̿�����ͷ�ļ� NotifyCtrl.h
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "localdef.h"

/////////////////////////////////////////////////////////////////////////////////////////
class CNotifyCtrl
{

public:
	CNotifyCtrl(void);
	~CNotifyCtrl(void);

public:
	//��������״̬
	void SetNotifyIcon(HINSTANCE hInstance,HWND hWnd);
	//ɾ������
	void DeleteNotifyIcon(HWND hWnd);
	//������Ϣ
	LRESULT OnNotifyIconMessage(HWND hWnd,WPARAM wParam,LPARAM lParam);
	//�˵��������
	BOOL OnMenuCommand(HWND hWnd,WPARAM wParam,LPARAM lParam);
};

