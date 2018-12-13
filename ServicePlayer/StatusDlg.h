////////////////////////////////////////////////////////////////////////////////////////////
//		״̬�Ի�����ͷ�ļ� StatusDlg.h
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "localdef.h"

////////////////////////////////////////////////////////////////////////////////////////////
// CStatusDlg �Ի���
class CStatusDlg : public CDialogEx,public IShowLogToWnd
{
	DECLARE_DYNAMIC(CStatusDlg)

protected:
	BOOL						m_bCreated;						//������־
	TCHAR						m_szLogMsg[MAX_PATH];			//��־�ı���Ϣ
	CThreadLock					m_LogMsgLock;					//��־��ʾ��
	COLORREF					m_LogColor;						//��־����ɫ
	CFont						m_txtFont12;					//12������
	BOOL						m_bFlushing;					//��˸��
	DWORD						m_dwFlushCount;					//��˸����
	BOOL						m_bIsAlpha;						//͸����־
	BYTE						m_cbAlphaNum;					//͸����(0--100)

protected:
	BOOL						bInvalidLisence;				//����ɫLisence
	int							nLisenceDays;					//Lisence����
	ISendMouseEvent				*m_pISendMouseEvent;			//���緢�ͽӿ�
	
protected:
	RECT						m_bltRect;						//���ƿ�
	COLORREF					m_gbColor;						//����ɫ
	BOOL						m_bShowLogTxt;					//��ʾ��־�ı���־
	UNDWORD						m_udwParam;						//��ʱ����
	BOOL						m_bDrawFull;					//����ȫ����
	DWORD						m_dwTimeSpace;					//��ʱ���
	DWORD						m_dwEndTime;					//��˸��������


public:
	CStatusDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStatusDlg();

public:
	//���÷����¼��ӿ�
	bool SetSendEventInterface(ISendMouseEvent *pISendMouseEvent);

public:
	//�����Ϣ������
	virtual void __cdecl OutputLogToWnd(LPCTSTR lpszMsg,LOGLEVEL ll=LL_NORMAL,BOOL bFlashWnd=FALSE);
	//�����Ϣ������
	virtual void __cdecl OutputFormateToWnd(LOGLEVEL ll,BOOL bFlashWnd,LPCTSTR lpszMsg,...);
	//�����Ϣ������
	virtual void __cdecl OutputLogAndValueToWnd(LPCTSTR lpszMsg,UNDWORD udwValue,LOGLEVEL ll,BOOL bFlashWnd);

protected:
	// DDX/DDV ֧��
	virtual void DoDataExchange(CDataExchange* pDX);
	//��ʼ���Ի���
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

protected:
	//���Ʊ���
	virtual void DrawTitleTxt(CDC &memDC,int cx,int cy){}
	//������־�ı�
	virtual void DrawLogText(CDC &memDC,int cx,int cy);
	//ֹͣ��˸
	void StopFlushShowMsg();
	//����ڴ�С
	virtual void OnSubSize(UINT nType, int cx, int cy){}
	//���ര�ڴ���
	virtual void OnSubCreate(){}
	//��˸���
	virtual void OnFlushFinished(){}

protected:
	//�ػ溯��
	afx_msg void OnPaint();
	//���ڴ�С��Ϣ
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//��ʱ����Ϣ
	afx_msg void OnTimer(UINT_PTR nTimerID);
};
