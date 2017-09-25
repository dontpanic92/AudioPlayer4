#pragma once
#include <Windows.h>

class RThread{
protected:
	HANDLE hThread;
	static DWORD WINAPI Thread(LPVOID lpThreadParameter){
		RThread* pThis = (RThread*)lpThreadParameter;
		pThis->Proc();
		return 1;
	}
	virtual void Proc() = 0;
public:
	RThread() : hThread(INVALID_HANDLE_VALUE){}
	void go(){
		Terminate();
		hThread = CreateThread(0, 0, Thread, this, 0, 0);
	}
	void Stop(){
		WaitForSingleObject(hThread, 1000);
		Terminate();
	}

	void Terminate(){
		if (hThread || hThread != INVALID_HANDLE_VALUE){
			TerminateThread(hThread, 0);
		}
		CloseHandle(hThread);		
	}
	~RThread(){
		Terminate();
	}
};