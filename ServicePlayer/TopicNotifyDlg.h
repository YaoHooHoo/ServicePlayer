/////////////////////////////////////////////////////////////////////////////////////////
//		确认客户请求视频辅助对话框类头文件 TopicNotifyDlg.h
//		2017-09-12		Bowen
//////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ConfirmDlg.h"

//////////////////////////////////////////////////////////////////////////////////////////
// CTopicNotifyDlg 对话框
class CTopicNotifyDlg : public CStatusDlg,public IMouseEvent
{
private:
	CFont						m_titleFont;					//标题字体
	CSkinButton					m_btnAccept;					//同意按钮
	CSkinButton					m_btnReject;					//拒绝按钮

public:
	CTopicNotifyDlg();   // 标准构造函数
	virtual ~CTopicNotifyDlg();

protected:
	//子类窗口创建
	virtual void OnSubCreate();
	//子类口大小
	virtual void OnSubSize(UINT nType, int cx, int cy);
	//绘制日志文本
	virtual void DrawLogText(CDC &memDC,int cx,int cy);
	//绘制标题
	virtual void DrawTitleTxt(CDC &memDC,int cx,int cy);
	//闪烁完成
	virtual void OnFlushFinished();
	//输出消息到窗口
	virtual void __cdecl OutputLogAndValueToWnd(LPCTSTR lpszMsg,UNDWORD udwValue,LOGLEVEL ll,BOOL bFlashWnd);

public:
	//鼠标按钮
	virtual HRESULT __cdecl OnEventLButtonUp(UINT uControlID, UINT nFlags, CPoint &point);
	//鼠标按钮
	virtual HRESULT __cdecl OnEventRButtonUp(UINT uControlID, UINT nFlags, CPoint &point);

};
