#pragma once
#include <windows.h>
#include "..\stdafx.h"
#include "E:\\SDKs\\SDL-devel-1.2.14-VC8\\SDL-1.2.14\\include\\SDL.h"
#include "RPlayerBuffer.h"
#include <queue>
#include "RDecoderManager.h"
#include "../3rdparty/SoundTouch/BPMDetect.h"
#pragma comment(lib,"lib/SDL.lib")
using namespace std;

#define BUF_SIZE 4096
enum _status{PLAY,PAUSE,STOP};
typedef void (*NotifyCallBack)(int, int, char*, void*);
#define NOTIFY_SONG_FINISHED	0

class RPlayer{
private:
	char filename[260];
	class MIDIPlayer{
	private:
	public:
		MIDIPlayer(){};
		~MIDIPlayer(){};
		int Play(const char* filename);
	}_MIDIPlayer;
	
	int status;
	double time;
	CRITICAL_SECTION cs;
	HANDLE hEvent;
	int volume;

	SDL_AudioSpec wanted_spec, spec;
    SDL_Thread* hPlayThread;
	unsigned char silence_buf[1024];
	bool loop;
	
	NotifyCallBack pNotify;
	void* NotifyParam;
	HWND hwnd;

	RDecoderManager decoder;

	int Open();
	void Close();
	void ClearCache();
	void ReadAndDecode();
	void FillBuffer(Uint8 *stream, int len);
	void NotifyUI(int msg, int param1 = 0, char* param2 = NULL);
	static int PlayThread(void* param);
	static void CallBack(void *opaque, Uint8 *stream, int len);

	static RPlayer* theSingleton;
	RPlayerBuffer* buffer;
protected:
	RPlayer();
	RPlayer(const RPlayer&){}; //防止拷贝构造另一个实例
    RPlayer &operator =(const RPlayer&){}; //防止赋值构造出另一个实例
public:
	virtual ~RPlayer();
	static RPlayer* getInstance();
	static void cleanInstance();

	int Play(char* filename);
	void Resume();
	void Pause();
	void Stop();
	void Seek(double sec);
	int GetStatus();
	RDecoderManager* GetDecoderManager();

	void SetVolume(int vol);
	int  GetVolume();

	double GetPosition();
	double GetDuration();
	void SetNotify(HWND hWnd);
	//void SetNotify(NotifyCallBack pFunc, void* param);
};