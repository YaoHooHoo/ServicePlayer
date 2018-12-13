
// ServicePlayerDlg.h : 头文件
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
// CServicePlayerDlg 对话框
class CServicePlayerDlg : public CDialogEx,public ITimerEngineSink,public ILogOutput,public IVideoManage
{
private:
	bool					m_bCreated;					//创建标志
	CUserVideoWnd			m_VideoWnd;					//视频窗口
	CStatusDlg				m_StatusDlg;				//日志状态
	CConfirmDlg				m_ConfirmDlg;				//确认窗口

private:
	DWORD					m_dwStartTime;				//开始时间
	DWORD					m_dwLastSize;				//最新大小
	CVideoSockSink			m_VideoSockSink;			//网络对象

private:
	CTimerEngine			m_TimerEngine;				//定时器引擎
	CLogFileManage			m_LogFileManage;			//日志管理
	CNotifyCtrl				m_NotifyCtrl;				//托盘对象
	CTopicNotifyDlg			m_TopicNotifyDlg;			//提示对话框

private:
	CAudioModuleHelper			m_AudioModuleHelper;			//音频组件辅助类对象
	IAudioModuleManage			*m_pIAudioModuleManage;			//音频组件管理接口

// 构造
public:
	CServicePlayerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SERVICEPLAYER_DIALOG };

protected:
	// DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange* pDX);	
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	//消息处理函数
	BOOL PreTranslateMessage(MSG * pMsg);
	//退出按钮事件
	virtual void OnCancel();
	//系统命令函数
    virtual BOOL OnCommand(WPARAM wParam,LPARAM lParam);

public:
	//开始定时器服务
	void StartTimerService();
	//停止定时器服务
	void StopTimerSerivce();
	//开始连接
	void OnStartConnect();
	//停止连接
	void OnStopConnnect();
	//开始音频模块
	bool StartAudioModule();

public:
	//定时器事件
	virtual bool __cdecl OnEventTimer(WORD wTimerID,WPARAM wBindParam,void * pTimerSink);
	//输出日志消息
	virtual void __cdecl OutputLogToFile(LPCTSTR lpszMsg);
	//输出日志消息
	virtual void __cdecl OutputFormateLog(LPCTSTR lpszMsg,...);
	//设置H264SpsPps数据
	virtual bool __cdecl SetH264SpsPpsData(const BYTE * pSpsPps,WORD wDataSize);
	//播放视频流
	virtual bool __cdecl PlayVideoStream(const BYTE *pVideoData,DWORD dwDataSize);
	//获取事件区大小
	virtual void __cdecl GetEventAreaSize(int &nOffsetX,int &nOffsetY,int &nVideoCX,int &nWidthCY,int &nWndCX,int &nWndCY);
	//用户退出通知
	virtual void __cdecl OnCustomExitNotify();

protected:
	//重绘消息
	afx_msg void OnPaint();
	//窗口大小消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//关闭事件
	afx_msg void OnClose();
	//复选框事件
	afx_msg void OnBnClickedChkMouseKey();
	//托盘消息
	afx_msg LRESULT OnNotifyIconMessage(WPARAM wParam,LPARAM lParam);
	//接受视频辅助消息
	afx_msg LRESULT OnAcceptNotifyMessage(WPARAM wParam,LPARAM lParam);
	//窗口失去焦点
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//激活消息
	afx_msg void OnActivate(UINT nState,CWnd* pWndOther, BOOL bMinined);
	//激活消息
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	//验测窗口活动消息
	afx_msg LRESULT OnCheckWndActiveMessage(WPARAM wParam,LPARAM lParam);
	//进程间控制通知消息
	afx_msg LRESULT OnProcessCtrlNotifyMsg(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()


private:
	//设置窗口大小
	void SetWindowSize();
	//发送键盘事件
	inline void SendKeyboardEvent(int nKeyValue,int nKeyType);
	
};

/////////////////////////////////////////////////////////////////////////////////////
//验证主窗口不可见
BOOL CheckMainWndVisible(HWND hMainWnd);
