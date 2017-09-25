// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#define RM_CHANGE_UI	WM_USER + 10001

#include "Skin.h"
#include "stdafx.h"
#include "AboutDlg.h"
#include "RPlayer\RPlayer.h"
#pragma once

typedef void (*pfnD)(HWND, LPVOID);

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>, 
		public CMessageFilter, public CIdleHandler, public RSkin<CMainDlg>
{
private:
	RPlayer& player;
public:
	enum { IDD = IDD_MAINDLG };
	CMainDlg():player(*RPlayer::getInstance()){}
	~CMainDlg(){DestroyWindow();}
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		UIUpdateChildWindows();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_CLOSE, OnMsgClose)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
		MESSAGE_HANDLER(WM_USER_DRAGGING, OnDragging)
		MESSAGE_HANDLER(WM_USER_DRAGGED, OnDragged)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_MOVE, OnMove)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(findBtnId("btnPlay"), OnPlay)
		COMMAND_ID_HANDLER(findBtnId("btnPause"), OnPause)
		COMMAND_ID_HANDLER(findBtnId("btnStop"), OnStop)
		COMMAND_ID_HANDLER(findBtnId("btnClose"), OnClose)
		COMMAND_ID_HANDLER(findBtnId("btnMinimize"), OnMinimize)
		COMMAND_ID_HANDLER(findBtnId("btnMinimode"), OnMinimode)
		COMMAND_ID_HANDLER(findBtnId("btnPrev"), OnPrev)
		CHAIN_MSG_MAP(RSkin<CMainDlg>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	BOOL ShowWindow(int nCmdShow){
		if (nCmdShow == SW_SHOW || nCmdShow == SW_RESTORE || nCmdShow == SW_SHOWNORMAL || nCmdShow == SW_NORMAL){
			SetLayeredWindowAttributes(m_hWnd, 0, 0, LWA_ALPHA);
			CDialogImpl<CMainDlg>::ShowWindow(nCmdShow);
			for (int i = 0; i * i <= 255; i+=2){
				SetLayeredWindowAttributes(m_hWnd, 0, i * i, LWA_ALPHA);
				Sleep(12);
			}
			SetLayeredWindowAttributes(m_hWnd, 0, 255, LWA_ALPHA);
			return true;
		}else if (nCmdShow == SW_HIDE || nCmdShow ==SW_MINIMIZE){
			for (int i = 0; i * i  <= 255; i+=2){
				SetLayeredWindowAttributes(m_hWnd, 0, 255 - i * i, LWA_ALPHA);
				Sleep(12);
			}
			CDialogImpl<CMainDlg>::ShowWindow(nCmdShow);
			return true;
		}
		CDialogImpl<CMainDlg>::ShowWindow(nCmdShow);
		return true;
	}
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		SetLayeredWindowAttributes(m_hWnd, 0, 255, LWA_ALPHA);
		CenterWindow();
		ModifyStyle(0, WS_MINIMIZEBOX);
		
		// set icons
		HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		SetIcon(hIconSmall, FALSE);
		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		UIAddChildWindowContainer(m_hWnd);
		LoadSkin(NULL);
		SetTimer(1, 50, NULL);
		//player.SetNotify(PlayerCallBack, this);
		player.SetNotify(*this);
		RSkin<CMainDlg>::SetPosSliderValid(false);
		return TRUE;
	}
	LRESULT OnMsgClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){
		::PostQuitMessage(0);
		return 1;
	}
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);
		KillTimer(1);
		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){
		double now = player.GetPosition();
		double length = player.GetDuration();
		RSkin<CMainDlg>::DrawTime(int(now));
		RSkin<CMainDlg>::SetPos((int(length) == 0) ? 0 : int(now / player.GetDuration() * 1000 + 1));
		return 1;
	}
	LRESULT OnMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){
		return 1;
	}
	LRESULT OnPlay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
		if (player.GetStatus() == PAUSE){
			player.Resume();
			RSkin<CMainDlg>::btnShowHide(findBtnId("btnPlay"), false);
			RSkin<CMainDlg>::btnShowHide(findBtnId("btnPause"), true);
			return 1;
		}
		char buffer[10000];
		player.GetDecoderManager()->GetSupportFileType(buffer);
		char Filepath[MAX_PATH];
		OPENFILENAMEA ofn;
		ZeroMemory(&ofn,sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = Filepath;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(Filepath);
		ofn.lpstrFilter = buffer;///"Supported Music File\0*.mp3;*.wav;*.ogg;*.ape\0All File\0*.*\0\0"; 
		ofn.nFilterIndex = 1; 
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_EXPLORER |OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		TCHAR AppPath[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, AppPath);
		GetOpenFileNameA(&ofn);
		SetCurrentDirectory(AppPath);
		if (Filepath[0] == '\0')
			return 1;
		player.Play(Filepath);
		RSkin<CMainDlg>::btnShowHide(findBtnId("btnPlay"), false);
		RSkin<CMainDlg>::btnShowHide(findBtnId("btnPause"), true);
		RSkin<CMainDlg>::SetPosSliderValid(true);
		return 1;
	}
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/){
		return SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, wParam);
	}
	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/){
		return SendMessage(WM_NCRBUTTONDOWN, HTCAPTION, wParam);
	}
	LRESULT OnPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
		player.Pause();
		RSkin<CMainDlg>::btnShowHide(findBtnId("btnPlay"), true);
		RSkin<CMainDlg>::btnShowHide(findBtnId("btnPause"), false);
		return 1;
	}
	LRESULT OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
		player.Stop();
		RSkin<CMainDlg>::btnShowHide(findBtnId("btnPlay"), true);
		RSkin<CMainDlg>::btnShowHide(findBtnId("btnPause"), false);
		RSkin<CMainDlg>::SetPosSliderValid(false);
		return 1;
	}
	LRESULT OnClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
		player.Stop();
		ShowWindow(SW_HIDE);
		SendMessage(WM_CLOSE);
		return 1;
	}
	LRESULT OnMinimize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
		ShowWindow(SW_MINIMIZE);
		return 1;
	}
	LRESULT OnMinimode(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
		PostMessage(RM_CHANGE_UI, 2);
		return 1;
	}
	LRESULT OnSysCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled){
		if (wParam == SC_RESTORE){
			ShowWindow(SW_RESTORE);
		}else if(wParam == SC_MINIMIZE){
			ShowWindow(SW_MINIMIZE);
			bHandled = false;
		}else
			bHandled = false;
		return 1;
	}
	LRESULT OnDragging(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled){
		if ((HWND)wParam == GetDlgItem(IDC_STATIC_VOL)){
			player.SetVolume(int((double)lParam / 10.0));
		}
		return 1;
	}
	LRESULT OnDragged(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled){
		if ((HWND)wParam == GetDlgItem(IDC_STATIC_POS)){
			player.Seek((double)lParam / 1000.0 * player.GetDuration());
		}else{
			player.SetVolume(int((double)lParam / 10.0));
			RSkin<CMainDlg>::SetVol(lParam);
		}
		return 1;
	}
	LRESULT OnPrev(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
		player.Stop();
		player.Play("c:\\1.mp3");
		RSkin<CMainDlg>::SetPosSliderValid(true);
		return 1;
	}
	static DWORD WINAPI callbackthread(LPVOID p){
		CMainDlg* pThis = (CMainDlg*)p;
		BOOL s;
		pThis->OnStop(0, 0, 0, s);
		return 1;
	}
	static void PlayerCallBack (int msg, int p1, char* p2, void* p){
		CMainDlg* pThis = (CMainDlg*)p;
		CreateThread(0, 0, pThis->callbackthread, p, 0, 0);
	}
};