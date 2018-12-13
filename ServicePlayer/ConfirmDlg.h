/////////////////////////////////////////////////////////////////////////////////////////
//		确认客户请求视频辅助对话框类头文件 ConfirmDlg.h
//		2017-09-12		Bowen
//////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "StatusDlg.h"
#include "SkinButton.h"

//////////////////////////////////////////////////////////////////////////////////////////
class CConfirmDlg: public CStatusDlg,public IMouseEvent
{
protected:
	CSkinButton					m_btnAccept;					//同意按钮
	CSkinButton					m_btnReject;					//拒绝按钮
	CSkinButton					m_btnStop;						//停止按钮

public:
	CConfirmDlg(void);
	~CConfirmDlg(void);

protected:
	//子类口大小
	virtual void OnSubSize(UINT nType, int cx, int cy);
	//子类窗口创建
	virtual void OnSubCreate();
	//绘制日志文本
	virtual void DrawLogText(CDC &memDC,int cx,int cy);
	//闪烁完成
	virtual void OnFlushFinished();

public:
	//鼠标按钮
	virtual HRESULT __cdecl OnEventLButtonUp(UINT uControlID, UINT nFlags, CPoint &point);
	//鼠标按钮
	virtual HRESULT __cdecl OnEventRButtonUp(UINT uControlID, UINT nFlags, CPoint &point);

};

