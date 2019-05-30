// ios 推送
#pragma once
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "HttpBase.h"


#define			MAX_PATH_V1			256

// apple服务器参数
typedef			struct		tagIOSDATA
{
	UINT port;
	char url[MAX_PATH_V1];
	char pemfile[MAX_PATH_V1];
	char password[128];
	tagIOSDATA()
	{
		memset(this, 0, sizeof(tagIOSDATA));
	}
}IOSDATA;

// 发送数据
struct	ZGIOSMSGDATA
{
	string title;				// 消息标题
	int ver;					// 协议版本
	int sysid;					// 平台代码
	int mainfun;				// 主功能
	int subfun;					// 子功能
	string content;				// 消息内容
	string strToken;			// meid
	string button;				// 
};


class CHttpsOpt : public CHttpBase
{
public:
	CHttpsOpt(/*IOSDATA* lpIos, LPCTSTR strPath*/);
	virtual ~CHttpsOpt(void);

	BOOL Init();

protected:
	virtual int Connect(const char* szHost, WORD wPort, DWORD dwTimeout=15);
	virtual int Recv(int socket1, char* buf, int len, int flags = 0);
	virtual int CloseSocket(int socket1);
	virtual int Send(SOCKET s, const char* buf, int len, int flags = 0);

protected:
	int Chr2Hex(char c);
	int Hex2Chr(char *DstStr, const char *HexStr,int HexLen);

	//void ClearSocket(BIO* pObj);
	void ClearBuffer(char** pBuffer);
	void ClearObj();

private:
	SSL*		m_pSSL;
	SSL_CTX*	m_pSSLCtx;
	//BIO*		m_pSockBIO;
	char m_szError[MAX_PATH_V1];
//	char m_szPath[MAX_PATH_V1];

//	IOSDATA		m_iosData;							// IOS相关数据
};

