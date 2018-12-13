/////////////////////////////////////////////////////////////////////////////////////
//		��¼�Ի�����ͷ�ļ� LoginDlg.h
////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "TimerEngine.h"
#include "HttpDataManage.h"

////////////////////////////////////////////////////////////////////////////////////
// CLoginDlg �Ի���

class CLoginDlg : public CDialogEx,public IHttpApiEventSink
{
	DECLARE_DYNAMIC(CLoginDlg)

private:
	CProgressCtrl			m_ProgressCtrl;				//������
	int						m_nProcessPos;				//������λ��
	DWORD					m_dwHttpStartTime;			//HttpApi���ÿ�ʼʱ��

private:
	DWORD					m_dwLoginCount;				//��¼����
	CHttpDataManage			m_HttpDataManage;			//Http����

public:
	CLoginDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLoginDlg();

// �Ի�������
	enum { IDD = IDD_LOGIN_DLG };

protected:
	// DDX/DDV ֧��
	virtual void DoDataExchange(CDataExchange* pDX); 
	//��ʼ���Ի���
	virtual BOOL OnInitDialog();

protected:
	//���ݴ�����ɽӿ�
	virtual bool __cdecl HttpEventNotify(int nStatus,WPARAM wParam,const void * pDownData,WORD wDataSize,const int ldt);

	DECLARE_MESSAGE_MAP()
public:
	//���밴ť�¼�
	afx_msg void OnBnClickedOk();
	//ȡ����ť�¼�
	afx_msg void OnBnClickedCancel();
	//���������Ϣ�¼�
	afx_msg LRESULT OnDownConfigFinished(WPARAM wParam,LPARAM lParam);
	//���µ�¼���÷������¼�
	afx_msg LRESULT OnReloginConfigSvr(WPARAM wParam,LPARAM lParam);
	//��ʱ����Ϣ
	afx_msg void OnTimer(UINT_PTR nIDTime);

private:
	//��֤Lisence
	void CheckLisenceValid();
};

///////////////////////////////////////////////////////////////////
//����������IP��ַ
void ParseLbsIpList(LPTSTR lszTmpIP);
//��ȡ��ָ���ַ��ָ����ַ���
int ParseSubString(LPTSTR lpszList,const TCHAR *split,CStringVec & slist);
BOOL SetMicrPhoneVolume(DWORD dwVolume);
