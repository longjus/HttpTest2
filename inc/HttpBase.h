// HttpBase.h: interface for the CHttpBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPBASE_H__90CA44D1_4D53_4801_90F1_D3107012A61F__INCLUDED_)
#define AFX_HTTPBASE_H__90CA44D1_4D53_4801_90F1_D3107012A61F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "string.h"
#include <string>
using namespace std;

#define			C_RECVBUFF_LENGTH		16384


typedef enum {_http_type_unknown=-1, _http_type_header, _http_type_body, _http_type_file}emHttpType;
typedef struct tagHeadInfo
{
	float ver;
	int retCode;
	bool bChucked;
	bool bGzip;
	__int64 lenContent;
	int lenHead;
	char ContentType[50];
	const char* pBodyStart;
	char* Location;
	tagHeadInfo(){
		memset(this, 0, sizeof(tagHeadInfo));
	}
	~tagHeadInfo(){
		if(Location){
			delete[] Location;
			Location =NULL;
		}
	}
}HeadInfo, *LPHeadInfo;

class CHttpBase  
{
public:
	CHttpBase();
	virtual ~CHttpBase();

	string GetRevHeaderOfBody();
	string GetSendHeader();
	virtual int getRevHeader(const char* szHost, const char* szUrl, const char* szHead, BYTE *pBuffer, DWORD dwSize, WORD dwPort);
	virtual int HttpAjax(const char* szHost, const char* szMethod, const char* szUrl, const char* szHead, 
		const char* szUserData, const char* cookie, BYTE* pBuffer, DWORD nSize, WORD wPort=80, BOOL bAutoRedir=FALSE);

protected:
	virtual	int Connect(const char* szHost, WORD wPort, DWORD dwTimeout=15);
	virtual int CloseSocket(int socket1) = 0;
	virtual int Recv(int socket1, char* buf, int len, int flags = 0) = 0;
	virtual int Send(SOCKET s, const char* buf, int len, int flags = 0) = 0;

protected:
	string BuildHeader(emHttpType type, const char* szMethod, const char* szUrl, const char* szHost, const char* cookie, const char* szUserData);
	BOOL GetHttpHeadInfo(const char *str0, HeadInfo* pHi);
	int UnZip(BYTE* pBuffer, int lenBuff, int lenSrc);
	string getHeadAttr(const char* str0, const char* name);
	__int64 fnGetBuffer(emHttpType type, HeadInfo* pHi, const char* method, const char* szHost, 
		const char* szUrl, const char* szHead, const char* szUserData, const char* cookie, BYTE *pBuffer, 
		__int64 dwSize, BOOL bAtuoRedir, WORD wPort=80);
	bool GetHostUrl(const char* url0, char* host, char* url, int* lenHost, int* lenGet);

private:
	int recv_content_data(int socket1, char* recbuf, int nLeftBytes, char* pBuffer, __int64 llSize, HeadInfo* pHi);	
	int recv_chuck_data(int socket1, char* recbuf, int nLeftBytes, char* pBuffer, __int64 llSize, HeadInfo* pHi);
	__int64 recv_file_data(int socket1, const char* szFilePath, char* revbuf, int nLenFree, HeadInfo* pHi);
	int recv_head_data(int socket1, char* recbuf, int bufsize, HeadInfo* pHi, emHttpType type);
	int SkipChuck(const char *src, int pos, int lenSrc) const;
	int ReadCRLF(const char *src, int pos, int lenSrc) const;
	int CopyChuckBuffer(char* buff,  __int64 buffSize,  __int64& buffPos, 
		const char* src, int lenSrc, int l_chucksize, int& l_left, bool& bStartNew);


protected:
	string m_strRevHeader;
	string m_strSendHeader;
};

#endif // !defined(AFX_HTTPBASE_H__90CA44D1_4D53_4801_90F1_D3107012A61F__INCLUDED_)
