/////////////////////////////////////////////////////////////////////////////////////////
//		ȷ�Ͽͻ�������Ƶ�����Ի�����ͷ�ļ� ConfirmDlg.h
//		2017-09-12		Bowen
//////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "StatusDlg.h"
#include "SkinButton.h"

//////////////////////////////////////////////////////////////////////////////////////////
class CConfirmDlg: public CStatusDlg,public IMouseEvent
{
protected:
	CSkinButton					m_btnAccept;					//ͬ�ⰴť
	CSkinButton					m_btnReject;					//�ܾ���ť
	CSkinButton					m_btnStop;						//ֹͣ��ť

public:
	CConfirmDlg(void);
	~CConfirmDlg(void);

protected:
	//����ڴ�С
	virtual void OnSubSize(UINT nType, int cx, int cy);
	//���ര�ڴ���
	virtual void OnSubCreate();
	//������־�ı�
	virtual void DrawLogText(CDC &memDC,int cx,int cy);
	//��˸���
	virtual void OnFlushFinished();

public:
	//��갴ť
	virtual HRESULT __cdecl OnEventLButtonUp(UINT uControlID, UINT nFlags, CPoint &point);
	//��갴ť
	virtual HRESULT __cdecl OnEventRButtonUp(UINT uControlID, UINT nFlags, CPoint &point);

};

