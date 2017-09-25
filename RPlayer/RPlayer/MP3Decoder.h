
#pragma once
#include "Decoder.h"
#include <stdio.h>
typedef long ssize_t;
#include "3rdparty/mpg123.h"
#pragma comment(lib,"lib/libmpg123.lib")


class MP3Decoder : public Decoder{
private:
	mpg123_handle *mh;
public:
	MP3Decoder() : mh(NULL){mpg123_init();}
	int CheckSupport(const char* fname);
	int Open(const char* fname);
	int Seek(double sec);
	void GetFileExt(char* Description, char* FileExt);
	int GetFormat(long* rate,int* channel);
	int GetFileInfo(Info* info);
	int GetFileInfo(const char* filename, Info* info);
	int ReadAndDecode(unsigned char* buf,int size,unsigned int* read);
	double GetPosition(int buffer_size = 0);
	double GetLength();
	int Close();
	~MP3Decoder(){mpg123_exit();}
};