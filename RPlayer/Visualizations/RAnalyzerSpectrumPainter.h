#pragma once
#include "RAnalyzerPainter.h"
class RAnalyzerSpectrumPainter : public RAnalyzerPainter{
protected:
	int m_nBars;
	HBITMAP hMap;
	CDC dcMem;

	const double *left, *right;
	int nSample;
public:
	RAnalyzerSpectrumPainter();
	RAnalyzerSpectrumPainter(HWND hwnd);
	RAnalyzerSpectrumPainter(HWND hwnd, COLORREF bkg, COLORREF fore1, COLORREF fore2, int nBars);
	~RAnalyzerSpectrumPainter();

	void SetParam(HWND hwnd, COLORREF bkg, COLORREF fore1, COLORREF fore2, int nBars);
	void SetBars(int nBars);
	void Paint();
};