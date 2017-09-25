#include "WAVDecoder.h"
#include <stdio.h>
WAVDecoder::WAVDecoder():WavFile(NULL),totalbytes(0),now(0){}
WAVDecoder::~WAVDecoder(){
	Close();
}
int WAVDecoder::CheckSupport(const char* fname){
	char filename[260];
	strcpy_s(filename, fname);
	HMMIO hmmio;
	hmmio = mmioOpenA(filename,NULL,MMIO_READWRITE);
	if (hmmio == NULL){
		return ERR_OPEN_ERROR;
	}
	char wave[4];
	mmioSeek(hmmio, 8, SEEK_SET);
	mmioRead(hmmio, wave, 4);
	if(wave[0]!='W'||wave[1]!='A'||wave[2]!='V'||wave[3]!='E'){
		mmioClose(hmmio, MMIO_FHOPEN);
		return ERR_NOT_SUPPORT;
	}
	mmioClose(hmmio, MMIO_FHOPEN);
	return DECODER_OK;
}
int WAVDecoder::Open(const char* fname){
	Close();
	char filename[260];
	strcpy_s(filename, fname);
	WavFile = mmioOpenA(filename,NULL,MMIO_READWRITE);
	if (WavFile == NULL){
		return ERR_OPEN_ERROR;
	}
	char wave[4];
	mmioSeek(WavFile, 8, SEEK_SET);
	mmioRead(WavFile, wave, 4);
	if(wave[0]!='W'||wave[1]!='A'||wave[2]!='V'||wave[3]!='E'){
		Close();
		return ERR_NOT_SUPPORT;
	}
	PCMWAVEFORMAT format;
	mmioSeek(WavFile,20,SEEK_SET);
	mmioRead(WavFile,(char*)&format,sizeof(PCMWAVEFORMAT));
	mmioSeek(WavFile,24+sizeof(PCMWAVEFORMAT),SEEK_SET); 
	mmioRead(WavFile,(char*)&totalbytes,4);
	channel = format.wf.nChannels;
	rate = format.wf.nSamplesPerSec;
	bitrate = 8 * format.wf.nAvgBytesPerSec;
	duration = double(totalbytes) / rate;
	blockalign = format.wf.nBlockAlign;
	strcpy_s(CurrentCodec, "WAV");
	printf("channel = %d, rate = %d, bitrate = %d, totalbytes = %d\n", channel, rate, bitrate, totalbytes);
	return DECODER_OK;
}
int WAVDecoder::Seek(double sec){
	if (sec > duration)
		sec = duration;
	if (sec < 0)
		sec = 0;

	unsigned int tmp = (unsigned int)(sec / duration * totalbytes);
	now = tmp / blockalign * blockalign;
	return DECODER_OK;
}
void WAVDecoder::GetFileExt(char* Description, char* FileExt){
	char des[] = "WAV File(*.wav)";
	char ext[] = "*.wav";
	strcpy_s(Description, sizeof(des), des);
	strcpy_s(FileExt, sizeof(ext), ext);
}
int WAVDecoder::GetFormat(long* rate,int* channel){
	*rate = this->rate;
	*channel = this->channel;
	return DECODER_OK;
}
int WAVDecoder::GetFileInfo(Info* info){
	info->rate = rate;
	info->bitrate = bitrate;
	info->channel = channel;
	info->duration = duration;
	strcpy_s(info->codec, CurrentCodec);
	return DECODER_OK;
}
int WAVDecoder::GetFileInfo(const char* filename, Info* info){
	char file[260];
	strcpy_s(file, filename);
	HMMIO hmmio;
	hmmio = mmioOpenA(file,NULL,MMIO_READWRITE);
	if (hmmio == NULL){
		return ERR_OPEN_ERROR;
	}
	char wave[4];
	mmioSeek(hmmio, 8, SEEK_SET);
	mmioRead(hmmio, wave, 4);
	if(wave[0]!='W'||wave[1]!='A'||wave[2]!='V'||wave[3]!='E'){
		mmioClose(hmmio, MMIO_FHOPEN);
		return ERR_NOT_SUPPORT;
	}
	PCMWAVEFORMAT format;
	unsigned int total;
	mmioSeek(hmmio,20,SEEK_SET);
	mmioRead(hmmio,(char*)&format,sizeof(PCMWAVEFORMAT));
	mmioSeek(hmmio,42,SEEK_SET); 
	mmioRead(hmmio,(char*)&total,4);
	info->channel = format.wf.nChannels;
	info->rate = format.wf.nSamplesPerSec;
	info->bitrate = 8 * format.wf.nAvgBytesPerSec;;
	info->duration = double(total) / info->rate;
	strcpy_s(info->codec, "WAV");
	mmioClose(hmmio, MMIO_FHOPEN);
	return DECODER_OK;
}
int WAVDecoder::ReadAndDecode(unsigned char* buf,int size,unsigned int* read){
	mmioSeek(WavFile,28 + sizeof(PCMWAVEFORMAT) + now,SEEK_SET);
	int r;
	if (now + size > totalbytes){
		r = totalbytes - now;
		*read = r;
		mmioRead(WavFile,(char*)buf,r);
		now += r;
		return DECODER_DONE;
	}else{
		r = size;
		mmioRead(WavFile,(char*)buf,r);
		*read = r;
		now += r;
		return DECODER_OK;
	}
}
double WAVDecoder::GetPosition(int buffer_size){
	return double(now - buffer_size) / totalbytes * duration;
}
double WAVDecoder::GetLength(){
	return duration;
}
int WAVDecoder::Close(){
	if (WavFile){
		mmioClose(WavFile, MMIO_FHOPEN);
		WavFile = NULL;
	}
	totalbytes = 0;
	now = 0;
	return DECODER_OK;
}