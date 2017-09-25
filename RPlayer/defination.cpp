#include "StdAfx.h"
#include "MainDlg.h"

bool mbs2wcs(LPCSTR lpcszStr, LPWSTR lpwszStr, DWORD dwSize){
	DWORD dwMinSize;
	dwMinSize = MultiByteToWideChar (CP_ACP, 0, lpcszStr, -1, NULL, 0);
	if(dwSize < dwMinSize)
		return false;
	MultiByteToWideChar(CP_ACP, 0, lpcszStr, -1, lpwszStr, dwMinSize);
	return true;
}
bool wcs2mbs(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize){
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,NULL,0,NULL,FALSE);
	if(dwSize < dwMinSize)
		return false;
	WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,lpszStr,dwSize,NULL,FALSE);
	return true;
}

const btn btnID[] = {
						{"btnPlay", 1001}, 
						{"btnPause", 1002},
						{"btnStop", 1003},
						{"btnPrev", 1004},
						{"btnNext", 1005},
						{"btnMinimize", 1006},
						{"btnMinimode", 1007},
						{"btnClose", 1008},
					 };
int findBtnId(const char* name){
	for (int i = 0; i < sizeof(btnID); i++){
		if (!strcmp(btnID[i].name, name)){
			return btnID[i].id;
		}
	}
	return -1;
}