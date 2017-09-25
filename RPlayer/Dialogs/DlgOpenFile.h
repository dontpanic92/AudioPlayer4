// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "../stdafx.h"
#include "../RPlayer/RPlayer.h"
#include "../Controls/RSkinAeroCtrl.h"
#include "../RThread.h"

class ProcessFileThread : public RThread{
private:
	//HANDLE hThread;
	RPlayer* m_player;
	PlayList* m_ppl;
	RListViewCtrl m_list;
	int startindex;
	HWND notify;

	/*static DWORD WINAPI Thread(LPVOID lpThreadParameter){
		ProcessFileThread* pThis = (ProcessFileThread*)lpThreadParameter;
		pThis->Proc();
		return 1;
	}*/
	void Proc(){
		SendMessage(notify, RM_SETSTATUS, (WPARAM)L"正在读取歌曲信息……", 0);
		int total = m_ppl->list.size() - startindex;
		RDecoderManager* pDecoder = m_player->GetDecoderManager();
		for(unsigned int i = startindex; i < m_ppl->list.size(); i++){
			Info info;
			char temp[MAX_PATH];
			wcs2mbs(m_ppl->list[i].strings[PATH].str.c_str(), temp, sizeof(temp));
			pDecoder->GetFileInfo(temp, &info);
			m_ppl->list[i].dwords[DURATION] = (DWORD)info.duration;

			wchar_t buffer[50];
			wsprintf(buffer, L"%02u:%02u",  (DWORD)info.duration / 60, (DWORD)info.duration % 60);
			m_list.SetItemText(i, 2, buffer);

			wsprintf(buffer, L"正在读取歌曲信息……%d%% （%d / %d）", int(double(i - startindex) / total * 100), i - startindex + 1, total);
			PostMessage(notify, RM_SETSTATUS, (WPARAM)buffer, 0);
			Sleep(15);
		}
		SendMessage(notify, RM_SETSTATUS, (WPARAM)L"读取完毕", 0);
	}
public:
	//ProcessFileThread() : hThread(INVALID_HANDLE_VALUE){}
	void go(PlayList* ppl, HWND list, int sindex, HWND notifyStatus){
		m_player = RPlayer::getInstance();
		m_ppl = ppl;
		m_list.Attach(list);
		notify = notifyStatus;
		startindex = sindex;

		//hThread = CreateThread(0, 0, Thread, this, 0, 0);
		RThread::go();
	}
	/*~ProcessFileThread(){
		if (hThread || hThread != INVALID_HANDLE_VALUE){
			TerminateThread(hThread, 0);
		}
		CloseHandle(hThread);
	}*/
};


class COpenFileDlg : public CDialogImpl<COpenFileDlg>, public CUpdateUI<COpenFileDlg>,
		public CMessageFilter, public CIdleHandler
{
private:
	wchar_t filepath[200000];
	WORD	nFileOffset;
	RListViewCtrl list;
	RPlayer* pplayer;
	RPlayList* prpl;
	CProgressBarCtrl progress1;
	TagSystem Tag;
	HANDLE hThread;
	bool abort;
	int lst;

	void ProcFile(){
		UINT index = prpl->GetPlayList(lst).list.size() - 1;
		wchar_t szPath[MAX_PATH];
		wchar_t szFile[MAX_PATH];
		wcsncpy_s(szPath, filepath, nFileOffset);
		
		szPath[nFileOffset] = '\0';
		int nLen = wcslen(szPath);
  
		if( szPath[nLen-1] != '\\' ){
			wcscat_s(szPath, TEXT("\\"));
		}
 
		wchar_t* p = filepath + nFileOffset;
		bool set = false;
		int j = 0;

		while(*p && !abort){
			j++;

			wcscpy_s(szFile, szPath);
			wcscat_s(szFile, p);

			wchar_t buffer[MAX_PATH];
			if (j % 30 == 0)
				SetDlgItemText(IDC_STATIC_PROCESSING, szFile);
			wsprintf(buffer, L"正在处理……（%d）", j);
			SetWindowText(buffer);

			int len = wcslen(p);
			//Info info;
			char temp[MAX_PATH];
			wcs2mbs(szFile, temp, sizeof(temp));
			//pplayer->GetFileInfo(temp, &info);
			SongInfo sinfo;
			sinfo.strings[PATH].str = szFile;

			for (UINT i = len - 1; i >= 0; i--){
				if(p[i] == '.'){
					p[i] = '\0';
					break;
				}
			}

			TAGW ttag;
			Tag.GetTagW(temp, &ttag);
			if (ttag.valid == true && ttag.artist[0] != '\0' && ttag.title[0] != '\0'){
				sinfo.strings[ARTIST].str = ttag.artist;
				sinfo.strings[TITLE].str = ttag.title;
			}else{
				sinfo.strings[TITLE].str = p;
			}
			//sinfo.dwords[DURATION] = (DWORD)info.duration;
			prpl->AddSong(sinfo, lst);

			index++;

			list.InsertItem(index, sinfo.strings[TITLE].str.c_str());
			list.SetItemText(index, 1, sinfo.strings[ARTIST].str.c_str());

			//UINT duration = sinfo.dwords[DURATION];
			//wsprintf(buffer, L"%02u:%02u",  duration / 60, duration % 60);
			//list.SetItemText(index, 2, buffer);

			p += len + 1;
			Sleep(10);
		}
		if (!abort)
			EndDialog(1);
	}

	static DWORD WINAPI processFile(LPVOID lpThreadParameter){
		COpenFileDlg* pDlg = (COpenFileDlg*)lpThreadParameter;
		pDlg->ProcFile();
		return 1;
	}
public:
	enum { IDD = IDD_DLGOPENFILE };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(COpenFileDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CProcessFileDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		CenterWindow();

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
		progress1.Attach(GetDlgItem(IDC_PROGRESS1));
		progress1.ModifyStyle(0, PBS_MARQUEE);
		progress1.SetMarquee(TRUE);
		return TRUE;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

		return 0;
	}

	void OpenFile(RPlayer& player, HWND hListWnd,RPlayList& playlist, int lst){
		char buffer[1000];
		wchar_t w_buffer[1000];
		player.GetDecoderManager()->GetSupportFileType(buffer);

		list.Attach(hListWnd);

		char* pBuffer = buffer;
		wchar_t* wpBuffer = w_buffer;
		while(*pBuffer){
			wchar_t temp[MAX_PATH];
			mbs2wcs(pBuffer, temp, sizeof(temp));
		
			wcscpy_s(wpBuffer, MAX_PATH, temp);
			wpBuffer += wcslen(temp);
			*wpBuffer = '\0';

			pBuffer += strlen(pBuffer) + 1;
			wpBuffer += 1;
		}
		*wpBuffer = '\0';

		OPENFILENAME ofn;
		ZeroMemory(&ofn,sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = filepath;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = 200000;
		ofn.lpstrFilter = w_buffer;///"Supported Music File\0*.mp3;*.wav;*.ogg;*.ape\0All File\0*.*\0\0"; 
		ofn.nFilterIndex = 1; 
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_EXPLORER |OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_CREATEPROMPT;
		wchar_t AppPath[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, AppPath);
		GetOpenFileName(&ofn);
		SetCurrentDirectory(AppPath);
		if (filepath[0] == '\0')
			return ;

		nFileOffset = ofn.nFileOffset;
		pplayer = &player;
		prpl = &playlist;
		this->lst = lst;
		abort = false;

		hThread = CreateThread(0, 0, processFile, this, 0, 0);
		DoModal();
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
		abort = true;
		WaitForSingleObject(hThread, 1000);
		TerminateThread(hThread, 0);
		CloseHandle(hThread);
		EndDialog(1);
	}
};
