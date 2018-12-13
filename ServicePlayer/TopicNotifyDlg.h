/////////////////////////////////////////////////////////////////////////////////////////
//		ȷ�Ͽͻ�������Ƶ�����Ի�����ͷ�ļ� TopicNotifyDlg.h
//		2017-09-12		Bowen
//////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ConfirmDlg.h"

//////////////////////////////////////////////////////////////////////////////////////////
// CTopicNotifyDlg �Ի���
class CTopicNotifyDlg : public CStatusDlg,public IMouseEvent
{
private:
	CFont						m_titleFont;					//��������
	CSkinButton					m_btnAccept;					//ͬ�ⰴť
	CSkinButton					m_btnReject;					//�ܾ���ť

public:
	CTopicNotifyDlg();   // ��׼���캯��
	virtual ~CTopicNotifyDlg();

protected:
	//���ര�ڴ���
	virtual void OnSubCreate();
	//����ڴ�С
	virtual void OnSubSize(UINT nType, int cx, int cy);
	//������־�ı�
	virtual void DrawLogText(CDC &memDC,int cx,int cy);
	//���Ʊ���
	virtual void DrawTitleTxt(CDC &memDC,int cx,int cy);
	//��˸���
	virtual void OnFlushFinished();
	//�����Ϣ������
	virtual void __cdecl OutputLogAndValueToWnd(LPCTSTR lpszMsg,UNDWORD udwValue,LOGLEVEL ll,BOOL bFlashWnd);

public:
	//��갴ť
	virtual HRESULT __cdecl OnEventLButtonUp(UINT uControlID, UINT nFlags, CPoint &point);
	//��갴ť
	virtual HRESULT __cdecl OnEventRButtonUp(UINT uControlID, UINT nFlags, CPoint &point);

};
