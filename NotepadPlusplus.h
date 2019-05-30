// NotepadPlusplus.h: interface for the CNotepadPlusplus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOTEPADPLUSPLUS_H__492B91C4_BD06_4294_9F94_A0D73CAE851E__INCLUDED_)
#define AFX_NOTEPADPLUSPLUS_H__492B91C4_BD06_4294_9F94_A0D73CAE851E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "inc/SciLexer/Scintilla.h"
#include "inc/SciLexer/SciLexer.h"

class CNotepadPlusplus  
{
public:
	void OnModified(NMHDR* nmhdr);
	void OnStyleNeeded(NMHDR* nmhdr);
	void OnMarginClicked(NMHDR* nmhdr);
	void OnDblClick(NMHDR* nmhdr);
	void UpdateUI();
	void SetContent(const char* str, int lenOfStr);
	void Init(HINSTANCE hInst, HWND hWndMain, int ctlId);
	CNotepadPlusplus();
	virtual ~CNotepadPlusplus();

	HWND m_hWnd;

protected:
	void ReloadStyles(const TCHAR* language);
};

#endif // !defined(AFX_NOTEPADPLUSPLUS_H__492B91C4_BD06_4294_9F94_A0D73CAE851E__INCLUDED_)
