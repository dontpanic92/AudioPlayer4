#include "RSpectrumWindow.h"

LRESULT RSpectrumWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// create command bar window
		
	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);
	SetTimer(1, 50);
	SpectrumPainter.SetParam(*this, RGB(0,0,0), RGB(0,0,255), RGB(0,0,0), 40);
	UISetCheck(ID_SPECTRUMWINDOW_40, 1);
	time = GetTickCount();
	return 0;
}

LRESULT RSpectrumWindow::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){
	//::PostQuitMessage(0);
	DestroyWindow();
	KillTimer(1);
	return 1;
}

LRESULT RSpectrumWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);
	return 1;
}

LRESULT RSpectrumWindow::OnDrawBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled){
	return 0;
}

LRESULT RSpectrumWindow::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled){
	time2 = GetTickCount();
	fps = 0;
	if (time2 != time){
		fps = 1000.0 / (time2 - time);
	}
	time = time2;
	SpectrumPainter.Paint();
	return 0;
}

void RSpectrumWindow::SetTitle(const wchar_t* name, const wchar_t* time){
	wchar_t s[512];
	swprintf(s, L"Spectrum - %s   %s   FPS: %.3f", name, time, fps);
	SetWindowText(s);
}

LRESULT RSpectrumWindow::OnRButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/){
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	ClientToScreen(&pt);

	HMENU hMenu = LoadMenu(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MENU1));
	HMENU hmenuTrackPopup = GetSubMenu(hMenu, 2); 
	TrackPopupMenu(hmenuTrackPopup, 0, pt.x, pt.y, 0, *this, NULL);
	DestroyMenu(hMenu);
	return 1;
}

void RSpectrumWindow::SetBar(int bars, int ID){
		SpectrumPainter.SetBars(bars);
		UISetCheck(ID_SPECTRUMWINDOW_10, 0);
		UISetCheck(ID_SPECTRUMWINDOW_20, 0);
		UISetCheck(ID_SPECTRUMWINDOW_40, 0);
		UISetCheck(ID_SPECTRUMWINDOW_80, 0);
		UISetCheck(ID_SPECTRUMWINDOW_160, 0);
		UISetCheck(ID_SPECTRUMWINDOW_320, 0);
		UISetCheck(ID, 1);
	}