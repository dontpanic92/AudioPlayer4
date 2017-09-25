#pragma once

#include "../stdafx.h"
#include <atlframe.h>
#include "../PlayList.h"
#include "../Dialogs/DlgBpmDetect.h"

class RAeroButton : public CWindowImpl<RAeroButton,CButton>, public COwnerDraw<RAeroButton>{

	BEGIN_MSG_MAP(RAeroButton)
		MESSAGE_HANDLER(WM_CTLCOLORBTN, OnCtl)
		MESSAGE_HANDLER(OCM_CTLCOLORBTN, OnCtl)
		//MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		//MESSAGE_HANDLER(OCM_DRAWITEM, OnDrawItem)
		CHAIN_MSG_MAP_ALT(COwnerDraw<RAeroButton>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	LRESULT OnCtl(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);

	void DrawItem(LPDRAWITEMSTRUCT lpStruct);

};
class RTabCtrl: public CWindowImpl<RTabCtrl,CTabCtrl>{

	BEGIN_MSG_MAP(RAeroButton)
		//MESSAGE_HANDLER(WM_ERASEBKGND, OnErase)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	LRESULT OnErase(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/){
		CPaintDC dc(*this);
		RECT rect;
		GetClientRect(&rect);
		dc.FillSolidRect(&rect, RGB(0,0,0));
		return 1;
	}
	LRESULT DrawItem(LPDRAWITEMSTRUCT lpStruct){
		MessageBox(L"");
		return 0;
	}
};
class RListViewCtrl: public CWindowImpl<RListViewCtrl, CListViewCtrl>, public COwnerDraw<RListViewCtrl>{

	BEGIN_MSG_MAP(RListViewCtrl)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		COMMAND_ID_HANDLER(ID_PLAY, OnPlay)
		COMMAND_ID_HANDLER(ID_REMOVE, OnRemove)
		COMMAND_ID_HANDLER(ID_ADD, OnAdd)
		COMMAND_ID_HANDLER(ID_BPMDETECT, OnBpmDetect)
		CHAIN_MSG_MAP_ALT(COwnerDraw<RListViewCtrl>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()
	int list;
	RBpmDetectDlg dlg;

public:
	RListViewCtrl():list(0){}

	void SetPlayList(PlayList& pl, int listindex);

	//DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
	LRESULT DeleteItem(LPDELETEITEMSTRUCT lpStruct);

	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);

	LRESULT OnPlay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnRemove(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnBpmDetect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT DrawItem(LPDRAWITEMSTRUCT lpStruct);
};

#define POSRANGE 1000
class RAeroTrackBar : public CWindowImpl<RAeroTrackBar,CTrackBarCtrl>, public CCustomDraw<RAeroTrackBar>{

	BEGIN_MSG_MAP(RAeroTrackBar)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtl)
		MESSAGE_HANDLER(OCM_CTLCOLORSTATIC, OnCtl)
		CHAIN_MSG_MAP(CCustomDraw<RAeroTrackBar>)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	LRESULT OnCtl(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);

public:
	BOOL SubclassWindow(_In_ HWND hWnd, bool pos) throw();

	BOOL SetPos(int Pos);

	static LRESULT CALLBACK PosProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK PosProc2(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};