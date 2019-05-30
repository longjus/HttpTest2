#include "StdAfx.h"
#include "HttpsOpt.h"

#pragma comment (lib, "WS2_32.lib")
#pragma comment( lib, "lib/libeay32.lib" )
#pragma comment( lib, "lib/ssleay32.lib" )

CHttpsOpt::CHttpsOpt()
{
	Init();
}


CHttpsOpt::~CHttpsOpt(void)
{
	ClearObj();
}

int CHttpsOpt::Chr2Hex(char c)
{
	int iResult = 0;
	if ( c >= '0' && c <= '9'){
		iResult = c-'0';
	}else if (c >= 'a' && c <= 'f'){
		iResult = c - 'a' + 10;
	}else if (c >= 'A' && c <= 'F'){
		iResult = c - 'A' + 10;
	}
	return iResult;
}

int CHttpsOpt::Hex2Chr(char *DstStr, const char *HexStr, int HexLen)
{
	int DesIndex = 0;
	for (int iSrcIndex=0; iSrcIndex < HexLen;)
	{
		if (*(HexStr+iSrcIndex)<'0' 
			|| (*(HexStr+iSrcIndex)>'9' && *(HexStr+iSrcIndex)<'A') 
			|| (*(HexStr+iSrcIndex)>'Z'&& *(HexStr+iSrcIndex)<'a') 
			|| *(HexStr+iSrcIndex)>'z')
		{
			iSrcIndex++;
			continue;
		}
		int iBig = Chr2Hex(*(HexStr+iSrcIndex));
		iSrcIndex++;
		int iSmall = Chr2Hex(*(HexStr+iSrcIndex));
		iSrcIndex++;

		*(DstStr+DesIndex) = iBig*16+iSmall;
		DesIndex++;
	}
	return DesIndex;
}

void CHttpsOpt::ClearBuffer(char** pBuffer)
{
	if(*pBuffer){
		delete[] *pBuffer;
		*pBuffer = NULL;
	}
}

void CHttpsOpt::ClearObj()
{
	if(m_pSSLCtx)
	{
		SSL_CTX_free(m_pSSLCtx);
		m_pSSLCtx = NULL;
	}
}

BOOL CHttpsOpt::Init()
{
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSLeay_add_ssl_algorithms();
    SSL_load_error_strings();

	m_pSSLCtx = SSL_CTX_new(SSLv23_client_method());
	if(m_pSSLCtx == NULL)
	{
		MOut("Error, SSL CTX new Fail.\n");
		ClearObj();
		return false;
	}
	SSL_CTX_set_options(m_pSSLCtx, SSL_OP_ALL);

	return true;
}


int CHttpsOpt::Connect(const char* szHost, WORD wPort, DWORD dwTimeout)
{
	if(NULL==m_pSSLCtx)
	{
		MOut("Error, m_pSSLCtx==NULL");
		return 0;
	}

	//检查证书是否有效
	BIO* pSockBIO = BIO_new_ssl_connect(m_pSSLCtx);
	if(pSockBIO == NULL)
	{
		ERR_error_string(ERR_get_error(), (char* )m_szError);
		MOut("Error, [CSSLConnect::new connect] pSockBIO ERROR: %s.\n", m_szError);
		return 0;
	}

	BIO_get_ssl(pSockBIO, &m_pSSL);
	if(m_pSSL == NULL)
	{
		ERR_error_string(ERR_get_error(), (char* )m_szError);
		MOut("Error, [CSSLConnect::get ssl] m_pSSL ERROR: %s.\n", m_szError);
		CloseSocket((int)pSockBIO);
		return 0;
	}

	if(SSL_get_verify_result(m_pSSL) != X509_V_OK)
	{
		//X509证书无效
		ERR_error_string(ERR_get_error(), (char* )m_szError);
		MOut("Error, [CSSLConnect::get result] SSL_get_verify_result ERROR: %s.\n", m_szError);
		CloseSocket((int)pSockBIO);
		return 0;
	}
	SSL_set_mode(m_pSSL, SSL_MODE_AUTO_RETRY);

	char szURL[200]   = {'\0'};
	_snprintf(szURL, sizeof(szURL), "%s:%d", szHost, wPort);
	BIO_set_conn_hostname(pSockBIO, szURL);
	if (NULL == pSockBIO)
	{
		ERR_error_string(ERR_get_error(), (char* )m_szError);
		MOut("Error, [CSSLConnect::set host] pSockBIO Connect ERROR: %s.\n", m_szError);
		CloseSocket((int)pSockBIO);
		return 0;
	}
	
	int nRet= BIO_do_connect(pSockBIO);
	if(nRet <= 0)
	{
		DWORD dwError = WSAGetLastError();
		ERR_error_string(ERR_get_error(), (char* )m_szError);
		MOut("Error, [CSSLConnect::Connect] pSockBIO Connect ERROR: %s.\n", m_szError);
		CloseSocket((int)pSockBIO);
		return 0;
	}

	return (int)pSockBIO;
}

int CHttpsOpt::Recv(int socket1, char* buf, int lenOfbuf, int flags/* = 0*/)
{
	int len =BIO_read((BIO*)socket1, buf, lenOfbuf);
	return len;
}


int CHttpsOpt::CloseSocket(int socket1)
{
	BIO_free_all((BIO*)socket1);
	return 0;
}


int CHttpsOpt::Send(SOCKET s, const char* buf, int lenOfBuf, int flags)
{
	int nPos    = 0;
	while(true)
	{
		if(nPos == lenOfBuf )
		{
			break;
		}
		int nLen = BIO_write((BIO*)s, buf+nPos, lenOfBuf-nPos);
		if(nLen <= 0)
		{
			break;
		}
		else
		{
			nPos += nLen;
		}
		//MOut("OK, write: pos %d, nlen %d\n ", nPos, nLen);
	}

	return nPos;
}




