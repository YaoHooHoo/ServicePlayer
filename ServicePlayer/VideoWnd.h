///////////////////////////////////////////////////////////////////////////
//		视频显示窗口类头文件 VideoWnd.h
//		2016-8-3 Bowen.hu
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "localdef.h"

#define TEST_VIDEO			0				//测试视频编解码播放
#define WM_GL_RENDER_ID		(WM_APP+8001)		//OpenGL渲染消息ID
////////////////////////////////////////////////////////////////////////////
// CVideoWnd
class CVideoWnd : public CWnd
{
	DECLARE_DYNAMIC(CVideoWnd)

protected:
	bool						m_bCreated;						//窗口创建标志
	DWORD						m_dwChannelID;					//通道ID(用户ID)
	int							m_nDevIndex;					//设备索引

protected:
	CFont						m_txtFont14;				//14号字体
	
public:
	CVideoWnd();
	virtual ~CVideoWnd();

public:
	//设置通道ID
	void SetChannelID(DWORD dwChannelID,int nDevIndex);
	//获取通道ID
	DWORD GetChannelID(){return m_dwChannelID;}

protected:
	//窗口创建消息
	virtual int OnSubCreate(LPCREATESTRUCT lpCreateStruct){return 0;}
	//绘制信息
	virtual void OnDrawUserInfo(CDC *pMemDC,int cx,int cy);
	//窗口大小
	virtual void OnSubSize(UINT nType,int cx,int cy){}

protected:
	//绘制消息
	afx_msg void OnPaint();
	//窗口创建消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//窗口大小消息
	afx_msg void OnSize(UINT nType,int cx,int cy);

	DECLARE_MESSAGE_MAP()
};


