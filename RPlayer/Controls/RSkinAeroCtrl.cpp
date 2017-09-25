#include "RSkinAeroCtrl.h"

LRESULT RAeroButton::OnCtl(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/){
	static HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
	return (LRESULT)hBrush;
}

void RAeroButton::DrawItem(LPDRAWITEMSTRUCT lpStruct){
	CDCHandle dc = lpStruct->hDC;
	CDC dcMem;
	int width = lpStruct->rcItem.right - lpStruct->rcItem.left;
	int height = lpStruct->rcItem.bottom - lpStruct->rcItem.top;

	dcMem.CreateCompatibleDC(dc);
	HBITMAP hBitmap = LoadBitmap(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDB_BITMAP1));
	dcMem.SelectBitmap(hBitmap);
	dc.BitBlt(0, 0, width, height, dcMem, 0, 0, SRCCOPY);
	dcMem.DeleteDC();
}





void RListViewCtrl::SetPlayList(PlayList& pl, int listindex){
	list = listindex;
}

LRESULT RListViewCtrl::DeleteItem(LPDELETEITEMSTRUCT lpStruct){
	return 1;
}

LRESULT RListViewCtrl::OnRButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/){
	SendMessage(WM_LBUTTONDOWN, wParam, lParam);
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	ClientToScreen(&pt);

	HMENU hMenu = LoadMenu(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MENU1));
	HMENU hmenuTrackPopup = GetSubMenu(hMenu, 1); 
	TrackPopupMenu(hmenuTrackPopup, 0, pt.x, pt.y, 0, *this, NULL);
	DestroyMenu(hMenu);
	return 1;
}

LRESULT RListViewCtrl::OnPlay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
	int index = this->GetSelectedIndex();
	if (index != -1)
		GetParent().GetParent().SendMessage(RM_PLAY, list, index);
	return 1;
}

LRESULT RListViewCtrl::OnRemove(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
	int index = this->GetSelectedIndex();
	if (index != -1)
		GetParent().GetParent().SendMessage(RM_REMOVE, list, index);
	return 1;
}

LRESULT RListViewCtrl::OnAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
	GetParent().GetParent().SendMessage(RM_ADD_SONG, list, (LPARAM)m_hWnd);
	return 1;
}

LRESULT RListViewCtrl::OnBpmDetect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
	int index = this->GetSelectedIndex();
	//dlg.Create(*this);
	char filename[260];
	wcs2mbs(RPlayList::getInstance()->GetPlayList(list).list[index].strings[PATH].str.c_str(), filename, sizeof(filename));
	dlg.DetectBpm(filename);
	return 1;
}

LRESULT RListViewCtrl::DrawItem(LPDRAWITEMSTRUCT lpStruct){
	MessageBox(L"");
	CDCHandle pDC = lpStruct->hDC;//lpStruct->nmcd.hdc;
	RECT rcItem = lpStruct->rcItem;//lpStruct->nmcd.rc;
	int nItem = lpStruct->itemID;

    LV_ITEM lvi;
    lvi.mask = LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
    lvi.iItem = nItem;
	lvi.iSubItem = 0;//lpStruct->iSubItem;
    lvi.stateMask = 0xFFFF;        
    GetItem(&lvi);
    //描画每一行的背景
    COLORREF clrTextSave;
    COLORREF clrBkSave;
    if (lvi.state & LVIS_SELECTED)
    {
        clrBkSave = pDC.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
        HBRUSH cb= CreateSolidBrush(::GetSysColor(COLOR_HIGHLIGHT));
        pDC.FillRect(&rcItem, cb);
		DeleteObject(cb);
    }
    else
    {
        if (nItem % 2)    //斑马线颜色
        {
            HBRUSH cb = CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
            pDC.FillRect(&rcItem, cb);
			DeleteObject(cb);
        }
        else
        {
            HBRUSH cb = CreateSolidBrush((COLORREF)0x00f0f0f0);
            pDC.FillRect(&rcItem, cb);
        }
    }
    //描画位图
    //CImageList* pImageList = GetImageList(LVSIL_SMALL);
    //pImageList->Draw(pDC, lvi.iImage, rcItem.TopLeft(), ILD_TRANSPARENT);

    //CRect rectItem(0, 0, 0, 0);
    //CRect rectText;
	RECT rectItem;
	RECT rectText;

    for (int j = 0; j < 3; j ++)
    {
        if (j == 0)
            GetItemRect(nItem, &rectItem, LVIR_LABEL);
        else
        {
            rectItem.left = rectItem.right;
            rectItem.right += GetColumnWidth(j);
        }

        rectText = rectItem;
        rectText.right -= 4;

        wchar_t strData[50];
		GetItemText(nItem, j, strData, sizeof(strData));

        if ((lvi.state & LVIS_SELECTED))
            clrTextSave = pDC.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
        
        if (rectText.right >= rectText.left)
            pDC.DrawText(strData, wcslen(strData), &rectText, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);

        if ((lvi.state & LVIS_SELECTED))
            pDC.SetTextColor(clrTextSave);
    }
    
    if (lvi.state & LVIS_SELECTED)
        pDC.SetBkColor(clrBkSave);
    //描画焦点
    if ((lvi.state & LVIS_FOCUSED) != 0 && GetFocus() == *this)
        pDC.DrawFocusRect(&rcItem);
	return 1;
}


 FARPROC OldProc = NULL;
 FARPROC OldProc2 = NULL;

LRESULT RAeroTrackBar::OnCtl(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/){
	static HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
	return (LRESULT)hBrush;
}

BOOL RAeroTrackBar::SubclassWindow(_In_ HWND hWnd, bool pos) throw(){
	BOOL ret = CWindowImpl<RAeroTrackBar, CTrackBarCtrl>::SubclassWindow(hWnd);
	if (pos == true)
		OldProc = (FARPROC)SetWindowLong(GWL_WNDPROC, (long)PosProc);
	else
		OldProc2 = (FARPROC)SetWindowLong(GWL_WNDPROC, (long)PosProc2);
	SendMessage(TBM_SETPAGESIZE, 0, 0);
	SendMessage(TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, POSRANGE));
	return ret;
}

BOOL RAeroTrackBar::SetPos(int Pos){
	if (!SendMessage(TBM_SETPOS, true, Pos))
		return TRUE;
	else
		return FALSE;
}

LRESULT CALLBACK RAeroTrackBar::PosProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	HWND hDlg = ::GetParent(hWnd);
	RECT   Rect;
	static bool OnDraging = false;
	int Position;
	switch (uMsg){
	case WM_LBUTTONDOWN:
		SendMessage(hWnd,TBM_GETCHANNELRECT, 0, (long)&Rect); 
		Position = int(1.0 * POSRANGE *(LOWORD(lParam)-Rect.left - 3)/(Rect.right - Rect.left - 6)); 
		SendMessage(hWnd,TBM_SETPOS, true, Position);
		OnDraging = true;
		break;
	case WM_LBUTTONUP:
		SendMessage(hWnd,TBM_GETCHANNELRECT, 0, (long)&Rect); 
		Position = int(1.0 * POSRANGE *(LOWORD(lParam)-Rect.left - 3)/(Rect.right - Rect.left - 6)); 
		OnDraging = false;
		if (Position < 0)
			Position = 0;
		break;
	case TBM_SETPOS:
		if (OnDraging)
			return 1;
		break;
	}
	return CallWindowProc((WNDPROC)OldProc,   hWnd,   uMsg,   wParam,   lParam);
}

LRESULT CALLBACK RAeroTrackBar::PosProc2(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	HWND hDlg = ::GetParent(hWnd);
	RECT   Rect;
	static bool OnDraging = false;
	int Position;
	switch (uMsg){
	case WM_LBUTTONDOWN:
		SendMessage(hWnd,TBM_GETCHANNELRECT, 0, (long)&Rect); 
		Position = int(1.0 * POSRANGE *(LOWORD(lParam)-Rect.left - 3)/(Rect.right - Rect.left - 6)); 
		SendMessage(hWnd,TBM_SETPOS, true, Position);
		OnDraging = true;
		break;
	case WM_LBUTTONUP:
		SendMessage(hWnd,TBM_GETCHANNELRECT, 0, (long)&Rect); 
		Position = int(1.0 * POSRANGE *(LOWORD(lParam)-Rect.left - 3)/(Rect.right - Rect.left - 6)); 
		OnDraging = false;
		if (Position < 0)
			Position = 0;
		break;
	case TBM_SETPOS:
		if (OnDraging)
			return 1;
		break;
	}
	return CallWindowProc((WNDPROC)OldProc2,   hWnd,   uMsg,   wParam,   lParam);
}
