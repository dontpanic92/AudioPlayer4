#pragma once
#include "Decoder.h"
#include "3rdparty/vorbisfile.h"
#pragma comment(lib,"lib/libvorbisfile.lib")

class OggDecoder : public Decoder{
private:
	OggVorbis_File ov_f;
public:
	OggDecoder();
	~OggDecoder();
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
};