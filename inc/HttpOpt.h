// HttpOpt.h: interface for the CHttpOpt class.
//
/*
2019.05.24:
	1. 修复BUG: 在gzip压缩文件大小 > 解压后的内容大小时, 缓冲区末有乱码.

2014.03.16:
	1. 增加HTTP1.0支持, HTTP1.0无Content Length属性，需要通过服务器断开来判断长度
	2. 301,302自动重定向
	3. 接收超时15秒
	4. 支持gzip压缩且为chuck方式传输的页面下载
	5. 按gzip方式接收网页，自动解压处理


Old date:
	chucked 格式:
	--------------------------
	head
	0x0d 0x0a 0x0d 0x0a		
	
	xxxx 0x0d 0x0a		//xxxx包含结束符0d 0a
	chucked data 0x0d 0x0a 

	xxxx 0x0d 0x0a //xxxx包含结束符0d 0a
	chucked data 0x0d 0x0a 

	xxxx 0x0d 0x0a //xxxx包含结束符0d 0a
	chucked data 0x0d 0x0a 
	....

	xxxx 0x0d 0x0a //xxxx不包含结束符0d 0a
	chucked data (there is not any 0d 0a)

	0x0d 0x0a 0x30 0x0d 0x0a 0x0d 0x0a	//last-chunk, 只有一位0表示结束

	以上的xxxx如下:
	xxxx 0x0d 0x0a //xxxx:chucked data長度,其位數不固定, 0x0a之後就是正文了

  RFC2616给出的示例:
       length := 0
       read chunk-size, chunk-extension （if any） and CRLF
       while （chunk-size > 0） {
          read chunk-data and CRLF
          append chunk-data to entity-body
          length := length + chunk-size
          read chunk-size and CRLF
       }
       read entity-header
       while （entity-header not empty） {
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
