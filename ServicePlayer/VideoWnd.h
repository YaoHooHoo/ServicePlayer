///////////////////////////////////////////////////////////////////////////
//		��Ƶ��ʾ������ͷ�ļ� VideoWnd.h
//		2016-8-3 Bowen.hu
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "localdef.h"

#define TEST_VIDEO			0				//������Ƶ����벥��
#define WM_GL_RENDER_ID		(WM_APP+8001)		//OpenGL��Ⱦ��ϢID
////////////////////////////////////////////////////////////////////////////
// CVideoWnd
class CVideoWnd : public CWnd
{
	DECLARE_DYNAMIC(CVideoWnd)

protected:
	bool						m_bCreated;						//���ڴ�����־
	DWORD						m_dwChannelID;					//ͨ��ID(�û�ID)
	int							m_nDevIndex;					//�豸����

protected:
	CFont						m_txtFont14;				//14������
	
public:
	CVideoWnd();
	virtual ~CVideoWnd();

public:
	//����ͨ��ID
	void SetChannelID(DWORD dwChannelID,int nDevIndex);
	//��ȡͨ��ID
	DWORD GetChannelID(){return m_dwChannelID;}

protected:
	//���ڴ�����Ϣ
	virtual int OnSubCreate(LPCREATESTRUCT lpCreateStruct){return 0;}
	//������Ϣ
	virtual void OnDrawUserInfo(CDC *pMemDC,int cx,int cy);
	//���ڴ�С
	virtual void OnSubSize(UINT nType,int cx,int cy){}

protected:
	//������Ϣ
	afx_msg void OnPaint();
	//���ڴ�����Ϣ
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//���ڴ�С��Ϣ
	afx_msg void OnSize(UINT nType,int cx,int cy);

	DECLARE_MESSAGE_MAP()
};


