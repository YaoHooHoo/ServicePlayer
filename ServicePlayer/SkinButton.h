////////////////////////////////////////////////////////////////////////////
//		ͼƬ��ť��ͷ�ļ� SkinButton.h
///////////////////////////////////////////////////////////////////////////
#ifndef __DEIRLYM_HUBO_SKIN_BUTTON_HEAD_FILE__
#define __DEIRLYM_HUBO_SKIN_BUTTON_HEAD_FILE__

#include "SkinImage.h"
//////////////////////////////////////////////////////////////////////////
//��������
#define SKIN_BUTTON_CRTEXTCOLOR			TEXT("��ť������ɫ")			// COLORREF ���� ����д��
#define SKIN_BUTTON_STRIMAGEPATH		TEXT("��ť����ͼ")				// CString ���� ����д��

//////////////////////////////////////////////////////////////////////////
//����¼��ص��ӿ�
interface IMouseEvent
{
	//��갴ť
	virtual HRESULT __cdecl OnEventLButtonUp(UINT uControlID, UINT nFlags, CPoint &point)=0;	
	//��갴ť
	virtual HRESULT __cdecl OnEventRButtonUp(UINT uControlID, UINT nFlags, CPoint &point)=0;
};
//////////////////////////////////////////////////////////////////////////
//��ť��
class CSkinButton : public CButton
{	
	//��������
private:
	CSinkBitmap							*m_pImageBack;					//��ťλͼ
	HICON								m_hIcon;						//��ťͼ��
	int									m_nWidth;						//���
	int									m_nHeight;						//�߶�

protected:
	DWORD								m_dwBitmapID;					//λͼID
	bool								m_bExpand;						//�Ƿ�����
	bool								m_bHovering;					//������־
	bool								m_bTransparent;					//�Ƿ�͸��
	bool								m_bButtonDown;					//���±�־	
	
	CSinkBitmap							*m_pDrawImage;					//����λͼ
	const int							m_nStatusNum;					//��ť״̬��(��ť�ɼ�֡ͼƬ���)

public:
	COLORREF							m_crTextColor;					//������ɫ
	int									m_nFontSize;					//�����С
	TCHAR								m_szFontName[32];				//��������

	//�ӿ�ָ��
protected:
	IMouseEvent							*m_pIMouseEvent;				//����¼�

	//��������
public:
	//���캯��
	CSkinButton(int nStateNum=4);
	//��������
	virtual ~CSkinButton();

	//���غ���
protected:
	//�ؼ����໯
	virtual void PreSubclassWindow();
	//����滭����
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//���ܺ���
public:
	//��ȡ���ص��ӿ�
	IMouseEvent * GetMouseEventCallBack() { return m_pIMouseEvent; }
	//�������ص��ӿ�
	void SetMouseEventCallBack(IMouseEvent * pIMouseEvent) { m_pIMouseEvent=pIMouseEvent; };

	//���ú���
public:
	//����λ��
	bool FixButtonSize();
	//����ͼ��
	bool SetButtonIcon(HICON hIcon);
	//������ɫ
	bool SetTextColor(COLORREF crTextColor);
	//��������
	bool SetTextFont(int nFontSize,LPCTSTR lpszFontName=NULL);
	//����λͼ
	bool SetButtonImage(LPCTSTR pszFileName, bool bExpandImage,bool bTransparent);
	//����λͼ
	bool SetButtonImage(UINT uBitmapID, HINSTANCE hInstance, bool bExpandImage,bool bTransparent=false);
	//Ĭ�ϰ�ť
	void SetDefaultButton(bool bResetIcon=true, bool bResetColor=true, bool bResetImage=true);
	//��������
	void RectifyRgn();
	//��������
	void SetButtonFont(int nFontSize,LPCTSTR lpszFontName);
	//��ȡ��
	int GetWidth(){return m_nWidth;}
	//��ȡ��
	int GetHeight(){return m_nHeight;}

	//��ѯ����
public:
	//��ȡͼ��
	HICON GetButtonIcon() { return m_hIcon; }
	//��ȡ��ɫ
	COLORREF GetButtonColor() { return m_crTextColor; }
	//���Ƹ�����ָ������
	void PaintParent();

protected:
	//������
	BOOL CheckMouseInButton();
	

protected:
	//���ư�ť
	virtual void DrawButton(HDC hDC,bool bDisable,bool bButtonDown,bool bFocus=false);
		
	//��Ϣ����
protected:
	//������Ϣ
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//����ƶ�
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//����뿪
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	//������Ϣ
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//���ù��
	afx_msg BOOL OnSetCursor(CWnd * pWnd,UINT nHitTest,UINT message);
	//��������Ϣ
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//��������Ϣ
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//����Ҽ���Ϣ
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//����Ҽ���Ϣ
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CSkinButton)
};


//��˸��ť
class CFlashButton: public CSkinButton
{
private:
	int								m_nLastCount;					//ʣ����˸����
	bool							m_bFlashing;					//��˸��
	bool							m_bBtnFlash;					//��˸��ť��־
	CSinkBitmap						*m_pHighLigthImg;				//����λͼ
	CSinkBitmap						*m_pSrcBgImage;					//ԭ����λͼ

private:
	int								m_nFlashCount;					//�����˸����
	int								m_nFlashRate;					//��˸Ƶ��

public:
	CFlashButton();
	~CFlashButton();

public:
	//������˸����(nFlashRate=��/s)
	void SetFlashParam(int nFlashCount,int nFlashRate);
	//��˸��ť
	void FlashButtonImage();
	
protected:
	//��ʱ����Ϣ
	afx_msg void OnTimer(UINT_PTR nEventID);
	//����ƶ�
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CFlashButton)

};

//״̬��ť
class CStatusButton: public CFlashButton
{
private:
	BOOL					m_bAutoSwitch;					//�Զ��л�ͼƬ
	BOOL					m_bSecondImg;					//�ڶ���ťͼƬ
	CSinkBitmap				*m_pImageBack1;					//�ڶ�״̬��ťλͼ
	CSinkBitmap				*m_pImageBack2;					//�ڶ�״̬��ťλͼ

public:
	CStatusButton();
	~CStatusButton();

public:
	//�����Զ��л�ͼƬ
	void SetAutoSwitch(BOOL bAutoSwitch){m_bAutoSwitch=bAutoSwitch;}
	//��ȡ��ťͼƬ״̬
	BOOL GetIsSecondImage(){return m_bSecondImg;}
	//���ð�ťͼƬ״̬
	void SetSecondImage(BOOL bIsSecond);
	//����λͼ
	bool SetButtonStatusImage(UINT uImgID1,UINT uImgID2,HINSTANCE hInstance,bool bExpandImage,bool bTransparent=false);
	
protected:
	//��������Ϣ
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CStatusButton)

};

//ѡ��״̬��ť
class CSelectButton: public CSkinButton
{
private:
	bool					m_bSelected;					//ѡ��״̬

public:
	//���캯��
	CSelectButton();
	//��������
	~CSelectButton();

public:
	//����״̬
	void SetSelected(bool bSelected);

protected:
	//���ư�ť
	virtual void DrawButton(HDC hDC,bool bDisable,bool bButtonDown,bool bFocus=false);
};


#endif  //__DEIRLYM_HUBO_SKIN_BUTTON_HEAD_FILE__
