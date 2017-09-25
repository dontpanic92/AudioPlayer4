// MainDlg2.h : interface of the CMainDlg2 class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "RPlayer\RPlayer.h"
//#include "3rdparty\BtnST\BtnST.h"
#include "TagSystem.h"
#include "PlayList.h"

#include "stdafx.h"
#include "aboutdlg.h"
#include "Dialogs\Dialogs.h"
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include "Controls/RSkinAeroCtrl.h"
#include "Windows/RSpectrumWindow.h"
#include "Visualizations/RAnalyzerSpectrumPainter.h"

class CMainDlg2 : public CDialogImpl<CMainDlg2>, public CUpdateUI<CMainDlg2>,
		public CMessageFilter, public CIdleHandler, public CThemeImpl<CMainDlg2>
{
private:
	CFont m_font;
	RAeroTrackBar m_posTrackBar;
	RAeroTrackBar m_volTrackBar;
	RAeroButton m_btnPlay;
	RAeroButton m_btnPause;
	RAeroButton m_btnStop;
	CTabCtrl	m_tabCtrl;
	RListViewCtrl	m_listCtrl;
	 
	RPlayer& player;
	RPlayList& playlist;

	ProcessFileThread pt;

	wstring time;
	wstring detail;
	wstring name;
	wstring status;
	TagSystem	Tag;
	TAGW	tagw;
	Info	info;
	char	filename[MAX_PATH];
	wchar_t	wfilename[MAX_PATH];
	RSpectrumWindow SpectrumWindow;
	RAnalyzerSpectrumPainter SpectrumPainter;
	bool IsCompositionEnabled() const{
		HRESULT hr;
		BOOL bEnabled;
		hr = DwmIsCompositionEnabled(&bEnabled);
		return SUCCEEDED(hr) && bEnabled;
	}
	
	void DrawGlowText(HDC dc, RECT rcItem,const wchar_t* szText, bool right = false, bool vcenter = true);
	void DrawText(HDC hdc, RECT rcItem,const wchar_t* szText, bool right = false, bool vcenter = true);

	void ShowPlayList(HWND listview, const PlayList& pl);
	void ShowPlayList(CListViewCtrl& listview, const PlayList& pl);

	void RefreshTime();
	void RefreshInfo();
public:
	enum { IDD = IDD_MAINDLG2 };
	
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}
	 
	virtual BOOL OnIdle()
	{
		UIUpdateChildWindows();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainDlg2)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg2)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_DWMCOMPOSITIONCHANGED, OnDwmCompoChanged)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll);
		MESSAGE_HANDLER(WM_CLOSE, OnClose);
		MESSAGE_HANDLER(RM_PLAY, OnListPlay)
		MESSAGE_HANDLER(RM_ADD_SONG, OnAddSong)
		MESSAGE_HANDLER(RM_REMOVE, OnRemove)
		MESSAGE_HANDLER(RM_CHANGE_SONG, OnChangeSong)
		MESSAGE_HANDLER(RM_SETSTATUS, OnSetStatus)
		MSG_WM_PAINT(OnPaint)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDC_BTN_PLAY, OnPlay)
		COMMAND_ID_HANDLER(IDC_BTN_PAUSE, OnPause)
		COMMAND_ID_HANDLER(IDC_BTN_STOP, OnStop)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(ID_SPECTRUM, OnOpenSpectrum)
		CHAIN_MSG_MAP(CThemeImpl<CMainDlg2>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	CMainDlg2():player(*RPlayer::getInstance()), playlist(*RPlayList::getInstance()){SetThemeClassList ( L"globals" );}

	LRESULT OnPaint(HDC hdc){
		CPaintDC dc(*this);
		RECT rect;
		GetClientRect(&rect);
		dc.FillSolidRect(&rect, RGB(0,0,0));
		m_tabCtrl.GetClientRect(&rect);
		m_tabCtrl.ClientToScreen(&rect);
		this->ScreenToClient(&rect);
		dc.FillSolidRect(&rect, RGB(255,255,255));
		return 0;
	}

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);

	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);

	LRESULT OnListPlay(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnAddSong(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnRemove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnChangeSong(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnSetStatus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnDwmCompoChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){
		MARGINS mar = {0};
		mar.cyTopHeight = 140;
		mar.cyBottomHeight = 16;
		if (IsCompositionEnabled()){
			DwmExtendFrameIntoClientArea(*this, &mar);
		}
		return 1;
	}
	LRESULT OnHScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);

	LRESULT OnPlay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnOpenSpectrum(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/){
		return SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, wParam);
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);
		KillTimer(1);
		KillTimer(2);
		playlist.SavePlayLists();
		return 0;
	}

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){
		player.Stop();
		DestroyWindow();
		::PostQuitMessage(0);
		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: Add validation code 
		CloseDialog(wID);
		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CloseDialog(wID);
		return 0;
	}

	void CloseDialog(int nVal)
	{
		DestroyWindow();
		::PostQuitMessage(nVal);
	}
};
