#include "Decoder.h"
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

class WAVDecoder : public Decoder{
private:
	HMMIO WavFile;
	unsigned int totalbytes;
	unsigned int now;
	unsigned int blockalign;
public:
	WAVDecoder();
	~WAVDecoder();
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