// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

// Change these values to use different versions
#undef WINVER
#undef _WIN32_WINNT
#undef _WIN32_IE

#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#define _WIN32_IE 0x0700
#define _RICHEDIT_VER	0x0200

#include "resource.h"
#include "resource1.h"
#include <atlbase.h>
#include <atlapp.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atlframe.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlcrack.h>
#include <atltheme.h>
extern CAppModule _Module;

bool mbs2wcs(LPCSTR lpcszStr, LPWSTR lpwszStr, DWORD dwSize);
bool wcs2mbs(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize);

#define RM_CHANGE_UI	WM_USER + 10001
#define RM_PLAY			WM_USER + 10002
#define RM_ADD_SONG		WM_USER + 10003
#define RM_CHANGE_SONG	WM_USER + 10004
#define RM_REMOVE		WM_USER + 10005
#define RM_SETSTATUS	WM_USER + 10006

#include <atlwin.h>

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
