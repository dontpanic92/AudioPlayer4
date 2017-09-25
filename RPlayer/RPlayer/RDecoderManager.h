#pragma once
#include "MP3Decoder.h"
#include "OggDecoder.h"
#include "ApeDecoder.h"
#include "WAVDecoder.h"
#include "RPlayerBuffer.h"

class RDecoderManager{
	char filename[260];
	Decoder* decoders[20];
	int DecoderNum;
	int CurrentDecoder;
	void CreateAllDecoders();
	void DeleteAllDecoders();
	void FindDecoder();

	RPlayerBuffer* pBuffer;
public:
	RDecoderManager();
	virtual ~RDecoderManager();

	int Open(const char* fname);
	void Close();
	int ReadAndDecode(unsigned char* buffer, unsigned int size, unsigned int* read);
	void Seek(double sec);
	double GetPosition(int buffered = 0);
	double GetDuration();
	void GetSupportFileType(char* buffer);
	bool GetInfo(Info* info);
	bool GetFileInfo(char* filename, Info* info);
};