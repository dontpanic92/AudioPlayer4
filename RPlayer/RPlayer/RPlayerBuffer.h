#pragma once
#include <windows.h>
#include "../Visualizations/RAnalyzer.h"
const unsigned int MAX_BUF_SIZE = 320 * 1024;

class RPlayerBuffer{

	static RPlayerBuffer* theSingleton;
	
	unsigned char *pBuffer;
	unsigned char *pivotPut;
	unsigned char *pivotGet;
	unsigned int ActualSize;

	CRITICAL_SECTION cs;
protected:
	RPlayerBuffer();
	RPlayerBuffer(const RPlayerBuffer&){}; //防止拷贝构造另一个实例
    RPlayerBuffer &operator =(const RPlayerBuffer&){}; //防止赋值构造出另一个实例
public:
	virtual ~RPlayerBuffer();
	static RPlayerBuffer* getInstance();
	static void cleanInstance();

	unsigned int GetActualSize();
	unsigned int GetSpareSize();
	unsigned int GetMaxSize();
	unsigned int PutBuffer(unsigned char* buffer, const unsigned int size);
	unsigned int GetBuffer(unsigned char* buffer, const unsigned int size);
	void Clear();

	unsigned int GetSample(unsigned char* buffer, const unsigned int size);
};