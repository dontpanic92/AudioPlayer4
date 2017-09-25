#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <Shlwapi.h>
using namespace std;

enum PlayOrder{PO_ORDER,PO_RANDOM};
struct myWString{
	DWORD len;		  //Should be in byte
	wstring str;
};

struct PLHeader{
	char magic[4];
	DWORD N;
	DWORD listNameLen; //Should be in byte
	wstring listName;
};

enum strs{PATH, TITLE, ARTIST, ALBUM, CODEC};
enum dws{FROM, TO, DURATION, BPM, BITRATE, RATE};

struct SongInfo{
	myWString strings[5];
	DWORD dwords[6];
};

struct PlayList{
	PLHeader header;
	vector<SongInfo> list;
};

class RPlayList{
	PlayOrder order;
	vector<PlayList> PlayLists;
	int CurrentList;
	int CurrentIndex;

	void LoadPlayList();

	static RPlayList* theSingleton;
protected:
	RPlayList();
	RPlayList(const RPlayList&){}; //防止拷贝构造另一个实例
    RPlayList &operator =(const RPlayList&){}; //防止赋值构造出另一个实例
public:
	static RPlayList* getInstance();
	static void cleanInstance();
	void AddPlayList(PlayList& pl);
	void AddSong(SongInfo& info, const unsigned int list);
	void LoadPlayLists();
	void SavePlayLists();
	void SetOrder(PlayOrder order);
	void SetCurrent(int list, int index);
	PlayList& GetPlayList(const unsigned int list);
	wstring GetNextFileName();
	wstring GetFileName(const unsigned int list, const unsigned int index);
};