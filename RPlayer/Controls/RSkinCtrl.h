#pragma once
#include "../stdafx.h"
#include <atlframe.h>
#define	IDC_THUMB	6000
#define	MAX_RANGE	1000
#define WM_USER_DRAGGING WM_USER + 100
#define WM_USER_DRAGGED  WM_USER + 101
#define WM_USER_SETPOS	 WM_USER + 102
#define WM_USER_SETVALID WM_USER + 102

class RTimeStatic : public CWindowImpl<RTimeStatic, CStatic>{

	BEGIN_MSG_MAP(RTimeStatic)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnErase)
	END_MSG_MAP()

	HBITMAP hBMPBackground;
	int width;
	bool alignRight;
	HBITMAP hNumbers;

	LRESULT OnErase(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);

public:
	RTimeStatic():alignRight(true), hBMPBackground(NULL){}

	void SetBk(HBITMAP hbk);

	void SetImg(wchar_t* szPath);

	void DrawText(const char* time);

};

class RBitmapButton : public CBitmapButtonImpl<RBitmapButton>{

	BEGIN_MSG_MAP(RBitmapButton)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMyMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnMyLButtonUp)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		CHAIN_MSG_MAP(CBitmapButtonImpl<RBitmapButton>)
	END_MSG_MAP()

	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnMyMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnMyLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	RBitmapButton(DWORD dwExtendedStyle = BMPBTN_AUTOSIZE, HIMAGELIST hImageList = NULL) : 
	  CBitmapButtonImpl<RBitmapButton>(dwExtendedStyle, hImageList)
	{ }
};

class RSlider : public CWindowImpl<RSlider, CStatic>{

	BEGIN_MSG_MAP(RSlider)
		MESSAGE_HANDLER(WM_PARENTNOTIFY, OnParentNotify)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnErase)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	HBITMAP hBMPWindow;
	RBitmapButton Thumb;
	CImageList	imgLst;
	BITMAP	ThumbBmp;
	HBITMAP	hThumbBmp;
	HBITMAP hProgressBmp;
	int pos;
	bool ondraging;
	bool vertical;
	bool valid;

	void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);

	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void privateSetPos(int pos);

	LRESULT OnParentNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);

	LRESULT OnErase(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);

public:
	RSlider(): hBMPWindow(NULL), hThumbBmp(NULL), hProgressBmp(NULL), pos(0), ondraging(false), vertical(false), valid(true){
	}

	~RSlider(){ReleaseSkin();}

	void SetBk(HBITMAP hBmp);

	void ReleaseSkin();

	void SetSkin(wchar_t* szProgress, wchar_t* szThumb);

	void SetPos(int pos);

	void SetVertical(bool vert);

	void SetValid(bool valid);
};