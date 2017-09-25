#include "MainDlg2.h"

void CMainDlg2::DrawGlowText(HDC dc, RECT rcItem, const wchar_t* szText, bool right, bool vcenter){
	CDC dcMem;
	CBitmap bmp;
	BITMAPINFO dib = {0};
	dcMem.CreateCompatibleDC ( dc );
	dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	dib.bmiHeader.biWidth = rcItem.right - rcItem.left;
	dib.bmiHeader.biHeight = rcItem.top - rcItem.bottom;
	dib.bmiHeader.biPlanes = 1;
	dib.bmiHeader.biBitCount = 32;
	dib.bmiHeader.biCompression = BI_RGB;
	bmp.CreateDIBSection (dc,&dib,DIB_RGB_COLORS,NULL,NULL,0);
	dcMem.SelectBitmap ( bmp );
	dcMem.SelectFont ( m_font );
	
	DTTOPTS dto = { sizeof(DTTOPTS) };
	//const UINT uFormat = /*DT_SINGLELINE|*/DT_RIGHT|DT_VCENTER|DT_NOPREFIX;
	UINT uFormat = DT_NOPREFIX;
	right ? (uFormat |= DT_RIGHT) : uFormat;
	vcenter ? (uFormat |= DT_VCENTER) : uFormat;
	RECT rcText2 = rcItem;
	dto.dwFlags = DTT_COMPOSITED|DTT_GLOWSIZE;
	dto.iGlowSize = 8;
	rcText2.bottom -= rcText2.top; //相同的rect，但左上角为(0,0)
	rcText2.right -= rcText2.left + 5;
	rcText2.top = vcenter ? 5 : 0;
	rcText2.left = 5;

	::DrawThemeTextEx(m_hTheme, dcMem, 0, 0, szText, -1,uFormat, &rcText2, &dto );
	int rcWidth = rcItem.right - rcItem.left;
	int rcHeight = rcItem.bottom - rcItem.top;
	
	BitBlt( dc, rcItem.left, rcItem.top, rcWidth, rcHeight, dcMem, 0, 0, SRCCOPY );		
}

LRESULT CMainDlg2::OnTimer(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/){
	if (wParam == 1){
		RefreshTime();
	}
	else if (wParam == 2)
		SpectrumPainter.Paint();
	return 1;
}

void CMainDlg2::RefreshTime(){
	double now = player.GetPosition();
	double length = player.GetDuration();
	wchar_t temp[200];
	if (m_posTrackBar.SetPos((int(length) == 0) ? 0 : int(now / player.GetDuration() * 1000 + 1))){
		wsprintf(temp, L"%02d:%02d/%02d:%02d", int(now) / 60, int(now) % 60, int(length) / 60, int(length) % 60);
		time = temp;
		::SendMessage(GetDlgItem(IDC_STATIC_TIME2), WM_SETTEXT, 0, 0);
		//SetDlgItemText(IDC_STATIC_TIME2, temp);
		if (SpectrumWindow.IsWindow()){
			SpectrumWindow.SetTitle(this->name.c_str(), temp);
		}
	}

}

void CMainDlg2::DrawText(HDC hdc, RECT rcItem, const wchar_t* szText, bool right, bool vcenter){
	CDCHandle dc = hdc;
	if ( IsCompositionEnabled() ){
		DrawGlowText(dc, rcItem, szText, right, vcenter);
	}else{
		const UINT uFormat = DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_NOPREFIX;
		dc.SetTextColor ( RGB(255,255,255) );
		dc.SelectFont ( m_font );
		dc.SetBkMode ( TRANSPARENT );
		dc.DrawText(szText, -1, &rcItem, uFormat );
	}
}

void CMainDlg2::ShowPlayList(HWND listview, const PlayList& pl){
	RListViewCtrl lvc;
	lvc.Attach(listview);
	lvc.DeleteAllItems();
	for (UINT i = 0; i < pl.list.size(); i++){
		lvc.InsertItem(i, pl.list[i].strings[TITLE].str.c_str());
		lvc.SetItemText(i, 1, pl.list[i].strings[ARTIST].str.c_str());
		wchar_t buffer[50];
		UINT duration = pl.list[i].dwords[DURATION];
		wsprintf(buffer, L"%02u:%02u",  duration / 60, duration % 60);
		lvc.SetItemText(i, 2, buffer);
	}
}

void CMainDlg2::ShowPlayList(CListViewCtrl& listview, const PlayList& pl){
	listview.DeleteAllItems();
	for (UINT i = 0; i < pl.list.size(); i++){
		listview.InsertItem(i, pl.list[i].strings[TITLE].str.c_str());
		listview.SetItemText(i, 1, pl.list[i].strings[ARTIST].str.c_str());
		wchar_t buffer[50];
		UINT duration = pl.list[i].dwords[DURATION];
		wsprintf(buffer, L"%02u:%02u",  duration / 60, duration % 60);
		listview.SetItemText(i, 2, buffer);
	}
}
LRESULT CMainDlg2::OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled){
	LPDRAWITEMSTRUCT pDrawStruct = (LPDRAWITEMSTRUCT)lParam;
	CDCHandle dc = pDrawStruct->hDC;
	if (wParam == IDC_STATIC_TIME2){
		DrawText(dc, pDrawStruct->rcItem, time.c_str(), true);
	}else if (wParam == IDC_STATIC_DETAIL2){
		DrawText(dc, pDrawStruct->rcItem, detail.c_str(), true);
	}else if(wParam == IDC_STATIC_NAME2){
		//DrawText(dc, pDrawStruct->rcItem, name.c_str(), true);
		DrawText(dc, pDrawStruct->rcItem, L"", true);
	}else if (wParam == IDC_STATIC_STATUS2){
		DrawText(dc, pDrawStruct->rcItem, status.c_str(), false, false);
	}
	ReflectNotifications(uMsg, wParam, lParam, bHandled); 
	return 1;
}

LRESULT CMainDlg2::OnHScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/){
	if (GetDlgItem(IDC_POS_SLIDER) == (HWND)lParam){
		int cur;
		wchar_t temp[200];
		double length = player.GetDuration();
		switch(LOWORD(wParam)){
		case SB_THUMBTRACK:
			cur = (int(length) == 0) ? 0 : int( (double)HIWORD(wParam) * (double)length / 1000);
			wsprintf(temp, L"%02d:%02d/%02d:%02d", int(cur) / 60, int(cur) % 60, int(length) / 60, int(length) % 60);
			time = temp;
			SetDlgItemText(IDC_STATIC_TIME2, temp);
			break;
		case SB_THUMBPOSITION:
			length = player.GetDuration();
			double cur2 = HIWORD(wParam) * length / 1000;
			player.Seek(cur2);
			RefreshTime();
			break;
		}
	}else{
		switch(LOWORD(wParam)){
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			int vol = HIWORD(wParam) / 10;
			player.SetVolume(vol);
			break;
		}
	}
	return 1;
}

LRESULT CMainDlg2::OnRButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/){
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	ClientToScreen(&pt);
	
	HMENU hMenu = LoadMenu(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MENU1));
	HMENU hmenuTrackPopup = GetSubMenu(hMenu, 0); 
	TrackPopupMenu(hmenuTrackPopup, 0, pt.x, pt.y, 0, *this, NULL);
	DestroyMenu(hMenu);
	return 1;
}

LRESULT CMainDlg2::OnOpenSpectrum(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
	if (!SpectrumWindow.IsWindow()){
		SpectrumWindow.Create(this->m_hWnd);
		SpectrumWindow.ShowWindow(SW_SHOW);
	}
	return 1;
}

LRESULT CMainDlg2::OnListPlay(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/){
	wcscpy_s(wfilename, playlist.GetFileName(wParam, lParam).c_str());
	wcs2mbs(wfilename, filename, sizeof(filename));
	player.Play(filename);
	m_btnPlay.ModifyStyle(WS_VISIBLE, 0);
	m_btnPause.ModifyStyle(0, WS_VISIBLE);
	RefreshInfo();
	KillTimer(1);
	KillTimer(2);
	SetTimer(1, 200, NULL);
	SetTimer(2, 60, NULL);
	playlist.SetCurrent(wParam, lParam);
	return 1;
}

LRESULT CMainDlg2::OnAddSong(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/){
	COpenFileDlg opendlg;
	int index = playlist.GetPlayList(wParam).list.size();
	opendlg.OpenFile(player, (HWND)lParam, playlist, wParam);
	pt.go(&playlist.GetPlayList(wParam), (HWND)lParam, index, *this);
	return 1;
}

LRESULT CMainDlg2::OnRemove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){

	return 1;
}

LRESULT CMainDlg2::OnChangeSong(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){
	BOOL tmp;
	OnPlay(0, 0, 0, tmp);
	return 1;
}

LRESULT CMainDlg2::OnSetStatus(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/){
	status = (wchar_t*)wParam;
	SetDlgItemText(IDC_STATIC_STATUS2, L"");
	return 1;
}

void CMainDlg2::RefreshInfo(){
	ZeroMemory(&info, sizeof(info));
	ZeroMemory(&tagw, sizeof(tagw));
	player.GetDecoderManager()->GetInfo(&info);
	wchar_t name[MAX_PATH];
	GetFileTitle(wfilename, name, sizeof(name));
	for (UINT i = wcslen(name); i >= 0; i--){
		if(name[i] == '.'){
			name[i] = '\0';
			break;
		}
	}
	this->name = name;

	Tag.GetTagW(filename, &tagw);
	if (tagw.valid == true){
		if (tagw.artist[0] != '\0' && tagw.title[0] != '\0'){
			wchar_t tmp[200];
			wsprintf(tmp, L"%s - %s", tagw.artist.c_str(), tagw.title.c_str());
			this->name = tmp;
		}
	}
	//this->name = L"";
	//SetDlgItemText(IDC_STATIC_NAME2, name);
	SetWindowText(this->name.c_str());

	if (info.bitrate == 0){
		detail[0] = '\0';
	}else{
		wchar_t codec[30];
		mbs2wcs(info.codec, codec, 30);
		wsprintf(name, L"%dHz\n%d kbps\n%s", info.rate, info.bitrate, codec);
		detail = name;
	}
	::SendMessage(GetDlgItem(IDC_STATIC_DETAIL2), WM_SETTEXT, 0, 0);
}

LRESULT CMainDlg2::OnPlay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
	if (player.GetStatus() == PAUSE){
		player.Resume();
		m_btnPlay.ModifyStyle(WS_VISIBLE, 0);
		m_btnPause.ModifyStyle(0, WS_VISIBLE);
		KillTimer(1);
		KillTimer(2);
		SetTimer(1, 200, NULL);
		SetTimer(2, 60, NULL);
		return 1;
	}
	wcscpy_s(wfilename, playlist.GetNextFileName().c_str());
	wcs2mbs(wfilename, filename, MAX_PATH);
	player.Play(filename);
	
	m_btnPlay.ModifyStyle(WS_VISIBLE, 0);
	m_btnPause.ModifyStyle(0, WS_VISIBLE);

	RefreshInfo();
	SetTimer(1, 200, NULL);
	SetTimer(2, 60, NULL);
	return 1;
}

LRESULT CMainDlg2::OnPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
	player.Pause();
	m_btnPlay.ModifyStyle(0, WS_VISIBLE);
	m_btnPause.ModifyStyle(WS_VISIBLE, 0);
	KillTimer(1);
	KillTimer(2);
	return 1;
}

LRESULT CMainDlg2::OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
	player.Stop();
	m_btnPlay.ModifyStyle(0, WS_VISIBLE);
	m_btnPause.ModifyStyle(WS_VISIBLE, 0);
	KillTimer(1);
	KillTimer(2);
	return 1;
}

LRESULT CMainDlg2::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
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

	LOGFONT lf = {0};
	OpenThemeData();

	if ( !IsThemeNull() )
		GetThemeSysFont ( TMT_MSGBOXFONT, &lf );
	else
	{
		NONCLIENTMETRICS ncm = { sizeof(NONCLIENTMETRICS) };
		SystemParametersInfo (SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS),&ncm, false );
		lf = ncm.lfMessageFont;
	}
	m_font.CreateFontIndirect ( &lf );
	m_posTrackBar.SubclassWindow(GetDlgItem(IDC_POS_SLIDER), true);
	m_volTrackBar.SubclassWindow(GetDlgItem(IDC_VOL_SLIDER), false);
	m_volTrackBar.SetPos(player.GetVolume() * 10);
	m_btnPlay.SubclassWindow(GetDlgItem(IDC_BTN_PLAY));
	m_btnPause.SubclassWindow(GetDlgItem(IDC_BTN_PAUSE));
	m_btnStop.SubclassWindow(GetDlgItem(IDC_BTN_STOP));
	m_btnPause.ModifyStyle(WS_VISIBLE, 0);
	EnableThemeDialogTexture ( ETDT_ENABLE );
	SetWindowText(L"米饭团");
	detail[0] = time[0] = name[0] = '\0';
	ZeroMemory(&info, sizeof(info));
	ZeroMemory(&tagw, sizeof(tagw));
	name = L"米饭团 0.1.1";
	status = L"欢迎使用米饭团";

	//m_tabCtrl.SubclassWindow(GetDlgItem(IDC_TAB1));
	m_tabCtrl.Attach(GetDlgItem(IDC_TAB1));
	m_tabCtrl.InsertItem(0, L"临时列表");
	RECT rs; 
	m_tabCtrl.GetClientRect(&rs); 
	rs.top+=20; 
	rs.bottom-=4;
	rs.left+=4; 
	rs.right-=4; 
	m_listCtrl.Create(m_tabCtrl, &rs, _T("Temporary List"), WS_CLIPCHILDREN | WS_CHILD | LVS_REPORT | WS_VISIBLE);// | LVS_OWNERDRAWFIXED);
	m_listCtrl.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT /*| LVS_EX_FLATSB*/);
	m_listCtrl.InsertColumn(0, L"曲目");
	m_listCtrl.InsertColumn(1, L"歌手");
	m_listCtrl.InsertColumn(2, L"时长");
	m_listCtrl.SetColumnWidth(0, (rs.right - rs.left) / 10 * 5);
	m_listCtrl.SetColumnWidth(1, (rs.right - rs.left) / 10 * 3);
	m_listCtrl.SetColumnWidth(2, (rs.right - rs.left) / 10 * 2);
	//m_listCtrl.InsertItem(0, L"123123");
	//m_listCtrl.SetItemText(0, 1, L"456456");
	m_tabCtrl.SetCurSel(0); 
	m_listCtrl.ShowWindow(TRUE);

	RECT rectWindow;
	m_tabCtrl.GetClientRect(&rs); 
	m_tabCtrl.ClientToScreen(&rs);
	this->ScreenToClient(&rs);
	this->GetClientRect(&rectWindow);

	MARGINS mar = {0};
	mar.cyTopHeight = rs.top;
	mar.cxLeftWidth = rs.left;
	mar.cxRightWidth = rectWindow.right - rs.right;
	mar.cyBottomHeight = rectWindow.bottom - rs.bottom;
	if (IsCompositionEnabled()){
		DwmExtendFrameIntoClientArea(*this, &mar);
	}

	playlist.LoadPlayLists();
	ShowPlayList(m_listCtrl, playlist.GetPlayList(0));
	player.SetNotify(*this);

	DWORD color;
	BOOL alpha;
	DwmGetColorizationColor(&color, &alpha);

	SpectrumPainter.SetParam(GetDlgItem(IDC_STATIC_NAME2), RGB(0,0,0), 0xFFFFFFFF - color, RGB(0,0,0), 40);

	return TRUE;
}