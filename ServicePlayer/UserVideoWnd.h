///////////////////////////////////////////////////////////////////////////
//		用户视频显示窗口类头文件 UserVideoWnd.h
//		2016-8-3 Bowen.hu
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "VideoWnd.h"

////////////////////////////////////////////////////////////////////////////
// CMyVideoWnd
class CUserVideoWnd : public CVideoWnd
{
	DECLARE_DYNAMIC(CUserVideoWnd)

private:
	WORD						m_wVideoCX;						//视频宽
	WORD						m_wVideoCY;						//视频高
	int							m_nOffsetX;						//X偏移量
	int							m_nOffsetY;						//Y偏移量
	int							m_nEventCX;						//事件区宽
	int							m_nEventCY;						//事件区高
	int							m_nWndCX;						//窗口宽
	int							m_nWndCY;						//窗口高

public:
	BOOL						m_bReadering;					//渲染中标志
	BOOL						m_bDevType;						//设备类型
	BOOL						m_bMouseKeyCtrl;				//鼠标设备控制状态

private:
	ISendMouseEvent				*m_pISendMouseEvent;			//网络发送接口
	CSnapPlayManageHelper		m_SnapPlayManageHelper;			//视频播放辅助类对象
	ISnapPlayManage				*m_pISnapPlayManage;			//视频播放接口

public:
	CUserVideoWnd();
	virtual ~CUserVideoWnd();

public:
	//初始化视频窗口
	bool InitUserVideoWnd(ISendMouseEvent *pISendMouseEvent);
	//开始播放视频
	void StartPlayVideo();
	//停止播放视频
	void StopPlayVideo();
	//设置H264SpsPps数据
	void SetH264SpsPpsData(const BYTE *pSpsPps,WORD wDataSize);
	//播放视频数据
	void PlayVideoData(const BYTE *pVideoData,DWORD dwDataSize);
	//获取事件区大小
	void GetEventAreaSize(int &nOffsetX,int &nOffsetY,int &nVideoCX,int &nWidthCY,int &nWndCX,int &nWndCY);
	//设置鼠标控制状态
	void SetMouseCtrlFlag(BOOL bMouseCtrl){m_bMouseKeyCtrl=bMouseCtrl;}
	//客户退出通知
	void CustomExitNotify();

protected:
	//窗口创建消息
	virtual int OnSubCreate(LPCREATESTRUCT lpCreateStruct);
	//绘制信息
	virtual void OnDrawUserInfo(CDC *pMemDC,int cx,int cy);
	//窗口大小
	virtual void OnSubSize(UINT nType,int cx,int cy);

private:
	//计算事件窗口大小
	void CountEventWndSize();

protected:
	//OpenGL渲染消息
	LRESULT OnOpenGLRender(WPARAM wParam,LPARAM lParam);
	//鼠标右键按下消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//鼠标右键弹起消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//鼠标移动消息
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//鼠标右键按下消息
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//鼠标右键按下消息
	void OnRButtonUp(UINT nFlags, CPoint point);
	//鼠标左键双击消息
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};


