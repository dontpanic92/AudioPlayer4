#include "Decoder.h"
#include "3rdparty/All.h"
#include "3rdparty/MACLib.h"
#include <cstdio>
#pragma comment(lib,"lib/MAClib.lib")
#undef UNICODE
class ApeDecoder : public Decoder{
private:
	IAPEDecompress* pAPEDec;
public:
	ApeDecoder();
	~ApeDecoder();
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
