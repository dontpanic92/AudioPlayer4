#include "PlayList.h"
RPlayList::RPlayList() : CurrentList(-1), CurrentIndex(-1), order(PO_ORDER){
	PlayList pl;
	pl.header.listName = L"¡Ÿ ±¡–±Ì";
	PlayLists.push_back(pl);
}

RPlayList* RPlayList::theSingleton = NULL;

RPlayList* RPlayList::getInstance(){
	if (theSingleton == NULL){
		theSingleton = new RPlayList();
	}
	return theSingleton;
}

void RPlayList::cleanInstance(){
	if (theSingleton != NULL){
		delete theSingleton;
	}
}

void RPlayList::AddSong(SongInfo& info, const unsigned int list){
	if (list < 0 || list >= PlayLists.size())
		return;
	PlayLists[list].list.push_back(info);
	PlayLists[list].header.N = PlayLists[0].list.size();
}

void RPlayList::AddPlayList(PlayList& pl){
	PlayLists.push_back(pl);
}
PlayList& RPlayList::GetPlayList(const unsigned int list){
	if (list < 0 || list >= PlayLists.size()){
		return PlayLists[0];
	}
	return PlayLists[list];
}
void RPlayList::LoadPlayList(){
	HANDLE  hf = CreateFile(L"..\\PlayList\\list0.rpl", GENERIC_READ,
							0, (LPSECURITY_ATTRIBUTES) NULL,
							OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
							(HANDLE) NULL);
	if (hf == INVALID_HANDLE_VALUE){
		return;
	}
	wchar_t* buffer = NULL;

	PlayLists.clear();
	PlayList playlist;
	DWORD dwSize = 0;
	if (!ReadFile(hf, playlist.header.magic, 4, &dwSize, NULL) || dwSize == 0 || strcmp(playlist.header.magic, "RPL"))
		goto END;
	if (!ReadFile(hf, &playlist.header.N, sizeof(playlist.header.N), &dwSize, NULL) || dwSize == 0)
		goto END;
	if (!ReadFile(hf, &playlist.header.listNameLen, sizeof(playlist.header.listNameLen), &dwSize, NULL) || dwSize == 0)
		goto END;

	buffer = new wchar_t[playlist.header.listNameLen];
	if (!ReadFile(hf, buffer, playlist.header.listNameLen, &dwSize, NULL) || dwSize == 0)
		goto END;
	buffer[playlist.header.listNameLen - 1] = '\0';
	playlist.header.listName = buffer;
	delete[] buffer;
	buffer = NULL;
	

	for (int i = 0; i < playlist.header.N; i++){
		SongInfo info;

		for (int j = 0; j < 5; j++){
			if (!ReadFile(hf, &info.strings[j].len, sizeof(info.strings[j].len), &dwSize, NULL) || dwSize == 0)
				goto END;
			buffer = new wchar_t[info.strings[j].len];
			if (!ReadFile(hf, buffer, info.strings[j].len, &dwSize, NULL) || dwSize == 0)
				goto END;
			buffer[info.strings[j].len - 1] = '\0';
			info.strings[j].str = buffer;
			delete[] buffer;
			buffer = NULL;
		}

		for (int k = 0; k < 6; k++){
			if (!ReadFile(hf, &info.dwords[k], sizeof(info.dwords[k]), &dwSize, NULL) || dwSize == 0)
				goto END;
		}

		playlist.list.push_back(info);
	}
	PlayLists.push_back(playlist);
END:
	if (buffer != NULL)
		delete[] buffer;
	CloseHandle(hf);
}

void RPlayList::LoadPlayLists(){
	LoadPlayList();
	if (PlayLists.size() == 0){
		PlayList pl;
		PlayLists.push_back(pl);
	}
	CurrentList = 0;
	CurrentIndex = 0;
}

void RPlayList::SavePlayLists(){
	PlayLists[0].header.listNameLen = (PlayLists[0].header.listName.size() + 1) * 2;
	PlayLists[0].header.N = PlayLists[0].list.size();

	if (!PathFileExists(L"..\\PlayList\\")){
		CreateDirectory(L"..\\PlayList\\", NULL);
	}

	HANDLE  hf = CreateFile(L"..\\PlayList\\list0.rpl", GENERIC_WRITE,
							0, (LPSECURITY_ATTRIBUTES) NULL,
							OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
							(HANDLE) NULL);
	if (hf == INVALID_HANDLE_VALUE){
		return;
	}
	DWORD dwSize = 0;
	char magic[4] = "RPL";
	if (!WriteFile(hf, magic, 4, &dwSize, NULL))
		goto END;
	if (!WriteFile(hf, &PlayLists[0].header.N, sizeof(PlayLists[0].header.N), &dwSize, NULL))
		goto END;
	if (!WriteFile(hf, &PlayLists[0].header.listNameLen, sizeof(PlayLists[0].header.listNameLen), &dwSize, NULL))
		goto END;
	if (!WriteFile(hf, PlayLists[0].header.listName.c_str(), PlayLists[0].header.listNameLen, &dwSize, NULL))
		goto END;

	for (int i = 0; i < PlayLists[0].header.N; i++){

		for (int j = 0; j < 5; j++){
			DWORD len = (PlayLists[0].list[i].strings[j].str.size() + 1) * 2;
			if (!WriteFile(hf, &len, sizeof(len), &dwSize, NULL))
				goto END;

			if (!WriteFile(hf, PlayLists[0].list[i].strings[j].str.c_str(), len, &dwSize, NULL))
				goto END;
		}

		for (int k = 0; k < 6; k++){
			if (!WriteFile(hf, &PlayLists[0].list[i].dwords[k], sizeof(&PlayLists[0].list[i].dwords[k]), &dwSize, NULL))
				goto END;
		}
	}

END:
	CloseHandle(hf);
}

void RPlayList::SetOrder(PlayOrder order){
	this->order = order;
}

void RPlayList::SetCurrent(int list, int index){
	CurrentList = list;
	CurrentIndex = index;
	if (CurrentIndex >= PlayLists[CurrentList].list.size() || CurrentIndex < 0)
		CurrentIndex = 0;
	if (list >= PlayLists.size() || list < 0)
		CurrentList = 0;
}

wstring RPlayList::GetNextFileName(){
	srand(GetTickCount());
	switch(order){
	case PO_ORDER:
		CurrentIndex++;
		if (CurrentIndex >= PlayLists[CurrentList].list.size())
			CurrentIndex = 0;
		break;
	case PO_RANDOM:
		CurrentIndex = rand() % PlayLists[CurrentList].list.size();
		break;
	}
	if (CurrentIndex >= PlayLists[CurrentList].list.size() || CurrentIndex < 0)
		return L"";
	return PlayLists[CurrentList].list[CurrentIndex].strings[PATH].str;
}

wstring RPlayList::GetFileName(const unsigned int list,const unsigned int index){
	if (index >= PlayLists[CurrentList].list.size() || index < 0 || list >= PlayLists.size() || list < 0)
		return L"";
	return PlayLists[list].list[index].strings[PATH].str;
}