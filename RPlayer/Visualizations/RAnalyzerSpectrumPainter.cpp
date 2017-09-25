#include "RAnalyzerSpectrumPainter.h"

RAnalyzerSpectrumPainter::RAnalyzerSpectrumPainter(){
	m_nBars = 50;
	hMap = NULL;
	pAnalyzer->SetFFTSize(2048);
	left = pAnalyzer->GetLeftFFTSamplePointer();
	right = pAnalyzer->GetRightFFTSamplePointer();		
	nSample = pAnalyzer->GetFFTSampleSize();
}

RAnalyzerSpectrumPainter::RAnalyzerSpectrumPainter(HWND hwnd) : RAnalyzerPainter(hwnd){
	m_nBars = 50;
	hMap = NULL;
	pAnalyzer->SetFFTSize(2048);
	left = pAnalyzer->GetLeftFFTSamplePointer();
	right = pAnalyzer->GetRightFFTSamplePointer();
	nSample = pAnalyzer->GetFFTSampleSize();
}

RAnalyzerSpectrumPainter::RAnalyzerSpectrumPainter(HWND hwnd, COLORREF bkg, COLORREF fore1, COLORREF fore2, int nBars):RAnalyzerPainter(hwnd){
	SetColor(bkg, fore1, fore2);
	m_nBars = nBars;
	hMap = NULL;
	pAnalyzer->SetFFTSize(2048);
	left = pAnalyzer->GetLeftFFTSamplePointer();
	right = pAnalyzer->GetRightFFTSamplePointer();
	nSample = pAnalyzer->GetFFTSampleSize();
}

RAnalyzerSpectrumPainter::~RAnalyzerSpectrumPainter(){
	if (hMap!=NULL)
		::DeleteObject(hMap);
	dcMem.DeleteDC();
}

void RAnalyzerSpectrumPainter::SetParam(HWND hwnd, COLORREF bkg, COLORREF fore1, COLORREF fore2, int nBars){
	if (hMap!=NULL)
		::DeleteObject(hMap);
	dcMem.DeleteDC();

	hWnd = hwnd;
	SetColor(bkg, fore1, fore2);
	m_nBars = nBars;
}

void RAnalyzerSpectrumPainter::SetBars(int nBars){
	m_nBars = nBars;
}

void RAnalyzerSpectrumPainter::Paint(){
		if (hWnd == NULL){
			return;
		}

		HDC dc = GetDC(hWnd);;
		RECT rect;
		GetClientRect(hWnd, &rect);

		static int height = rect.bottom;
		static int width = rect.right;
		int nheight = rect.bottom;
		int nwidth = rect.right;

		if (nheight != height || nwidth != width){
			::DeleteObject(hMap);
		}
		if (hMap == NULL ||nheight != height || nwidth != width){
			hMap = ::CreateCompatibleBitmap( dc, nwidth, nheight);
			dcMem.DeleteDC();
			dcMem.CreateCompatibleDC(dc);
			::SelectObject(dcMem, hMap);
		}

		height = nheight;
		width = nwidth;
		dcMem.FillSolidRect(&rect, BackgroundColor);

		int BarWidth = width / m_nBars;
		int startx = (width - BarWidth * m_nBars) / 2;

		int pos = 1;
		int step = nSample / m_nBars;
		for (int i = 0; i < m_nBars; i++)
		{
			int tot=0;
			RECT rt;
			for (int j = pos; j < pos+step; j++)
			{
				tot = tot + (int)(left[j] + right[j]);
			}
			tot /= step * 2;
			pos += step;
			rt.left = startx + BarWidth * i + 1;
			if (tot > 0)
				tot = 0;
			if (tot < -110)
				tot = -110;
			double r =  (tot + 110) / 110.0;
			int t = ((height - 15) * r + 10);
			rt.top = height - t;
			rt.bottom = height - 10;
			rt.right = rt.left +  BarWidth - 1;
			BYTE rb = GetRValue(ForegroundColor1) * r;
			BYTE g = GetGValue(ForegroundColor1) * r;
			BYTE b = GetBValue(ForegroundColor1) * r;
			dcMem.GradientFillRect(rt, RGB(rb,g,b), ForegroundColor2, false);
			//dcMem.FillSolidRect(&rt, RGB(0,0,255));
		}

		::BitBlt(dc, 0, 0, width, height, dcMem ,0, 0, SRCCOPY);
		::DeleteDC(dc);
}