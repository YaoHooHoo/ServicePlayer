
// ServicePlayerDlg.h : ͷ�ļ�
//

#pragma once

#include "UserVideoWnd.h"
#include "TimerEngine.h"
#include "LogFileManage.h"
#include "VideoSockSink.h"
#include "NotifyCtrl.h"
#include "TopicNotifyDlg.h"
#include "ConfirmDlg.h"

////////////////////////////////////////////////////////////////////////////////////////////
// CServicePlayerDlg �Ի���
class CServicePlayerDlg : public CDialogEx,public ITimerEngineSink,public ILogOutput,public IVideoManage
{
private:
	bool					m_bCreated;					//������־
	CUserVideoWnd			m_VideoWnd;					//��Ƶ����
	CStatusDlg				m_StatusDlg;				//��־״̬
	CConfirmDlg				m_ConfirmDlg;				//ȷ�ϴ���

private:
	DWORD					m_dwStartTime;				//��ʼʱ��
	DWORD					m_dwLastSize;				//���´�С
	CVideoSockSink			m_VideoSockSink;			//�������

private:
	CTimerEngine			m_TimerEngine;				//��ʱ������
	CLogFileManage			m_LogFileManage;			//��־����
	CNotifyCtrl				m_NotifyCtrl;				//���̶���
	CTopicNotifyDlg			m_TopicNotifyDlg;			//��ʾ�Ի���

private:
	CAudioModuleHelper			m_AudioModuleHelper;			//��Ƶ������������
	IAudioModuleManage			*m_pIAudioModuleManage;			//��Ƶ�������ӿ�

// ����
public:
	CServicePlayerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SERVICEPLAYER_DIALOG };

protected:
	// DDX/DDV ֧��
	virtual void DoDataExchange(CDataExchange* pDX);	
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	//��Ϣ������
	BOOL PreTranslateMessage(MSG * pMsg);
	//�˳���ť�¼�
	virtual void OnCancel();
	//ϵͳ�����
    virtual BOOL OnCommand(WPARAM wParam,LPARAM lParam);

public:
	//��ʼ��ʱ������
	void StartTimerService();
	//ֹͣ��ʱ������
	void StopTimerSerivce();
	//��ʼ����
	void OnStartConnect();
	//ֹͣ����
	void OnStopConnnect();
	//��ʼ��Ƶģ��
	bool StartAudioModule();

public:
	//��ʱ���¼�
	virtual bool __cdecl OnEventTimer(WORD wTimerID,WPARAM wBindParam,void * pTimerSink);
	//�����־��Ϣ
	virtual void __cdecl OutputLogToFile(LPCTSTR lpszMsg);
	//�����־��Ϣ
	virtual void __cdecl OutputFormateLog(LPCTSTR lpszMsg,...);
	//����H264SpsPps����
	virtual bool __cdecl SetH264SpsPpsData(const BYTE * pSpsPps,WORD wDataSize);
	//������Ƶ��
	virtual bool __cdecl PlayVideoStream(const BYTE *pVideoData,DWORD dwDataSize);
	//��ȡ�¼�����С
	virtual void __cdecl GetEventAreaSize(int &nOffsetX,int &nOffsetY,int &nVideoCX,int &nWidthCY,int &nWndCX,int &nWndCY);
	//�û��˳�֪ͨ
	virtual void __cdecl OnCustomExitNotify();

protected:
	//�ػ���Ϣ
	afx_msg void OnPaint();
	//���ڴ�С��Ϣ
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//�ر��¼�
	afx_msg void OnClose();
	//��ѡ���¼�
	afx_msg void OnBnClickedChkMouseKey();
	//������Ϣ
	afx_msg LRESULT OnNotifyIconMessage(WPARAM wParam,LPARAM lParam);
	//������Ƶ������Ϣ
	afx_msg LRESULT OnAcceptNotifyMessage(WPARAM wParam,LPARAM lParam);
	//����ʧȥ����
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//������Ϣ
	afx_msg void OnActivate(UINT nState,CWnd* pWndOther, BOOL bMinined);
	//������Ϣ
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	//��ⴰ�ڻ��Ϣ
	afx_msg LRESULT OnCheckWndActiveMessage(WPARAM wParam,LPARAM lParam);
	//���̼����֪ͨ��Ϣ
	afx_msg LRESULT OnProcessCtrlNotifyMsg(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()


private:
	//���ô��ڴ�С
	void SetWindowSize();
	//���ͼ����¼�
	inline void SendKeyboardEvent(int nKeyValue,int nKeyType);
	
};

/////////////////////////////////////////////////////////////////////////////////////
//��֤�����ڲ��ɼ�
BOOL CheckMainWndVisible(HWND hMainWnd);
