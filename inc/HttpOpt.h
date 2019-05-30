// HttpOpt.h: interface for the CHttpOpt class.
//
/*
2019.05.24:
	1. �޸�BUG: ��gzipѹ���ļ���С > ��ѹ������ݴ�Сʱ, ������ĩ������.

2014.03.16:
	1. ����HTTP1.0֧��, HTTP1.0��Content Length���ԣ���Ҫͨ���������Ͽ����жϳ���
	2. 301,302�Զ��ض���
	3. ���ճ�ʱ15��
	4. ֧��gzipѹ����Ϊchuck��ʽ�����ҳ������
	5. ��gzip��ʽ������ҳ���Զ���ѹ����


Old date:
	chucked ��ʽ:
	--------------------------
	head
	0x0d 0x0a 0x0d 0x0a		
	
	xxxx 0x0d 0x0a		//xxxx����������0d 0a
	chucked data 0x0d 0x0a 

	xxxx 0x0d 0x0a //xxxx����������0d 0a
	chucked data 0x0d 0x0a 

	xxxx 0x0d 0x0a //xxxx����������0d 0a
	chucked data 0x0d 0x0a 
	....

	xxxx 0x0d 0x0a //xxxx������������0d 0a
	chucked data (there is not any 0d 0a)

	0x0d 0x0a 0x30 0x0d 0x0a 0x0d 0x0a	//last-chunk, ֻ��һλ0��ʾ����

	���ϵ�xxxx����:
	xxxx 0x0d 0x0a //xxxx:chucked data�L��,��λ�����̶�, 0x0a֮�����������

  RFC2616������ʾ��:
       length := 0
       read chunk-size, chunk-extension ��if any�� and CRLF
       while ��chunk-size > 0�� {
          read chunk-data and CRLF
          append chunk-data to entity-body
          length := length + chunk-size
          read chunk-size and CRLF
       }
       read entity-header
       while ��entity-header not empty�� {
          append entity-header to existing header fields
          read entity-header
       }
       Content-Length := length
       Remove "chunked" from Transfer-Encoding 
*/
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPOPT_H__B5B1A557_9C84_44ED_8954_723BA9B134A9__INCLUDED_)
#define AFX_HTTPOPT_H__B5B1A557_9C84_44ED_8954_723BA9B134A9__INCLUDED_
#pragma once

#ifdef WIN32
	//#include <winsock2.h>
	#include <windows.h>
#endif
#include "Cookie.h"
#include "HttpBase.h"


class CHttpOpt  : public CHttpBase
{
public:
	CHttpOpt();
	virtual ~CHttpOpt();
public:	
	__int64 InternetDownloadFile(const char *URL, const char *FileDest, WORD wPort=80);

protected:
	virtual int Recv(int socket1, char* buf, int len, int flags = 0);
	virtual int CloseSocket(int socket1);
	virtual int Send(SOCKET s, const char* buf, int len, int flags = 0);

private:

};

#endif // !defined(AFX_HTTPOPT_H__B5B1A557_9C84_44ED_8954_723BA9B134A9__INCLUDED_)
