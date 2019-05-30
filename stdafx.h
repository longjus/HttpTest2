// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__DC588FE9_9798_426A_8F41_6436CC196B19__INCLUDED_)
#define AFX_STDAFX_H__DC588FE9_9798_426A_8F41_6436CC196B19__INCLUDED_

// Change these values to use different versions
#define WINVER		0x0600
//#define _WIN32_WINNT	0x0400
#define _WIN32_IE	0x0600
#define _RICHEDIT_VER	0x0100
#define _RICHEDIT_VER 0x300

#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>

#include <stdio.h>
#include <time.h>
inline void MOut(const char * szFormat, ...)
{
	char szBuffer[1024] = {0};
	va_list va;
	va_start(va, szFormat);
	vsprintf(szBuffer, szFormat, va);
	va_end(va);

	szBuffer[1023] = 0;
	OutputDebugString((LPCTSTR)szBuffer);
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__DC588FE9_9798_426A_8F41_6436CC196B19__INCLUDED_)
