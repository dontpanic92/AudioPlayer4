#include "RSkinCtrl.h"

LRESULT RTimeStatic::OnErase(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/){
	DrawText(NULL);
	return 1;
}

void RTimeStatic::SetBk(HBITMAP hbk){
	hBMPBackground = hbk;
}

void RTimeStatic::SetImg(wchar_t* szPath){
	DeleteObject(hNumbers);
	TCHAR tmpDir[MAX_PATH];
	TCHAR Path[MAX_PATH];
	GetTempPath(MAX_PATH, tmpDir);
	wcscat_s(tmpDir, L"\\RPLAYERSKIN");
	wsprintf(Path, L"%s\\%s", tmpDir, szPath);
	hNumbers = (HBITMAP)LoadImage(NULL, Path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	BITMAP bmp;
	GetObject(hNumbers, sizeof(bmp), &bmp);
	width = bmp.bmWidth / 12;
}

void RTimeStatic::DrawText(const char* time){
	CDC dc = GetDC();
	RECT rect, myrect;
	GetClientRect(&rect);
	myrect = rect;
	ClientToScreen(&rect);
	GetParent().ScreenToClient(&rect);
	int rcheight =  rect.bottom - rect.top;
	int rcwidth = rect.right - rect.left;
	CDC dcMem;
	CDC dcMem2;
	CDC dcParent = GetParent().GetDC();
	dcMem.CreateCompatibleDC(dcParent);
	dcMem2.CreateCompatibleDC(dc);
	HBITMAP hMap = ::CreateCompatibleBitmap( dc, rcwidth, rect.bottom - rect.top );
	if (hBMPBackground)
		dcMem.SelectBitmap(hBMPBackground);
	::SelectObject(dcMem2, hMap);
	dcMem2.BitBlt(0, 0, rcwidth, rcheight, dcMem, rect.left, rect.top, SRCCOPY);
	dcMem.SelectBitmap(hNumbers);
	if (time){
		int l = (int)strlen(time);
		for (int i = 0; time[i] != 0; i++){
			int x = myrect.right - (i + 1) * width;
			if (time[l - i - 1] <= '9' && time[l - i - 1] >= '0'){
				dcMem2.BitBlt(x, 0, width,rcheight, dcMem, (time[l - i - 1] - '0') * width, 0, SRCCOPY);
			}else{
				dcMem2.BitBlt(x, 0, width, rcheight, dcMem, 10 * width, 0, SRCCOPY);
			}
		}
	}
	dc.BitBlt(0, 0, rcwidth, rcheight, dcMem2 ,0, 0, SRCCOPY);
	dcMem.DeleteDC();
	dcMem2.DeleteDC();
	dcParent.DeleteDC();
	::DeleteObject(hMap);
	dc.DeleteDC();
}



LRESULT RBitmapButton::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled){
	static HCURSOR hCur = LoadCursor(NULL, IDC_HAND);
	SetCursor(hCur);
	return 1;
}

LRESULT RBitmapButton::OnMyMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled){
	::SendMessage(GetParent(), WM_PARENTNOTIFY, WM_MOUSEMOVE, lParam);
	bHandled =false;
	return 1;
}

LRESULT RBitmapButton::OnMyLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled){
	::SendMessage(GetParent(), WM_PARENTNOTIFY, WM_LBUTTONUP, lParam);
	bHandled =false;
	return 1;
}




void RSlider::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/){   
	CDC dc = GetDC();
	RECT rect;
	GetClientRect(&rect);
	ClientToScreen(&rect);
	GetParent().ScreenToClient(&rect);
	int rcheight =  rect.bottom - rect.top;
	int rcwidth = rect.right - rect.left;
	CDC dcMem;
	CDC dcMem2;
	CDC dcParent = GetParent().GetDC();
	dcMem.CreateCompatibleDC(dcParent);
	dcMem2.CreateCompatibleDC(dc);
	HBITMAP hMap = ::CreateCompatibleBitmap( dc, rcwidth, rect.bottom - rect.top );
	if (hBMPWindow)
		dcMem.SelectBitmap(hBMPWindow);
	::SelectObject(dcMem2, hMap);
	dcMem2.BitBlt(0, 0, rcwidth, rcheight, dcMem, rect.left, rect.top, SRCCOPY);
	dcMem.SelectBitmap(hProgressBmp);
	if (!vertical)
		dcMem2.BitBlt(1, (rcheight - ThumbBmp.bmHeight) / 2, pos * (rcwidth - ThumbBmp.bmWidth / 4) / 1000, ThumbBmp.bmHeight, dcMem, 0, 0, SRCCOPY);
	else
		dcMem2.BitBlt((rcwidth - ThumbBmp.bmWidth / 4) / 2, rcheight - pos * (rcheight - ThumbBmp.bmHeight) / 1000, ThumbBmp.bmWidth / 4, pos * (rcheight - ThumbBmp.bmHeight) / 1000, dcMem, 0, 0, SRCCOPY);
	dcMem.SelectBitmap(hThumbBmp);
	if (!vertical)
		dcMem2.BitBlt(pos * (rcwidth - ThumbBmp.bmWidth / 4 - 1) / 1000, (rcheight - ThumbBmp.bmHeight) / 2, ThumbBmp.bmWidth / 4, ThumbBmp.bmHeight, dcMem, ThumbBmp.bmWidth / 4, 0, SRCCOPY);
	else
		dcMem2.BitBlt((rcwidth - ThumbBmp.bmWidth / 4) / 2, rcheight - ThumbBmp.bmHeight - pos * (rcheight - ThumbBmp.bmHeight) / 1000, ThumbBmp.bmWidth / 4, ThumbBmp.bmHeight, dcMem, ThumbBmp.bmWidth / 4, 0, SRCCOPY);
	dc.BitBlt(0, 0, rcwidth, rcheight, dcMem2 ,0, 0, SRCCOPY);
	dcMem.DeleteDC();
	dcMem2.DeleteDC();
	dcParent.DeleteDC();
	::DeleteObject(hMap);
	dc.DeleteDC();
	if (!vertical)
		Thumb.SetWindowPos(0, pos * (rcwidth - ThumbBmp.bmWidth / 4 - 1) / 1000, (rcheight - ThumbBmp.bmHeight) / 2, ThumbBmp.bmWidth / 4, ThumbBmp.bmHeight, SWP_NOZORDER);
	else
		Thumb.SetWindowPos(0, (rcwidth - ThumbBmp.bmWidth / 4) / 2, rcheight - ThumbBmp.bmHeight - pos * (rcheight - ThumbBmp.bmHeight) / 1000, ThumbBmp.bmWidth / 4, ThumbBmp.bmHeight, SWP_NOZORDER);
}

LRESULT RSlider::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled){
	::SetCursor(LoadCursor(NULL,IDC_HAND));
	return 1;
}

void RSlider::privateSetPos(int pos){
	this->pos = pos;
}

LRESULT RSlider::OnParentNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled){
	if (!valid)
		return 1;
	static POINT point2 = {0}; 
	switch(wParam){
	case WM_LBUTTONDOWN:
		ondraging = true;
		point2.x = GET_X_LPARAM(lParam);
		point2.y = GET_Y_LPARAM(lParam);
		ClientToScreen(&point2);
		Thumb.ScreenToClient(&point2);
		::SendMessageA(GetParent(), WM_USER_DRAGGING, WPARAM(m_hWnd), pos);
		break;
	case WM_MOUSEMOVE:
		if (ondraging){
			POINT point;
			point.x = GET_X_LPARAM(lParam);
			point.y = GET_Y_LPARAM(lParam);
			Thumb.ClientToScreen(&point);
			ScreenToClient(&point);
			RECT rc;
			GetClientRect(&rc);
			int pos ;
			if (!vertical)
				pos = (point.x - point2.x) * 1000 / (rc.right - rc.left - ThumbBmp.bmWidth / 4);
			else
				pos = (rc.bottom - rc.top - ThumbBmp.bmHeight - point.y + ThumbBmp.bmHeight - point2.y) * 1000 / (rc.bottom - rc.top - ThumbBmp.bmHeight);
			pos = (pos > 1000) ? 1000 : pos;
			pos = (pos < 0) ? 0 : pos;
			privateSetPos(pos);
			DrawItem(NULL);
			::SendMessageA(GetParent(), WM_USER_DRAGGING, WPARAM(m_hWnd), pos);
		}
		break;
	case WM_LBUTTONUP:
		if (ondraging == true){
			ondraging = false;
			::SendMessageA(GetParent(), WM_USER_DRAGGED, WPARAM(m_hWnd), pos);
		}
		break;
	}
	return 1;
}

LRESULT RSlider::OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/){
	if (!valid)
		return 1;
	int xpos = GET_X_LPARAM(lParam);
	int ypos = GET_Y_LPARAM(lParam);
	int tmppos = 0;
	RECT rc;
	GetClientRect(&rc);
	if (!vertical){
		if (xpos - ThumbBmp.bmWidth / 4 / 2 < 0)
			xpos = 0;
		else if (xpos + ThumbBmp.bmWidth / 4 / 2 > rc.right - rc.left)
			xpos = rc.right - rc.left - ThumbBmp.bmWidth / 4;
		else
			xpos -= ThumbBmp.bmWidth / 4 / 2;
		tmppos = xpos * 1000 / (rc.right - rc.left - ThumbBmp.bmWidth / 4);
	}else{
		if (ypos - ThumbBmp.bmHeight / 2 < 0)
			ypos = rc.bottom - rc.top - ThumbBmp.bmHeight;
		else if (ypos + ThumbBmp.bmHeight / 2 > rc.bottom - rc.top)
			ypos = 0;
		else
			ypos = rc.bottom - rc.top - ypos - ThumbBmp.bmHeight / 2;
		tmppos = ypos * 1000 / (rc.bottom - rc.top - ThumbBmp.bmHeight);		
	}
	DrawItem(NULL);
	::SendMessageA(GetParent(), WM_USER_DRAGGED, WPARAM(m_hWnd), tmppos);
	return 1;
}

LRESULT RSlider::OnErase(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/){
	DrawItem(NULL);
	Thumb.Invalidate();
	return 1;
}

void RSlider::SetBk(HBITMAP hBmp){
	hBMPWindow = hBmp;
}

void RSlider::ReleaseSkin(){
	Thumb.DestroyWindow();
	DeleteObject(hThumbBmp);
	DeleteObject(hProgressBmp);
	hThumbBmp = NULL;
	hProgressBmp = NULL;
	imgLst.RemoveAll();
	Thumb.ModifyStyle(WS_VISIBLE, 0);
}

void RSlider::SetSkin(wchar_t* szProgress, wchar_t* szThumb){
	ReleaseSkin();
	Thumb.Create(m_hWnd, NULL, _T("SliderThumb"), WS_CHILD | WS_VISIBLE, 0, IDC_THUMB);
	TCHAR tmpDir[MAX_PATH];
	TCHAR bmpPath[MAX_PATH];
	GetTempPath(MAX_PATH, tmpDir);
	wcscat_s(tmpDir, L"\\RPLAYERSKIN");
	wsprintf(bmpPath, L"%s\\%s", tmpDir, szThumb);
	hThumbBmp = (HBITMAP)LoadImage(NULL, bmpPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	wsprintf(bmpPath, L"%s\\%s", tmpDir, szProgress);
	hProgressBmp = (HBITMAP)LoadImage(NULL, bmpPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	GetObject(hThumbBmp, sizeof(ThumbBmp), &ThumbBmp);
	imgLst.Create(ThumbBmp.bmWidth / 4, ThumbBmp.bmHeight, ILC_COLOR32, 0, 0);
	imgLst.Add(hThumbBmp);
	Thumb.SetBitmapButtonExtendedStyle(BMPBTN_HOVER);
	Thumb.SetImageList(imgLst);
	Thumb.SetImages(0, 2, 1, 3);
	Thumb.SetWindowPos(0, 0, 0, ThumbBmp.bmWidth / 4, ThumbBmp.bmHeight, SWP_NOZORDER);
	Thumb.ModifyStyleEx(WS_EX_NOPARENTNOTIFY, WS_VISIBLE);
}

void RSlider::SetPos(int pos){
	if (!ondraging){
		this->pos = pos;
		DrawItem(NULL);
	}
}

void RSlider::SetVertical(bool vert){
	vertical = vert;
}

void RSlider::SetValid(bool valid){
	this->valid = valid;
}