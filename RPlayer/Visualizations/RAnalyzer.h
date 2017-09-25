#pragma once
#include "..\RPlayer\RPlayerBuffer.h"
#include "..\RThread.h"
#include <Windows.h>

class RAnalyzer : public RThread{

	unsigned char sample[4096];
	unsigned int sample_size;
	
	unsigned char left_sample[2048];
	unsigned char right_sample[2048];

	/*float fft_raw_sample[4096];
	float left_fft_raw_sample[2048];
	float right_fft_raw_sample[2048];
	*/
	double fft_sample[4096];
	double left_fft_sample[2048];
	double right_fft_sample[2048];

	int channel;

	int fftneeds;
	void Proc();

	CRITICAL_SECTION cs;
	CRITICAL_SECTION cs2;
	int fft_size;

public:
	RAnalyzer();
	virtual ~RAnalyzer();

	void SetFFTSize(const int size);
	void StartFFTAnalyze();
	void StopFFTAnalyze();
	unsigned int GetMaxSize();
	void PutSample(unsigned char* buffer,const unsigned int size);
	unsigned int GetSample(unsigned char* buffer,const unsigned int size);
	unsigned int GetLeftSample(unsigned char* buffer,const unsigned int size);
	unsigned int GetRightSample(unsigned char* buffer,const unsigned int size);
	unsigned int GetFFTSample(double* buffer,const unsigned int size);
	unsigned int GetLeftFFTSample(double* buffer,const unsigned int size);
	unsigned int GetRightFFTSample(double* buffer,const unsigned int size);
	unsigned int GetFFTSampleSize();
	const double* GetLeftFFTSamplePointer();
	const double* GetRightFFTSamplePointer();
};