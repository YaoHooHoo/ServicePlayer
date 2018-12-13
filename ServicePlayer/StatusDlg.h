////////////////////////////////////////////////////////////////////////////////////////////
//		状态对话框类头文件 StatusDlg.h
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "localdef.h"

////////////////////////////////////////////////////////////////////////////////////////////
// CStatusDlg 对话框
class CStatusDlg : public CDialogEx,public IShowLogToWnd
{
	DECLARE_DYNAMIC(CStatusDlg)

protected:
	BOOL						m_bCreated;						//创建标志
	TCHAR						m_szLogMsg[MAX_PATH];			//日志文本信息
	CThreadLock					m_LogMsgLock;					//日志显示锁
	COLORREF					m_LogColor;						//日志显颜色
	CFont						m_txtFont12;					//12号字体
	BOOL						m_bFlushing;					//闪烁中
	DWORD						m_dwFlushCount;					//闪烁次数
	BOOL						m_bIsAlpha;						//透明标志
	BYTE						m_cbAlphaNum;					//透明度(0--100)

protected:
	BOOL						bInvalidLisence;				//有颜色Lisence
	int							nLisenceDays;					//Lisence天数
	ISendMouseEvent				*m_pISendMouseEvent;			//网络发送接口
	
protected:
	RECT						m_bltRect;						//绘制块
	COLORREF					m_gbColor;						//背景色
	BOOL						m_bShowLogTxt;					//显示日志文本标志
	UNDWORD						m_udwParam;						//临时参数
	BOOL						m_bDrawFull;					//绘制全区域
	DWORD						m_dwTimeSpace;					//定时间隔
	DWORD						m_dwEndTime;					//闪烁结束次数


public:
	CStatusDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStatusDlg();

public:
	//设置发送事件接口
	bool SetSendEventInterface(ISendMouseEvent *pISendMouseEvent);

public:
	//输出消息到窗口
	virtual void __cdecl OutputLogToWnd(LPCTSTR lpszMsg,LOGLEVEL ll=LL_NORMAL,BOOL bFlashWnd=FALSE);
	//输出消息到窗口
	virtual void __cdecl OutputFormateToWnd(LOGLEVEL ll,BOOL bFlashWnd,LPCTSTR lpszMsg,...);
	//输出消息到窗口
	virtual void __cdecl OutputLogAndValueToWnd(LPCTSTR lpszMsg,UNDWORD udwValue,LOGLEVEL ll,BOOL bFlashWnd);

protected:
	// DDX/DDV 支持
	virtual void DoDataExchange(CDataExchange* pDX);
	//初始化对话框
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

protected:
	//绘制标题
	virtual void DrawTitleTxt(CDC &memDC,int cx,int cy){}
	//绘制日志文本
	virtual void DrawLogText(CDC &memDC,int cx,int cy);
	//停止闪烁
	void StopFlushShowMsg();
	//子类口大小
	virtual void OnSubSize(UINT nType, int cx, int cy){}
	//子类窗口创建
	virtual void OnSubCreate(){}
	//闪烁完成
	virtual void OnFlushFinished(){}

protected:
	//重绘函数
	afx_msg void OnPaint();
	//窗口大小消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//定时器消息
	afx_msg void OnTimer(UINT_PTR nTimerID);
};
