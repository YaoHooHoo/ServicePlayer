///////////////////////////////////////////////////////////////////////////
//		�û���Ƶ��ʾ������ͷ�ļ� UserVideoWnd.h
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
	WORD						m_wVideoCX;						//��Ƶ��
	WORD						m_wVideoCY;						//��Ƶ��
	int							m_nOffsetX;						//Xƫ����
	int							m_nOffsetY;						//Yƫ����
	int							m_nEventCX;						//�¼�����
	int							m_nEventCY;						//�¼�����
	int							m_nWndCX;						//���ڿ�
	int							m_nWndCY;						//���ڸ�

public:
	BOOL						m_bReadering;					//��Ⱦ�б�־
	BOOL						m_bDevType;						//�豸����
	BOOL						m_bMouseKeyCtrl;				//����豸����״̬

private:
	ISendMouseEvent				*m_pISendMouseEvent;			//���緢�ͽӿ�
	CSnapPlayManageHelper		m_SnapPlayManageHelper;			//��Ƶ���Ÿ��������
	ISnapPlayManage				*m_pISnapPlayManage;			//��Ƶ���Žӿ�

public:
	CUserVideoWnd();
	virtual ~CUserVideoWnd();

public:
	//��ʼ����Ƶ����
	bool InitUserVideoWnd(ISendMouseEvent *pISendMouseEvent);
	//��ʼ������Ƶ
	void StartPlayVideo();
	//ֹͣ������Ƶ
	void StopPlayVideo();
	//����H264SpsPps����
	void SetH264SpsPpsData(const BYTE *pSpsPps,WORD wDataSize);
	//������Ƶ����
	void PlayVideoData(const BYTE *pVideoData,DWORD dwDataSize);
	//��ȡ�¼�����С
	void GetEventAreaSize(int &nOffsetX,int &nOffsetY,int &nVideoCX,int &nWidthCY,int &nWndCX,int &nWndCY);
	//����������״̬
	void SetMouseCtrlFlag(BOOL bMouseCtrl){m_bMouseKeyCtrl=bMouseCtrl;}
	//�ͻ��˳�֪ͨ
	void CustomExitNotify();

protected:
	//���ڴ�����Ϣ
	virtual int OnSubCreate(LPCREATESTRUCT lpCreateStruct);
	//������Ϣ
	virtual void OnDrawUserInfo(CDC *pMemDC,int cx,int cy);
	//���ڴ�С
	virtual void OnSubSize(UINT nType,int cx,int cy);

private:
	//�����¼����ڴ�С
	void CountEventWndSize();

protected:
	//OpenGL��Ⱦ��Ϣ
	LRESULT OnOpenGLRender(WPARAM wParam,LPARAM lParam);
	//����Ҽ�������Ϣ
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//����Ҽ�������Ϣ
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//����ƶ���Ϣ
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//����Ҽ�������Ϣ
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//����Ҽ�������Ϣ
	void OnRButtonUp(UINT nFlags, CPoint point);
	//������˫����Ϣ
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};


