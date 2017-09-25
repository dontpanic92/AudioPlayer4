// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef __RSPECTRUMWINDOW__H__
#define __RSPECTRUMWINDOW__H__
#include "../stdafx.h"
#include "../Visualizations/RAnalyzer.h"
#include "../Visualizations/RAnalyzerSpectrumPainter.h"

class RSpectrumWindow : 
	public CFrameWindowImpl<RSpectrumWindow>, 
	public CUpdateUI<RSpectrumWindow>,
	public CMessageFilter, public CIdleHandler
{
private:
	RAnalyzerSpectrumPainter SpectrumPainter;
	double fps;
	DWORD time;
	DWORD time2;
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_SPECTRUMWINDOW)

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CFrameWindowImpl<RSpectrumWindow>::PreTranslateMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(RSpectrumWindow)
		UPDATE_ELEMENT(ID_SPECTRUMWINDOW_10, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_SPECTRUMWINDOW_20, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_SPECTRUMWINDOW_40, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_SPECTRUMWINDOW_80, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_SPECTRUMWINDOW_160, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_SPECTRUMWINDOW_320, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(RSpectrumWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnDrawBkgnd);
		MESSAGE_HANDLER(WM_TIMER, OnTimer);
		COMMAND_ID_HANDLER(ID_SPECTRUMWINDOW_10, OnSetBar10);
		COMMAND_ID_HANDLER(ID_SPECTRUMWINDOW_20, OnSetBar20);
		COMMAND_ID_HANDLER(ID_SPECTRUMWINDOW_40, OnSetBar40);
		COMMAND_ID_HANDLER(ID_SPECTRUMWINDOW_80, OnSetBar80);
		COMMAND_ID_HANDLER(ID_SPECTRUMWINDOW_160, OnSetBar160);
		COMMAND_ID_HANDLER(ID_SPECTRUMWINDOW_320, OnSetBar320);
		CHAIN_MSG_MAP(CUpdateUI<RSpectrumWindow>)
		CHAIN_MSG_MAP(CFrameWindowImpl<RSpectrumWindow>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

	LRESULT OnDrawBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);

	void SetTitle(const wchar_t* name, const wchar_t* time);

	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);

	LRESULT OnSetBar10(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
		SetBar(10, ID_SPECTRUMWINDOW_10);
		return 1;
	}

	LRESULT OnSetBar20(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
		SetBar(20, ID_SPECTRUMWINDOW_20);
		return 1;
	}

	LRESULT OnSetBar40(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
		SetBar(40, ID_SPECTRUMWINDOW_40);
		return 1;
	}

	LRESULT OnSetBar80(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
		SetBar(80, ID_SPECTRUMWINDOW_80);
		return 1;
	}

	LRESULT OnSetBar160(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
		SetBar(160, ID_SPECTRUMWINDOW_160);
		return 1;
	}

	LRESULT OnSetBar320(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
		SetBar(320, ID_SPECTRUMWINDOW_320);
		return 1;
	}

	void SetBar(int bars, int ID);
};
#endif