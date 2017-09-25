#include "RPlayerBuffer.h"

RPlayerBuffer* RPlayerBuffer::theSingleton = NULL;

RPlayerBuffer* RPlayerBuffer::getInstance(){
	if (theSingleton == NULL){
		theSingleton = new RPlayerBuffer();
	}
	return theSingleton;
}

void RPlayerBuffer::cleanInstance(){
	if (theSingleton != NULL){
		delete theSingleton;
	}
}

RPlayerBuffer::RPlayerBuffer(){
	pBuffer = new unsigned char[MAX_BUF_SIZE];
	pivotPut = pivotGet = pBuffer;
	ActualSize = 0;
	InitializeCriticalSection(&cs);
}

RPlayerBuffer::~RPlayerBuffer(){
	delete[] pBuffer;
}

unsigned int RPlayerBuffer::GetActualSize(){
	return ActualSize;
}

unsigned int RPlayerBuffer::GetSpareSize(){
	return MAX_BUF_SIZE - ActualSize - 16;
}

unsigned int RPlayerBuffer::GetMaxSize(){
	return MAX_BUF_SIZE - 16;
}

unsigned int RPlayerBuffer::PutBuffer(unsigned char* buffer, const unsigned int size){
	EnterCriticalSection(&cs);
	int ret = (MAX_BUF_SIZE - ActualSize > size) ? size : (MAX_BUF_SIZE - ActualSize);
	if ((pBuffer + MAX_BUF_SIZE) - pivotPut >= ret){
		memcpy(pivotPut, buffer, ret);
		pivotPut += ret;
	}else{
		unsigned int tmp  = (pBuffer + MAX_BUF_SIZE) - pivotPut;
		memcpy(pivotPut, buffer, tmp);
		unsigned int tmp2 = ret - tmp;
		memcpy(pBuffer, buffer + tmp, tmp2);
		pivotPut = pBuffer + tmp2;
	}
	
	ActualSize += ret;
	LeaveCriticalSection(&cs);
	return ret;
}

unsigned int RPlayerBuffer::GetBuffer(unsigned char* buffer, const unsigned int size){
	EnterCriticalSection(&cs);
	unsigned int ret = (ActualSize > size) ? size : ActualSize;
	if ((pBuffer + MAX_BUF_SIZE) - pivotGet >= ret){
		memcpy(buffer, pivotGet, ret);
		pivotGet += ret;
	}else{
		unsigned int tmp = (pBuffer + MAX_BUF_SIZE) - pivotGet;
		memcpy(buffer, pivotGet, tmp);
		unsigned int tmp2 = ret - tmp;
		memcpy(buffer + tmp, pBuffer, tmp2);
		pivotGet = pBuffer + tmp2;
	}

	ActualSize -= ret;
	LeaveCriticalSection(&cs);
	//unsigned int sample_size = RAnalyzer::getInstance()->GetMaxSize() > ret ? ret : RAnalyzer::getInstance()->GetMaxSize();
	//RAnalyzer::getInstance()->PutSample(buffer, sample_size);
	return ret;
}

void RPlayerBuffer::Clear(){
	EnterCriticalSection(&cs);
	pivotPut = pivotGet = pBuffer;
	ActualSize = 0;
	LeaveCriticalSection(&cs);
}

unsigned int RPlayerBuffer::GetSample(unsigned char* buffer, const unsigned int size){
	unsigned int ret = (ActualSize > size) ? size : ActualSize;
	if ((pBuffer + MAX_BUF_SIZE) - pivotGet >= ret){
		memcpy(buffer, pivotGet, ret);
	}else{
		unsigned int tmp = (pBuffer + MAX_BUF_SIZE) - pivotGet;
		memcpy(buffer, pivotGet, tmp);
		unsigned int tmp2 = ret - tmp;
		memcpy(buffer + tmp, pBuffer, tmp2);
	}
	return ret;
}