/*************************************************************************
	> File Name: xchar.h
	> Author: longjus
	> Mail: longjus@gmail.com 
	> Created Time: 2013年03月27日 星期三 14时30分06秒
 ************************************************************************/
#pragma once
#ifndef _INC_X_TCHAR
#define _INC_X_TCHAR

#ifdef UNICODE
	#define	tstring wstring
	#define	t_string wstring
	#define	t_ofstream	wofstream
	#define	t_ifstream	wifstream
#else
	#define	tstring string
	#define	t_string string
	#define	t_ofstream	ofstream
	#define	t_ifstream	ifstream
#endif

/*win32*/
#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <tchar.h>
	#define		DIRECTORY_SEPARATOR	((TCHAR)'\\')
	#define		DIRECTORY_SEPARATOR_STR	_T("\\")
	#define 	THREADVOID void

	extern void MOut(TCHAR * szFormat, ...);
#else
/*linux/unix*/
	#include "tchar.h"
	#define		DIRECTORY_SEPARATOR	((TCHAR)'/')
	#define		DIRECTORY_SEPARATOR_STR	_T("/")
	#define 	THREADVOID void*	
	#ifndef FALSE
	#define FALSE 0
	#endif
 
	#ifndef TRUE
	#define TRUE 1
	#endif

#ifndef MAX_PATH
#define MAX_PATH	PATH_MAX
#endif
#ifndef _MAX_PATH
#define _MAX_PATH	PATH_MAX
#endif

//#ifndef FAR
//#define FAR                 far
//#endif
#ifndef CHAR
typedef char 					CHAR;
#endif
#ifndef WORD
typedef unsigned short      WORD;
#endif
#ifndef DWORD
typedef unsigned long       DWORD;
#endif
#ifndef BOOL
typedef int                 BOOL;
#endif
#ifndef BYTE
typedef unsigned char       BYTE;
#endif
#ifndef FLOAT
typedef float               FLOAT;
#endif
#ifndef ULONG
typedef unsigned long ULONG;
#endif
#ifndef UINT
typedef unsigned int UINT;
#endif
#ifndef HANDLE
typedef void *HANDLE;
#endif

#ifndef LPCTSTR
typedef const TCHAR *	LPCTSTR;
#endif

#ifndef LPBYTE
typedef BYTE *			LPBYTE;
#endif

#ifndef __int64
typedef long long		__int64;
#endif

#define _vsnprintf vsnprintf
#define _atoi64(a) atoll(a)
#define strstri	strcasestr

#ifdef UNICODE
	#define _wcsnicmp wcsncasecmp
	#define _snwprintf swprintf
#else
	#define _strnicmp strncasecmp
	#define stristr	strcasestr
	#define strstri	strcasestr
#endif
#define MOut _tprintf
#define OutputDebugString _tprintf 
#define GetLastError() 0
/*end define linux/unix */
#endif 
/*end define win32*/

#endif
