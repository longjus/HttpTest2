// maindlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "maindlg.h"
#include <atlmisc.h>

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) 
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) 
		return TRUE;
	else	return IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}


LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();
	
	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);
	
	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);
	
	UIAddChildWindowContainer(m_hWnd);
	
	// Methord
	CComboBox* pCBB = new CComboBox();
	if(pCBB)
	{
		pCBB->Attach(GetDlgItem(IDC_METHOD));
		pCBB->AddString("HEAD");
		pCBB->AddString("GET");
		pCBB->AddString("POST");
		pCBB->SetCurSel(1);
		pCBB->Detach();
		delete pCBB;
	}
	
	//TAB
	HWND hWndTab = GetDlgItem(IDC_TAB1);
	TCITEM tc;  
	tc.mask        = TCIF_TEXT;  
	tc.iImage      = 0;  
	tc.lParam      = 0;  
	//tc.cchTextMax  = 4;  
	
	tc.pszText     = TEXT(" 返回头 ");
	SendMessage(hWndTab, TCM_INSERTITEM, 0, (LPARAM)&tc);  
	
	tc.pszText     = TEXT(" 返回内容 ");
	SendMessage(hWndTab, TCM_INSERTITEM, 1, (LPARAM)&tc);  
	
	tc.pszText     = TEXT(" 设置 ");
	SendMessage(hWndTab, TCM_INSERTITEM, 2, (LPARAM)&tc);  
	
	tc.pszText     = TEXT(" 请求数据 ");
	SendMessage(hWndTab, TCM_INSERTITEM, 3, (LPARAM)&tc);  

	HINSTANCE hInst = NULL;
	m_hWndPage1 = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_PAGE1), hWndTab, (DLGPROC)NULL, 0);  
	//m_hWndPage2 = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_PAGE2), hWndTab, (DLGPROC)NULL, 0);  
	m_hWndPage3 = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_PAGE3), hWndTab, (DLGPROC)NULL, 0);  
	m_hWndPage4 = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_PAGE4), hWndTab, (DLGPROC)NULL, 0);  

	// protocal
	pCBB = new CComboBox();
	if(pCBB)
	{
		pCBB->Attach(GetDlgItem(IDC_PROTOCOL));
		pCBB->AddString("http");
		pCBB->AddString("https");
		pCBB->SetCurSel(0);
		pCBB->Detach();
		delete pCBB;
	}

	// page1
	setText(m_hWndPage1, IDC_HOST,		"www.163.com");
	setText(m_hWndPage1, IDC_PORT,		"80");
	setText(m_hWndPage1, IDC_URL,		"/");
	::SendMessage(::GetDlgItem(m_hWndPage1, IDC_HTTP_HEAD), EM_SETLIMITTEXT, 2147483648, 0);

	// page2
	m_notepad.Init(hInst, m_hWnd, IDC_NOTEPAD);
	::ShowWindow(m_notepad.m_hWnd, SW_HIDE);
	//::SendMessage(::GetDlgItem(m_hWndPage2, IDC_CONTENT), EM_SETLIMITTEXT, 2147483648, 0);

	// page3
	setText(m_hWndPage3, IDC_USERDATA,	"userid=admin&pwd=123456");
	setText(m_hWndPage3, IDC_HEADDATA,	"GET / HTTP/1.1\r\n"
									"Host: news.163.com\r\n"
									"Accept: */*\r\n"
									"Cache-Conctrol: nocache\r\n"
									"Connection: Keep-Alive\r\n"
									"User-Agent: Mozilla/99.9[en](Apple&Adriod)\r\n"
									"Content-Type: application/x-www-form-urlencoded\r\n"
									"Accept-Encoding:gzip,deflate\r\n"
									"\r\n"
									);
	
	// page4
	setText(m_hWndPage4, IDC_HELP2,	
		"1. Cookie 在取到内容时会保存下来\r\n"
		"2. 再次发出请求时 Cookie 会发出去\r\n"
		"3. 参见http1.1协议文档\r\n"
		);
	
	// show 
	::ShowWindow(m_notepad.m_hWnd, SW_HIDE); 
	::SetWindowPos(m_notepad.m_hWnd, HWND_TOP, 10, 95, 0, 0, SWP_NOSIZE|SWP_HIDEWINDOW);

	::SetWindowPos(m_hWndPage1, HWND_TOP, 5, 25, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
	//::SetWindowPos(m_hWndPage2, HWND_TOP, 5, 25, 0, 0, SWP_NOSIZE|SWP_HIDEWINDOW);
	::SetWindowPos(m_hWndPage3, HWND_TOP, 5, 25, 0, 0, SWP_NOSIZE|SWP_HIDEWINDOW);
	::SetWindowPos(m_hWndPage4, HWND_TOP, 5, 25, 0, 0, SWP_NOSIZE|SWP_HIDEWINDOW);
	
	return TRUE;
}

// Application
LRESULT CMainDlg::OnClickedBtn_gethead(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	SetDlgItemText(IDC_MSG, "");
	setText(m_hWndPage1, IDC_HTTP_HEAD, "");
	setText(m_hWndPage4, IDC_HELP2,	"");
	m_notepad.SetContent("", 0);
	
	string protocal		= getString(IDC_PROTOCOL);
	string method		= getString(IDC_METHOD);

	CButton btn;
	btn.Attach(::GetDlgItem(m_hWndPage3, IDC_SEND_TIP2));
	string strHeader	= getString(m_hWndPage3, IDC_HEADDATA);
	int bCheckHeader	= btn.GetCheck();
	btn.Detach();

	int nType = ("http" == protocal) ? 0 : (("https" == protocal) ? 1 : 0);
	GetHttp(nType, bCheckHeader ? strHeader.c_str() : NULL);
	
	return 0;
}

#define		MYSHOW(n) ::ShowWindow(m_hWndPage##n, SW_SHOWDEFAULT)

LRESULT CMainDlg::OnSelchangeTab1(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	HWND hWndTab = GetDlgItem(IDC_TAB1);
	
	::ShowWindow(m_hWndPage1, SW_HIDE);
	//::ShowWindow(m_hWndPage2, SW_HIDE);
	::ShowWindow(m_notepad.m_hWnd, SW_HIDE);
	::ShowWindow(m_hWndPage3, SW_HIDE);
	::ShowWindow(m_hWndPage4, SW_HIDE);
	
	int i = ::SendMessage(hWndTab, TCM_GETCURSEL, 0, 0);
	switch(i)
	{
	case 0:			MYSHOW(1);			break;
	case 1:
		{
			MYSHOW(2);			
			SetNotepadPos();
			::ShowWindow(m_notepad.m_hWnd, SW_SHOW);
			break;
		}
	case 2:			MYSHOW(3);			break;
	case 3:			MYSHOW(4);			break;
	}
	return 0;
}


LRESULT CMainDlg::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hWndTab = GetDlgItem(IDC_TAB1);
	if(NULL==hWndTab)
		return NULL;

	CRect rcClient;
	GetClientRect(&rcClient);

	// tab
	int x = (rcClient.left += 8);
	int y = (rcClient.top += 40);
	int w = rcClient.Width() - 10;
	int h = rcClient.Height() - 40;
	::SetWindowPos(hWndTab, HWND_TOP, x, y, w, h, SWP_SHOWWINDOW);
	

	// msg tip
	::SetWindowPos(GetDlgItem(IDC_MSG), HWND_TOP, x, rcClient.bottom-25, w, 22, SWP_NOACTIVATE);

	// page1
	CRect rcTab;
	::GetClientRect(hWndTab, &rcTab);
	::SetWindowPos(m_hWndPage1, HWND_TOP, rcTab.left+1, rcTab.top+22, rcTab.Width()-2, rcTab.Height()-24, SWP_NOACTIVATE);

	CRect rcPage1;
	::GetClientRect(m_hWndPage1, &rcPage1);

	x = rcPage1.left + 5;
	y = rcTab.bottom - 100;
	w = rcPage1.Width()-10;
	h = 70;
	::SetWindowPos(::GetDlgItem(m_hWndPage1, IDC_COOKIE), HWND_TOP, x, y, w, h, SWP_NOACTIVATE);

	y = rcTab.bottom - 118;
	h = 20;
	::SetWindowPos(::GetDlgItem(m_hWndPage1, IDC_COOKIE_TIP), HWND_TOP, x, y, w, h, SWP_NOACTIVATE);

	x = rcPage1.left + 5;
	y = rcPage1.top + 60;
	w = rcPage1.Width()-10;
	h = rcPage1.Height()-160;
	::SetWindowPos(::GetDlgItem(m_hWndPage1, IDC_HTTP_HEAD), HWND_TOP, x, y, w, h, SWP_NOACTIVATE);
	::SetWindowPos(::GetDlgItem(m_hWndPage1, IDC_URL), HWND_TOP, x, y, w-50, 21, SWP_NOACTIVATE|SWP_NOMOVE);

	// page3 
	::SetWindowPos(m_hWndPage3, HWND_TOP, rcTab.left+1, rcTab.top+22, rcTab.Width()-2, rcTab.Height()-24, SWP_NOACTIVATE);
	y = rcPage1.top + 30;
	h = rcPage1.Height() * 2./5;
	::SetWindowPos(::GetDlgItem(m_hWndPage3, IDC_USERDATA), HWND_TOP, x, y, w, h, SWP_NOACTIVATE);

	y += (h + 30);
	h = rcPage1.bottom - y - 8;
	::SetWindowPos(::GetDlgItem(m_hWndPage3, IDC_HEADDATA), HWND_TOP, x, y, w, h, SWP_NOACTIVATE);

	y -= 20;
	h = 20;
	::SetWindowPos(::GetDlgItem(m_hWndPage3, IDC_SEND_TIP2), HWND_TOP, x, y, w, h, SWP_NOACTIVATE);

	// page4
	::SetWindowPos(m_hWndPage4, HWND_TOP, rcTab.left+1, rcTab.top+22, rcTab.Width()-2, rcTab.Height()-24, SWP_NOACTIVATE);
	y = rcPage1.top + 80;
	h = rcPage1.Height()-30;
	::SetWindowPos(::GetDlgItem(m_hWndPage4, IDC_HELP2), HWND_TOP, x, y, w-12, h, SWP_NOACTIVATE|SWP_NOMOVE);

	// notepad
	SetNotepadPos();
	return 0;
}

LRESULT CMainDlg::OnUpdataUI(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	switch(pnmh->code)
	{
	case SCN_STYLENEEDED:	m_notepad.OnStyleNeeded(pnmh); break; 
	case SCN_UPDATEUI:		m_notepad.UpdateUI(); break;
	case SCN_DOUBLECLICK:	m_notepad.OnDblClick(pnmh); break;
	case SCN_MARGINCLICK:	m_notepad.OnMarginClicked(pnmh); break;
	case SCN_MODIFIED:		m_notepad.OnModified(pnmh); break;
	}
	return 0;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}


BOOL CMainDlg::getCheck(HWND page, int id)
{
	CButton* p = new CButton();
	p->Attach(::GetDlgItem(page, id));
	BOOL ret = !p->GetCheck();
	
	p->Detach();
	delete p;
	
	return ret;
}

int CMainDlg::getInt(HWND page, int id)
{
	int ret = -1;
	CEdit* p = new CEdit();
	if(p)
	{
		p->Attach(::GetDlgItem(page, id));
		char buf[200];
		p->GetWindowText(buf, 200);
		ret = atoi(buf);
	}
	return ret;
}

string CMainDlg::getString(HWND page, int id)
{
	string ret;
	TCHAR* buf = new TCHAR[BUFFER_LEN_2MB];
	if(buf)
	{
		memset(buf, 0, BUFFER_LEN_2MB);
		::GetDlgItemText(page, id, buf, BUFFER_LEN_2MB-1);
		ret = buf;
		delete[] buf;
	}
	
	return ret;
}

void CMainDlg::setText(HWND page, int ctrlid, const char *str)
{
	HWND hwndIt = ::GetDlgItem(page, ctrlid);
	if(hwndIt)
		::SetWindowText(hwndIt, str);
}

string CMainDlg::getString(int id)
{
	string ret;
	TCHAR* buf = new TCHAR[BUFFER_LEN_2MB];
	if(buf)
	{
		memset(buf, 0, BUFFER_LEN_2MB);
		GetDlgItemText(id, buf, BUFFER_LEN_2MB-1);
		ret = buf;
		delete[] buf;
	}
	
	return ret;
}

void CMainDlg::GetHttp(int type, const char* strHeader)
{
	TCHAR* buf = new TCHAR[BUFFER_LEN_2MB];
	if(NULL==buf)
		return;

	string sHost	= getString(m_hWndPage1, IDC_HOST);
	string sUrl		= getString(m_hWndPage1, IDC_URL);
	int port		= getInt(m_hWndPage1, IDC_PORT);
	string method	= getString(IDC_METHOD);

	BOOL bAutoDir	= getCheck(m_hWndPage1, IDC_CK);
	string cookie	= getString(m_hWndPage1, IDC_COOKIE);
	string postData	= getString(m_hWndPage3, IDC_USERDATA);

	if(m_strHost != sHost)
	{
		m_strHost = sHost;
		m_cookie.Clear();
		setText(m_hWndPage1, IDC_COOKIE, "");
	}

	int nLen;
	CHttpBase* ho;
	memset(buf, 0, BUFFER_LEN_2MB);
	if(0==type)
		ho = new CHttpOpt();
	else
		ho = new CHttpsOpt();

	if(NULL==ho)
	{
		delete[] buf;
		return;
	}

	nLen = ho->HttpAjax(sHost.c_str(), method.c_str(), sUrl.c_str(), strHeader, 
		postData.c_str(), cookie.c_str(), (BYTE*)buf, BUFFER_LEN_2MB, port, bAutoDir);
	
	setText(m_hWndPage1, IDC_HTTP_HEAD, ho->GetRevHeaderOfBody().c_str());
	setText(m_hWndPage4, IDC_HELP2,	ho->GetSendHeader().c_str());
	if(nLen>0)
	{
		string strOut;
		int isUtf8 = IsTextUtf8(buf, nLen);
		if(isUtf8)
			UTF8ToAnsi(strOut, (const CHAR*)buf);
		else
			strOut = buf;
		
		char msg[1000];
		sprintf(msg, "接收内容长度:%d bytes, 编码:%s", nLen, nLen>0?(isUtf8 ? "UTF-8" : "GB2312"):"无");
		SetDlgItemText(IDC_MSG, msg);

		m_notepad.SetContent(strOut.c_str(), strOut.length());
		
		m_cookie.push(ho->GetRevHeaderOfBody().c_str());
		setText(m_hWndPage1, IDC_COOKIE, m_cookie.getCookie().c_str());
	}

	delete ho;
	delete[] buf;
}


LRESULT CMainDlg::OnSelchangeProtocol(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int i = ::SendMessage(GetDlgItem(IDC_PROTOCOL), CB_GETCURSEL, 0, 0);

	char* port = (0==i) ? "80" : "443";
	setText(m_hWndPage1, IDC_PORT, port);
	
	return 0;
}

void CMainDlg::SetNotepadPos()
{
	HWND hWndTab = GetDlgItem(IDC_TAB1);
	if(hWndTab)
	{
		CRect rcTab;
		::GetWindowRect(hWndTab, &rcTab);
		POINT pt;
		pt.x = (rcTab.left += 4);
		pt.y = (rcTab.top += 25);
		::ScreenToClient(m_hWnd, &pt);
		int w = rcTab.Width() - 6;
		int h = rcTab.Height() - 4;
		::SetWindowPos(m_notepad.m_hWnd, HWND_TOP, pt.x, pt.y, w, h, SWP_NOACTIVATE);
	}

}
