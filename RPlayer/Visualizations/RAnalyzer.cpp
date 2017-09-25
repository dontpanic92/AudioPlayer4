#include "RAnalyzer.h"
#include "Fourier.h"
#include <math.h>
inline double GetFrequencyIntensity(double re, double im)
{
	return sqrt((re*re)+(im*im));
}
#define mag_sqrd(re,im) (re*re+im*im)
#define Decibels(re,im) ((re == 0 && im == 0) ? (0) : 10.0 * log10(double(mag_sqrd(re,im))))
#define Amplitude(re,im,len) (GetFrequencyIntensity(re,im)/(len))
#define AmplitudeScaled(re,im,len,scale) ((int)Amplitude(re,im,len)%scale)


RAnalyzer::RAnalyzer() : channel(2), fftneeds(0), fft_size(2048){
	InitializeCriticalSection(&cs);
	InitializeCriticalSection(&cs2);
}

RAnalyzer::~RAnalyzer(){
	Terminate();
}

void RAnalyzer::SetFFTSize(const int size){
	if (size > sizeof(sample) / 2)
		return;
	fft_size = size;
}

void RAnalyzer::StartFFTAnalyze(){
	EnterCriticalSection(&cs);
	if (fftneeds == 0){
		go();
	}
	fftneeds++;
	LeaveCriticalSection(&cs);
}

void RAnalyzer::StopFFTAnalyze(){
	EnterCriticalSection(&cs);
	fftneeds--;
	if (fftneeds == 0){
		Terminate();
	}
	LeaveCriticalSection(&cs);
}

unsigned int RAnalyzer::GetMaxSize(){
	return sizeof(sample);
}

void RAnalyzer::PutSample(unsigned char* buffer,const unsigned int size){
	sample_size = size > sizeof(sample) ? sizeof(sample) : size;
	memcpy(sample, buffer, sample_size);
}

unsigned int RAnalyzer::GetSample(unsigned char* buffer,const unsigned int size){
	unsigned int tmp = size > sample_size ? sample_size : size;
	memcpy(buffer, sample, tmp);
	return tmp;
}

unsigned int RAnalyzer::GetLeftSample(unsigned char* buffer,const unsigned int size){
	if (channel == 2){
		unsigned int tmp = size > sample_size / 2 ? sample_size / 2 : size;
		for(unsigned int i = 0; i < tmp; i++){
			buffer[i] = sample[i * 2];
		}
		return tmp;
	}else{
		return GetSample(buffer, size);
	}
}

unsigned int RAnalyzer::GetRightSample(unsigned char* buffer,const unsigned int size){
	if (channel == 2){
		unsigned int tmp = size > sample_size / 2 ? sample_size / 2 : size;
		for(unsigned int i = 0; i < tmp && i * 2 + 1 < sample_size; i++){
			buffer[i] = sample[i * 2 + 1];
		}
		return tmp;
	}else{
		return GetSample(buffer, size);
	}
}

unsigned int RAnalyzer::GetFFTSample(double* buffer,const unsigned int size){
	if (fftneeds == 0)
		return 0;
	unsigned int tmp = size > fft_size / 2 ? fft_size / 2 : size;
	memcpy(buffer, fft_sample, tmp);
	return tmp;
}

unsigned int RAnalyzer::GetLeftFFTSample(double* buffer,const unsigned int size){
	if (channel == 2 && fftneeds > 0){
		unsigned int tmp = size > fft_size / 4 ? fft_size / 4 : size;
		memcpy(buffer, left_fft_sample, tmp);
		return tmp;
	}else{
		return GetFFTSample(buffer, size);
	}
}

unsigned int RAnalyzer::GetRightFFTSample(double* buffer,const unsigned int size){
	if (channel == 2 && fftneeds > 0){
		unsigned int tmp = size > fft_size / 4 ? fft_size / 4 : size;
		memcpy(buffer, right_fft_sample, tmp);
		return tmp;
	}else{
		return GetFFTSample(buffer, size);
	}
}

unsigned int RAnalyzer::GetFFTSampleSize(){
	return fft_size / 4;
}
const double* RAnalyzer::GetLeftFFTSamplePointer(){
	return left_fft_sample;
}
const double* RAnalyzer::GetRightFFTSamplePointer(){
	return right_fft_sample;
}

#include <vector>
using namespace std;
void RAnalyzer::Proc(){
#define FFT_LEN 2048
	while(true){
		RPlayerBuffer::getInstance()->GetSample(sample, 4096);
	double finleft[FFT_LEN/2],finright[FFT_LEN/2],fout[FFT_LEN],foutimg[FFT_LEN];//fdraw[FFT_LEN/2];
	DWORD nCount = 0;
	for (DWORD dw = 0; dw < FFT_LEN; dw++)
	{
		{
			//copy audio signal to fft real component for left channel
			finleft[nCount] = (double)((short*)sample)[dw++] / 32767.0;
			//copy audio signal to fft real component for right channel
			finright[nCount++] = (double)((short*)sample)[dw] / 32767.0;
		}
	}
	fft_double(FFT_LEN/2,0,finleft,NULL,fout,foutimg);
	float re,im;
	left_fft_sample[0] = left_fft_sample[FFT_LEN/4] = 0;
	for(int i=1;i < FFT_LEN/4;i++)//Use FFT_LEN/4 since the data is mirrored within the array.
	{
		re = (float)fout[i];
		im = (float)foutimg[i];
		double t = Amplitude(re, im, FFT_LEN / 2);
		left_fft_sample[i] = (t == 0) ? -110 : 20 * log10(t);
	}

	Sleep(30);

	fft_double(FFT_LEN/2,0,finright,NULL,fout,foutimg);
	right_fft_sample[0] = right_fft_sample[FFT_LEN/4] = 0;
	for(int i=1;i < FFT_LEN/4;i++)//Use FFT_LEN/4 since the data is mirrored within the array.
	{
		re = (float)fout[i];
		im = (float)foutimg[i];
		double t = Amplitude(re, im, FFT_LEN / 2);
		right_fft_sample[i] = (t == 0) ? -110 : 20 * log10(t);
	}
	Sleep(40);
	}
}