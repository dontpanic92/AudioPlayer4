#include "RAnalyzerPainter.h"

RAnalyzerPainter::RAnalyzerPainter():hWnd(NULL){
	pAnalyzer = new RAnalyzer();
	pAnalyzer->StartFFTAnalyze();
}

RAnalyzerPainter::RAnalyzerPainter(HWND hwnd){
	hWnd = hwnd;
	pAnalyzer->StartFFTAnalyze();
}

RAnalyzerPainter::~RAnalyzerPainter(){
	pAnalyzer->StopFFTAnalyze();
	if (pAnalyzer)
		delete pAnalyzer;
}

void RAnalyzerPainter::SetWindowHandle(HWND hwnd){
	hWnd = hwnd;
}

void RAnalyzerPainter::SetColor(COLORREF bkg, COLORREF fore1, COLORREF fore2){
	BackgroundColor = bkg;
	ForegroundColor1 = fore1;
	ForegroundColor2 = fore2;
}

void RAnalyzerPainter::SetBkColor(COLORREF bkg){
	BackgroundColor = bkg;
}

void RAnalyzerPainter::SetFgColor(COLORREF fore1, COLORREF fore2){
	ForegroundColor1 = fore1;
	ForegroundColor2 = fore2;
}

double RAnalyzerPainter::GetFPS(){
	return fps;
}