/////////////////////////////////////////////////////////////////////////////////////
//		登录对话框类头文件 LoginDlg.h
////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "TimerEngine.h"
#include "HttpDataManage.h"

////////////////////////////////////////////////////////////////////////////////////
// CLoginDlg 对话框

class CLoginDlg : public CDialogEx,public IHttpApiEventSink
{
	DECLARE_DYNAMIC(CLoginDlg)

private:
	CProgressCtrl			m_ProgressCtrl;				//进度条
	int						m_nProcessPos;				//进度条位置
	DWORD					m_dwHttpStartTime;			//HttpApi调用开始时间

private:
	DWORD					m_dwLoginCount;				//登录次数
	CHttpDataManage			m_HttpDataManage;			//Http管理

public:
	CLoginDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLoginDlg();

// 对话框数据
	enum { IDD = IDD_LOGIN_DLG };

protected:
	// DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange* pDX); 
	//初始化对话框
	virtual BOOL OnInitDialog();

protected:
	//数据传输完成接口
	virtual bool __cdecl HttpEventNotify(int nStatus,WPARAM wParam,const void * pDownData,WORD wDataSize,const int ldt);

	DECLARE_MESSAGE_MAP()
public:
	//进入按钮事件
	afx_msg void OnBnClickedOk();
	//取消按钮事件
	afx_msg void OnBnClickedCancel();
	//下载完成消息事件
	afx_msg LRESULT OnDownConfigFinished(WPARAM wParam,LPARAM lParam);
	//重新登录配置服务器事件
	afx_msg LRESULT OnReloginConfigSvr(WPARAM wParam,LPARAM lParam);
	//定时器消息
	afx_msg void OnTimer(UINT_PTR nIDTime);

private:
	//验证Lisence
	void CheckLisenceValid();
};

///////////////////////////////////////////////////////////////////
//解析服务器IP地址
void ParseLbsIpList(LPTSTR lszTmpIP);
//获取以指定字符分隔的字符串
int ParseSubString(LPTSTR lpszList,const TCHAR *split,CStringVec & slist);
BOOL SetMicrPhoneVolume(DWORD dwVolume);
