#pragma once
#include "../stdafx.h"
#include "../RPlayer/RDecoderManager.h"
#include "../RThread.h"
#include "../3rdparty/SoundTouch/BPMDetect.h"
#pragma comment(lib, "lib/soundtouch.lib")
class RBpmDetectDlg : public CDialogImpl<RBpmDetectDlg>, public CUpdateUI<RBpmDetectDlg>,
		public CMessageFilter, public CIdleHandler,public RThread
{
private:
	char filename[260];
	CProgressBarCtrl progress1;
	HANDLE hThread;
	bool abort;
	RDecoderManager decoder;
	void Proc(){
		if (decoder.Open(filename) == -1){
			SetDlgItemText(IDC_STATIC_PROGRESS, L"文件不存在或未知编码");
			return;
		}

		Info info;
		decoder.GetInfo(&info);
		double duration = decoder.GetDuration();
		soundtouch::BPMDetect detector(info.channel, info.rate);
		soundtouch::SAMPLETYPE sampleBuffer[20480];
		unsigned char buffer[40960];
		progress1.SetRange(0, 100);
		abort = false;
		while(!abort){
			double pos = decoder.GetPosition();
			int t = pos / duration * 100;
			if (int(t) % 5 == 0){
				wchar_t pro[260];
				wsprintf(pro, L"正在解码…… %d%%", t);
				SetDlgItemText(IDC_STATIC_PROGRESS, pro);
				progress1.SetPos(t);
			}

			unsigned int size;
			int mc = decoder.ReadAndDecode(buffer, sizeof(buffer), &size);		
			if (mc != DECODER_OK){
				if (mc == DECODER_DONE){
					break;
				}else if (mc == ERR_GENERIC){
					SetDlgItemText(IDC_STATIC_PROGRESS, L"解码出错");
					return;
				}
			}
			fprintf(stderr, "%f / %f\r", pos, duration);
			for (int i = 0; i < size / 2; i++){
				sampleBuffer[i] = *(short*)&buffer[i * 2];
			}
			detector.inputSamples(sampleBuffer, size / 2 / info.channel);
		}
		decoder.Close();
		SetDlgItemText(IDC_STATIC_PROGRESS, L"正在检测……");
		float result = detector.getBpm();
		if (result < 1e-6){
			MessageBox(L"检测失败:(");
			SetDlgItemText(IDC_STATIC_PROGRESS, L"无法测出BPM值。");
		}else{
			wchar_t r[200];
			swprintf(r, L"BPM: %f 请酌情按倍数扩大或缩小。", result);
			SetDlgItemText(IDC_STATIC_PROGRESS, r);
		}
		progress1.SetPos(100);
		SetDlgItemText(IDCANCEL, L"完成");
	}

public:
	enum { IDD = IDD_DLGBPMDETECT };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(RBpmDetectDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(RBpmDetectDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		CenterWindow();

		// set icons
		HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		SetIcon(hIconSmall, FALSE);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		UIAddChildWindowContainer(m_hWnd);
		progress1.Attach(GetDlgItem(IDC_PROGRESS1));

		return TRUE;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

		return 0;
	}

	void DetectBpm(const char* filename){
		memcpy(this->filename, filename, sizeof(this->filename));
		Create(NULL);
		SetDlgItemTextA(*this, IDC_STATIC_FILENAME, filename);
		ShowWindow(SW_SHOW);
	}

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: Add validation code 
		CButton ok(GetDlgItem(IDOK));
		ok.EnableWindow(FALSE);
		go();
		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CloseDialog(wID);
		return 0;
	}

	void CloseDialog(int nVal)
	{
		abort = true;
		Terminate();
		EndDialog(1);
	}
};
