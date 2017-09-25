// RPlayer.cpp : main source file for RPlayer.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlcrack.h>
#include <atltheme.h>

#include "resource.h"
#include "aboutdlg.h"
#include "MainDlg.h"
#include "MainDlg2.h"
#include "MainFrm.h"

CAppModule _Module;

class RMessageLoop : public CMessageLoop{
public:
	MSG Run(){
		BOOL bDoIdle = TRUE;
		int nIdleCount = 0;
		BOOL bRet;

		for(;;)
		{
			while(bDoIdle && !::PeekMessage(&m_msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if(!OnIdle(nIdleCount++))
					bDoIdle = FALSE;
			}

			bRet = ::GetMessage(&m_msg, NULL, 0, 0);

			if(bRet == -1)
			{
				ATLTRACE2(atlTraceUI, 0, _T("::GetMessage returned -1 (error)\n"));
				continue;   // error, don't process
			}
			else if(!bRet)
			{
				ATLTRACE2(atlTraceUI, 0, _T("CMessageLoop::Run - exiting\n"));
				break;   // WM_QUIT, exit message loop

			}else if (m_msg.message == RM_CHANGE_UI){
				break;
			}

			if(!PreTranslateMessage(&m_msg))
			{
				::TranslateMessage(&m_msg);
				::DispatchMessage(&m_msg);
			}

			if(IsIdleMessage(&m_msg))
			{
				bDoIdle = TRUE;
				nIdleCount = 0;
			}
		}

		return m_msg;
	}
};

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	RMessageLoop* theLoop;
	CMainDlg* dlgMain;
	CMainDlg2* dlgMain2;
	CMainFrame* frmMain;

	AllocConsole();
	//freopen("CONOUT$", "w+t", stderr);
	freopen("CONOUT$", "w+t", stdout);

	MSG Ret;
	Ret.message = RM_CHANGE_UI;
	Ret.wParam = 1;
	for(;;){
		if (Ret.message == RM_CHANGE_UI){
			if (Ret.wParam == 1){
				dlgMain = new CMainDlg;
				theLoop = new RMessageLoop;
				_Module.AddMessageLoop(theLoop);
				if(dlgMain->Create(NULL) == NULL)
				{  
					ATLTRACE(_T("Main dialog creation failed!\n"));
					return 0;
				}
				dlgMain->ShowWindow(nCmdShow);
				Ret = theLoop->Run();
				_Module.RemoveMessageLoop();
				delete theLoop;
				delete dlgMain;
			}else if (Ret.wParam == 2){
				frmMain = new CMainFrame;
				theLoop = new RMessageLoop;
				_Module.AddMessageLoop(theLoop);
				if(frmMain->CreateEx() == NULL)
				{
					ATLTRACE(_T("Main Frame creation failed!\n"));
					return 0;
				}
				frmMain->ShowWindow(nCmdShow);
				Ret = theLoop->Run();
				frmMain->DestroyWindow();
				_Module.RemoveMessageLoop();
				delete theLoop;
				delete frmMain;
			}else{
				dlgMain2 = new CMainDlg2;
				theLoop = new RMessageLoop;
				_Module.AddMessageLoop(theLoop);
				if(dlgMain2->Create(NULL) == NULL)
				{
					ATLTRACE(_T("Main dialog creation failed!\n"));
					return 0;
				}
				dlgMain2->ShowWindow(nCmdShow);
				Ret = theLoop->Run();
				_Module.RemoveMessageLoop();
				delete theLoop;
				delete dlgMain2;			
			}
		}else{
			break;
		}
	}
	RPlayer::cleanInstance();
	RPlayList::cleanInstance();
	RPlayerBuffer::cleanInstance();

	FreeConsole();
	return Ret.wParam;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));
	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES | ICC_PROGRESS_CLASS | ICC_COOL_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);
	
	_Module.Term();

	::CoUninitialize();

	return nRet;
}
