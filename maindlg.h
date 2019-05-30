// maindlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINDLG_H__672A556B_A35E_4C70_B267_C805252131AA__INCLUDED_)
#define AFX_MAINDLG_H__672A556B_A35E_4C70_B267_C805252131AA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "inc/HttpOpt.h"
#include "inc/HttpsOpt.h" 
#include "inc/decodeurl.h"
#include <string>
using namespace std;
#include "NotepadPlusplus.h"

#define BUFFER_LEN_2MB (2*1024*1024)
#define	IDC_NOTEPAD		900

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler
{
public:
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_HANDLER(IDC_BTN_GETHEAD, BN_CLICKED, OnClickedBtn_gethead)
		NOTIFY_HANDLER(IDC_TAB1, TCN_SELCHANGE, OnSelchangeTab1)
		COMMAND_HANDLER(IDC_PROTOCOL, CBN_SELCHANGE, OnSelchangeProtocol)
		NOTIFY_ID_HANDLER(IDC_NOTEPAD, OnUpdataUI)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void CloseDialog(int nVal);
	LRESULT OnClickedBtn_gethead(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSelchangeTab1(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnSelchangeProtocol(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnUpdataUI(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	void GetHttp(int type, const char* strHeader);
	string getString(int id);
	void setText(HWND page, int ctrlid, const char* str);
	string getString(HWND page, int id);
	int getInt(HWND page, int id);
	BOOL getCheck(HWND page, int id);

private:
	void SetNotepadPos();
	string m_strHost;
	CCookie m_cookie; 
	CNotepadPlusplus m_notepad;

	HWND m_hWndPage1;
	HWND m_hWndPage2;
	HWND m_hWndPage3;
	HWND m_hWndPage4;

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINDLG_H__672A556B_A35E_4C70_B267_C805252131AA__INCLUDED_)
