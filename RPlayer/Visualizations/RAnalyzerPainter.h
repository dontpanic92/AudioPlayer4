#pragma once
#include <Windows.h>
#include "../RThread.h"
#include "../stdafx.h"
#include "RAnalyzer.h"

class RAnalyzerPainter{
protected:
	HWND hWnd;
	COLORREF BackgroundColor;
	COLORREF ForegroundColor1;
	COLORREF ForegroundColor2;

	RAnalyzer* pAnalyzer;
	double fps;

public:
	RAnalyzerPainter();
	RAnalyzerPainter(HWND hwnd);
	virtual ~RAnalyzerPainter();
	void SetWindowHandle(HWND hwnd);
	void SetColor(COLORREF bkg, COLORREF fore1, COLORREF fore2);
	void SetBkColor(COLORREF bkg);
	void SetFgColor(COLORREF fore1, COLORREF fore2);

	double GetFPS();

	virtual void Paint() = 0;
};