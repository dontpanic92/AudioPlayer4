#pragma once
#include <windows.h>

enum DECODER_ERR{
	DECODER_OK,
	DECODER_DONE,
	ERR_OPEN_ERROR,
	ERR_INIT_ERROR,
	ERR_NOT_SUPPORT,
	ERR_GENERIC
};
struct Info{
	long rate;
	long bitrate;
	int channel;
	double duration;
	char codec[30];
};
class Decoder{
protected:
	long rate;
	long bitrate;
	int channel;
	double duration;
	char CurrentCodec[30];
public:
	Decoder() : rate(0), channel(0){}
	virtual int CheckSupport(const char*) = 0;
	virtual int Open(const char*) = 0;
	virtual int Seek(double) = 0;
	virtual void GetFileExt(char* ,char*) = 0;
	virtual int GetFormat(long*,int*) = 0;
	virtual int GetFileInfo(Info*) = 0;
	virtual int GetFileInfo(const char*, Info*) = 0;
	virtual int ReadAndDecode(unsigned char*,int,unsigned int*) = 0;
	virtual double GetPosition(int buffered = 0) = 0;
	virtual double GetLength() = 0;
	virtual int Close() = 0;
	virtual ~Decoder(){}
};