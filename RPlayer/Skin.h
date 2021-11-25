#include "stdafx.h"
#include "resource.h"
#include "Controls/RSkinCtrl.h"
#include <fstream>
#include "3rdparty/ZLibWrap.h"
#include "3rdparty/rapidxml/rapidall.h"
#pragma comment(lib, "lib/zlibwrap.lib")
#pragma once

#define IDC_STATIC_POS                  1010
#define IDC_STATIC_TIME					1011
#define IDC_STATIC_VOL					1012
#define BTN_NUM 8

struct btn{
	char name[50];
	int id;
};
extern const btn btnID[8];
int findBtnId(const char*);

template <class T>
class RSkin{
public:
	BEGIN_MSG_MAP(RSkin)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnDrawBkgnd);
	END_MSG_MAP()

private:
	struct btnStruct{
		wchar_t BMPFileName[50];
		HBITMAP hBtnBmp;
		RECT rect;
		CImageList btnIml;
		CBitmapButton btn;
		int btnId;
	}Buttons[BTN_NUM];

	T* pT;
	bool loaded;
	DWORD TransColor;
	HBITMAP hBMPWindow;
	HRGN hWindowRgn;
	int wX, wY;

	RSlider posSlider;
	RECT	rectPos;
	wchar_t BMPPos[50];
	wchar_t BMPPosThumb[50];

	RSlider volSlider;
	RECT	rectVol;
	wchar_t BMPVol[50];
	wchar_t BMPVolThumb[50];


	RTimeStatic staticTime;
	RECT rectTime;
	wchar_t BMPTime[50];
	

	wchar_t BMPWindow[50];

	bool LoadXmlFile();
	void ReleaseSkin();
protected:

public:
	RSkin();
	~RSkin();
	void btnShowHide(int id, bool show);
	void LoadSkin(const wchar_t* path);
	void DrawTime(int now);
	void SetVol(int vol);
	void SetPos(int pos);
	void SetPosSliderValid(bool valid);
	LRESULT OnDrawBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

template <class T>
RSkin<T>::RSkin(){
	pT = static_cast<T*>(this);
	TransColor = RGB(255,0,255);
	hBMPWindow = NULL;
	hWindowRgn = NULL;
	loaded = false;
}

template <class T>
RSkin<T>::~RSkin(){ReleaseSkin();}

template <class T>
bool RSkin<T>::LoadXmlFile(){
	using namespace rapidxml;
	file<> fdoc("1.xml");
	xml_document<>  doc;    
    doc.parse<0>(fdoc.data());
	xml_node<>* root = doc.first_node("rskin");
	if (!root)
		return false;
	xml_attribute<>* attr = root->first_attribute("img");
	mbs2wcs(attr->value(), BMPWindow, sizeof(BMPWindow));
	xml_node<>* node = root->first_node("Button");
	for (int i = 0; node && i < BTN_NUM; i++){
		attr = node->first_attribute("id");
		Buttons[i].btnId = findBtnId(attr->value());
		attr = node->first_attribute("img");
		mbs2wcs(attr->value(), Buttons[i].BMPFileName, sizeof(Buttons[i].BMPFileName));
		attr = node->first_attribute("x");
		Buttons[i].rect.left = atoi(attr->value());
		attr = node->first_attribute("y");
		Buttons[i].rect.top = atoi(attr->value());
		Buttons[i].rect.bottom = Buttons[i].rect.right;
		node = node->next_sibling("Button");
	}
	node = root->first_node("Static");
	attr = node->first_attribute("id");
	if (!strcmp(attr->value(), "staticTime")){
		attr = node->first_attribute("x1");
		rectTime.left = atoi(attr->value());
		attr = node->first_attribute("y1");
		rectTime.top = atoi(attr->value());
		attr = node->first_attribute("x2");
		rectTime.right = atoi(attr->value());
		attr = node->first_attribute("y2");
		rectTime.bottom = atoi(attr->value());
		attr = node->first_attribute("img");
		mbs2wcs(attr->value(), BMPTime, sizeof(BMPTime));
	}
	int n = 2;
	node = root->first_node("Slider");
	while (n--){
		attr = node->first_attribute("id");
		if (!strcmp(attr->value(), "posSlider")){
			attr = node->first_attribute("x1");
			rectPos.left = atoi(attr->value());
			attr = node->first_attribute("y1");
			rectPos.top = atoi(attr->value());
			attr = node->first_attribute("x2");
			rectPos.right = atoi(attr->value());
			attr = node->first_attribute("y2");
			rectPos.bottom = atoi(attr->value());
			attr = node->first_attribute("img");
			mbs2wcs(attr->value(), BMPPos, sizeof(BMPPos));
			attr = node->first_attribute("thumbimg");
			mbs2wcs(attr->value(), BMPPosThumb, sizeof(BMPPosThumb));
		}else{
			attr = node->first_attribute("x1");
			rectVol.left = atoi(attr->value());
			attr = node->first_attribute("y1");
			rectVol.top = atoi(attr->value());
			attr = node->first_attribute("x2");
			rectVol.right = atoi(attr->value());
			attr = node->first_attribute("y2");
			rectVol.bottom = atoi(attr->value());
			attr = node->first_attribute("img");
			mbs2wcs(attr->value(), BMPVol, sizeof(BMPVol));
			attr = node->first_attribute("thumbimg");
			mbs2wcs(attr->value(), BMPVolThumb, sizeof(BMPVolThumb));
		}
		node = node->next_sibling("Slider");
	}
	return true;
}

template <class T>
void RSkin<T>::LoadSkin(const wchar_t* name){
	wchar_t tmpDir[MAX_PATH], path[MAX_PATH];
	GetTempPath(MAX_PATH, tmpDir);
	wcscat_s(tmpDir, L"\\RPLAYERSKIN");
	if (name != NULL && name[0] != '\0'){
		wcscpy_s(path, name);
	}else{
		wcscpy_s(path, L"..\\skin\\default.zip");
	}
	if (PathFileExists(path)){
		if (!ZWZipExtract(path, tmpDir)){
			MessageBox(NULL, L"Can't Decompress Skin File.", L"", 0);
			return;
		}
	}else{
		MessageBox(NULL, L"Skin File doesn't exist.", L"", 0);
		return;
	}
	LoadXmlFile();
	ReleaseSkin();
	wsprintf(path, L"%s\\%s", tmpDir, BMPWindow);
	hBMPWindow = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (!hBMPWindow)
		return;
	HDC hdc = CreateCompatibleDC(NULL);
	SelectObject(hdc, hBMPWindow);
	HRGN tmpRgn;
	BITMAP tmp;
	GetObject(hBMPWindow, sizeof(tmp), &tmp);
	bool first = true;
	for (int y = 0; y < tmp.bmHeight; y++){
		for (int x = 0; x < tmp.bmWidth; x++){
			if (GetPixel(hdc, x, y) != TransColor){
				tmpRgn = CreateRectRgn(x, y, x + 1, y + 1);
				if (first == true){
					first = false;
					hWindowRgn = CreateRectRgn(x, y, x + 1, y + 1);
				}else{
					CombineRgn(hWindowRgn, hWindowRgn, tmpRgn, RGN_OR);
				}
				DeleteObject(tmpRgn);
			}
		}
	}
	DeleteDC(hdc);
	wX = tmp.bmWidth;
	wY = tmp.bmHeight;

	RECT rect;
	pT->GetWindowRect(&rect);
	pT->SetWindowPos(0, rect.left, rect.top, wX, wY, 0);
	pT->SetWindowRgn(hWindowRgn, true);

	
	staticTime.Create(pT->m_hWnd, rectTime, _T("StaticTime"), WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, 0);
	staticTime.SetWindowPos(0, rectTime.left, rectTime.top, rectTime.right - rectTime.left, rectTime.bottom - rectTime.top, SWP_NOZORDER);
	staticTime.SetBk(hBMPWindow);
	staticTime.SetImg(BMPTime);

	posSlider.Create(pT->m_hWnd, NULL, _T("PosSlider"),  0, 0, IDC_STATIC_POS);
	posSlider.SetWindowPos(0, rectPos.left, rectPos.top, rectPos.right - rectPos.left, rectPos.bottom - rectPos.top, SWP_NOZORDER);
	posSlider.SetBk(hBMPWindow);
	posSlider.SetSkin(BMPPos, BMPPosThumb);
	posSlider.ModifyStyle(0, SS_OWNERDRAW | SS_NOTIFY | WS_VISIBLE);

	volSlider.Create(pT->m_hWnd, NULL, _T("VolSlider"),  0, 0, IDC_STATIC_VOL);
	volSlider.SetWindowPos(0, rectVol.left, rectVol.top, rectVol.right - rectVol.left, rectVol.bottom - rectVol.top, SWP_NOZORDER);
	volSlider.SetBk(hBMPWindow);
	volSlider.SetSkin(BMPVol, BMPVolThumb);
	volSlider.ModifyStyle(0, SS_OWNERDRAW | SS_NOTIFY | WS_VISIBLE);
	volSlider.SetPos(1000);

	for (int i = 0; i < BTN_NUM; i++){
		wsprintf(path, L"%s\\%s", tmpDir, Buttons[i].BMPFileName);
		Buttons[i].hBtnBmp = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		BITMAP temp;
		GetObject(Buttons[i].hBtnBmp, sizeof(temp), &temp);
		Buttons[i].btnIml.Create(temp.bmWidth / 4, temp.bmHeight, ILC_COLOR32, 0, 0);
		Buttons[i].btnIml.Add(Buttons[i].hBtnBmp);

		Buttons[i].btn.SetBitmapButtonExtendedStyle(BMPBTN_HOVER);
		Buttons[i].btn.Create(pT->m_hWnd, 0, 0, 0, 0, Buttons[i].btnId);
		Buttons[i].btn.SetImageList(Buttons[i].btnIml);
		Buttons[i].btn.SetImages(0, 2, 1, 3);
		Buttons[i].rect.right = Buttons[i].rect.left + temp.bmWidth / 4;
		Buttons[i].rect.bottom = Buttons[i].rect.top + temp.bmHeight;
		Buttons[i].btn.SetWindowPos(0, Buttons[i].rect.left, Buttons[i].rect.top, temp.bmWidth / 4, temp.bmHeight, SWP_NOZORDER);
		if (Buttons[i].btnId != findBtnId("btnPause"))
			Buttons[i].btn.ModifyStyle(0, WS_VISIBLE);
	}
}

template <class T>
void RSkin<T>::ReleaseSkin(){
	TransColor = RGB(255,0,255);
	if (hBMPWindow)
		DeleteObject(hBMPWindow);
	if (hWindowRgn)
		DeleteObject(hWindowRgn);
	hBMPWindow = NULL;
	hWindowRgn = NULL;
	loaded = false;
	wX = wY = 0;

	if (posSlider) {
		posSlider.DestroyWindow();
	}

	if (staticTime) {
		staticTime.DestroyWindow();
	}

	for(int i = 0; i < BTN_NUM; i++){
		if (Buttons[i].btnIml) {
			Buttons[i].btnIml.RemoveAll();
		}

		if (Buttons[i].btn) {
			Buttons[i].btn.ModifyStyle(WS_VISIBLE, 0);
		}

		if (Buttons[i].hBtnBmp) {
			DeleteObject(Buttons[i].hBtnBmp);
		}
	}
}
template <class T>
void RSkin<T>::btnShowHide(int id, bool show){
	for (int i = 0; i < BTN_NUM; i++){
		if (Buttons[i].btnId == id){
			if (show)
				Buttons[i].btn.ModifyStyle(0, WS_VISIBLE); 
			else
				Buttons[i].btn.ModifyStyle(WS_VISIBLE, 0);
			Buttons[i].btn.Invalidate();
		}
	}
}
template <class T>
LRESULT RSkin<T>::OnDrawBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled){
	if (!hBMPWindow)
		return 0;

	CDC hdc = pT->GetDC();
	CDC dcMem;
	dcMem.CreateCompatibleDC(hdc);
	dcMem.SelectBitmap(hBMPWindow);
	hdc.BitBlt(0, 0, wX, wY, dcMem, 0, 0, SRCCOPY);
	dcMem.DeleteDC();
	for (int i = 0; i < BTN_NUM; i++){
		Buttons[i].btn.Invalidate();
	}
	posSlider.Invalidate();
	return 1;
}
template <class T>
void RSkin<T>::DrawTime(int now){
	char tmp[10];
	sprintf_s(tmp, "%02d:%02d", now / 60, now % 60);
	staticTime.DrawText(tmp);
}
template <class T>
void RSkin<T>::SetPos(int pos){
	posSlider.SetPos(pos);
}
template <class T>
void RSkin<T>::SetVol(int vol){
	volSlider.SetPos(vol);
}
template <class T>
void RSkin<T>::SetPosSliderValid(bool valid){
	posSlider.SetValid(valid);
}