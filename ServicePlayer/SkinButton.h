////////////////////////////////////////////////////////////////////////////
//		图片按钮类头文件 SkinButton.h
///////////////////////////////////////////////////////////////////////////
#ifndef __DEIRLYM_HUBO_SKIN_BUTTON_HEAD_FILE__
#define __DEIRLYM_HUBO_SKIN_BUTTON_HEAD_FILE__

#include "SkinImage.h"
//////////////////////////////////////////////////////////////////////////
//属性名字
#define SKIN_BUTTON_CRTEXTCOLOR			TEXT("按钮字体颜色")			// COLORREF 类型 （读写）
#define SKIN_BUTTON_STRIMAGEPATH		TEXT("按钮背景图")				// CString 类型 （读写）

//////////////////////////////////////////////////////////////////////////
//鼠标事件回调接口
interface IMouseEvent
{
	//鼠标按钮
	virtual HRESULT __cdecl OnEventLButtonUp(UINT uControlID, UINT nFlags, CPoint &point)=0;	
	//鼠标按钮
	virtual HRESULT __cdecl OnEventRButtonUp(UINT uControlID, UINT nFlags, CPoint &point)=0;
};
//////////////////////////////////////////////////////////////////////////
//按钮类
class CSkinButton : public CButton
{	
	//变量定义
private:
	CSinkBitmap							*m_pImageBack;					//按钮位图
	HICON								m_hIcon;						//按钮图标
	int									m_nWidth;						//宽度
	int									m_nHeight;						//高度

protected:
	DWORD								m_dwBitmapID;					//位图ID
	bool								m_bExpand;						//是否拉伸
	bool								m_bHovering;					//盘旋标志
	bool								m_bTransparent;					//是否透明
	bool								m_bButtonDown;					//按下标志	
	
	CSinkBitmap							*m_pDrawImage;					//绘制位图
	const int							m_nStatusNum;					//按钮状态数(按钮由几帧图片组成)

public:
	COLORREF							m_crTextColor;					//字体颜色
	int									m_nFontSize;					//字体大小
	TCHAR								m_szFontName[32];				//字体名称

	//接口指针
protected:
	IMouseEvent							*m_pIMouseEvent;				//鼠标事件

	//函数定义
public:
	//构造函数
	CSkinButton(int nStateNum=4);
	//析构函数
	virtual ~CSkinButton();

	//重载函数
protected:
	//控件子类化
	virtual void PreSubclassWindow();
	//界面绘画函数
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//功能函数
public:
	//获取鼠标回调接口
	IMouseEvent * GetMouseEventCallBack() { return m_pIMouseEvent; }
	//设置鼠标回调接口
	void SetMouseEventCallBack(IMouseEvent * pIMouseEvent) { m_pIMouseEvent=pIMouseEvent; };

	//设置函数
public:
	//调整位置
	bool FixButtonSize();
	//设置图标
	bool SetButtonIcon(HICON hIcon);
	//设置颜色
	bool SetTextColor(COLORREF crTextColor);
	//设置字体
	bool SetTextFont(int nFontSize,LPCTSTR lpszFontName=NULL);
	//加载位图
	bool SetButtonImage(LPCTSTR pszFileName, bool bExpandImage,bool bTransparent);
	//加载位图
	bool SetButtonImage(UINT uBitmapID, HINSTANCE hInstance, bool bExpandImage,bool bTransparent=false);
	//默认按钮
	void SetDefaultButton(bool bResetIcon=true, bool bResetColor=true, bool bResetImage=true);
	//调整区域
	void RectifyRgn();
	//设置字体
	void SetButtonFont(int nFontSize,LPCTSTR lpszFontName);
	//获取宽
	int GetWidth(){return m_nWidth;}
	//获取高
	int GetHeight(){return m_nHeight;}

	//查询函数
public:
	//获取图标
	HICON GetButtonIcon() { return m_hIcon; }
	//获取颜色
	COLORREF GetButtonColor() { return m_crTextColor; }
	//绘制父窗口指定区域
	void PaintParent();

protected:
	//鼠标测试
	BOOL CheckMouseInButton();
	

protected:
	//绘制按钮
	virtual void DrawButton(HDC hDC,bool bDisable,bool bButtonDown,bool bFocus=false);
		
	//消息函数
protected:
	//建立消息
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标移动
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//鼠标离开
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	//背景消息
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//设置光标
	afx_msg BOOL OnSetCursor(CWnd * pWnd,UINT nHitTest,UINT message);
	//鼠标左键消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//鼠标左键消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//鼠标右键消息
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//鼠标右键消息
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CSkinButton)
};


//闪烁按钮
class CFlashButton: public CSkinButton
{
private:
	int								m_nLastCount;					//剩除闪烁次数
	bool							m_bFlashing;					//闪烁中
	bool							m_bBtnFlash;					//闪烁按钮标志
	CSinkBitmap						*m_pHighLigthImg;				//高亮位图
	CSinkBitmap						*m_pSrcBgImage;					//原背景位图

private:
	int								m_nFlashCount;					//最大闪烁次数
	int								m_nFlashRate;					//闪烁频率

public:
	CFlashButton();
	~CFlashButton();

public:
	//设置闪烁参数(nFlashRate=次/s)
	void SetFlashParam(int nFlashCount,int nFlashRate);
	//闪烁按钮
	void FlashButtonImage();
	
protected:
	//定时器消息
	afx_msg void OnTimer(UINT_PTR nEventID);
	//鼠标移动
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CFlashButton)

};

//状态按钮
class CStatusButton: public CFlashButton
{
private:
	BOOL					m_bAutoSwitch;					//自动切换图片
	BOOL					m_bSecondImg;					//第二按钮图片
	CSinkBitmap				*m_pImageBack1;					//第二状态按钮位图
	CSinkBitmap				*m_pImageBack2;					//第二状态按钮位图

public:
	CStatusButton();
	~CStatusButton();

public:
	//设置自动切换图片
	void SetAutoSwitch(BOOL bAutoSwitch){m_bAutoSwitch=bAutoSwitch;}
	//获取按钮图片状态
	BOOL GetIsSecondImage(){return m_bSecondImg;}
	//设置按钮图片状态
	void SetSecondImage(BOOL bIsSecond);
	//加载位图
	bool SetButtonStatusImage(UINT uImgID1,UINT uImgID2,HINSTANCE hInstance,bool bExpandImage,bool bTransparent=false);
	
protected:
	//鼠标左键消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CStatusButton)

};

//选择状态按钮
class CSelectButton: public CSkinButton
{
private:
	bool					m_bSelected;					//选择状态

public:
	//构造函数
	CSelectButton();
	//析构函数
	~CSelectButton();

public:
	//设置状态
	void SetSelected(bool bSelected);

protected:
	//绘制按钮
	virtual void DrawButton(HDC hDC,bool bDisable,bool bButtonDown,bool bFocus=false);
};


#endif  //__DEIRLYM_HUBO_SKIN_BUTTON_HEAD_FILE__
