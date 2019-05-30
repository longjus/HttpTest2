// HttpOpt.cpp: implementation of the CHttpOpt class.
//
//////////////////////////////////////////////////////////////////////
#include "HttpOpt.h"
#include<stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#ifdef WIN32
	#include <tchar.h>
	#include <malloc.h>
	#include <atlbase.h>
	#define LONG_LONG_MAX 9223372036854775807
	#pragma comment(lib, "ws2_32.lib")
	#pragma comment(lib, "lib/zlibd.lib")
#else
	#include "xchar.h"
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#define closesocket(a) close(a)
	#define recv(a,b,c,d) read(a,b,c)
#endif

#include <string>
using namespace std;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHttpOpt::CHttpOpt()
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD( 1, 1 );
	int err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		_tprintf(_T("inisocket error\n"));
	}
#endif
}

CHttpOpt::~CHttpOpt()
{
#ifdef WIN32
	WSACleanup();
#endif
}


__int64 CHttpOpt::InternetDownloadFile(const char *url0, const char *FileDest, WORD uPort/*=80*/)
{
	if(NULL==url0 || NULL==FileDest)
	{
		_tprintf(_T("Error, url0 and FileDest is NULL.\n"));
		return ((__int64)false);
	}

	char* host = NULL;
	char* get = NULL;
	int lenHost, lenGet;
	if(GetHostUrl(url0, host, get, &lenHost, &lenGet))
	{
		host = (char*)alloca((lenHost+1)*sizeof(char));
		get = (char*)alloca((lenGet+1)*sizeof(char));
		GetHostUrl(url0, host, get, &lenHost, &lenGet);
	}

	BYTE* pBuffer = (BYTE*) FileDest;
	__int64 dwFileSize = LONG_LONG_MAX;	//max file size is 2^64/2-1
	HeadInfo hi; 
	return fnGetBuffer(_http_type_file, &hi, "GET", &host[0], &get[0], NULL, NULL, NULL, pBuffer, (const __int64)dwFileSize, TRUE, uPort);
}


int CHttpOpt::Recv(int socket1, char* buf, int len, int flags/* = 0*/)
{
	return recv(socket1, buf, len, flags);
}

int CHttpOpt::CloseSocket(int socket1)
{
	return closesocket(socket1);
}

int CHttpOpt::Send(SOCKET s, const char* buf, int len, int flags)
{
	return send(s, buf, len, flags);
}


