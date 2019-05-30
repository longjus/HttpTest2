// HttpBase.cpp: implementation of the CHttpBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HttpBase.h"
#include "zlib/zlib.h"

inline unsigned int hextoi(const char *hexstring, const int len=4)
{
	const char* h = hexstring;
	register unsigned int  c, v;
	v = 0;
	if (*h == '0' && (*(h+1) == 'x' || *(h+1) == 'X')) 
		h += 2;

	int pos = 0;
	for(; (c = (unsigned int)*h++) != 0; pos++) 
	{
		if(pos>=len)
			break;
		if (c >= '0' && c <= '9') 
			c -= '0';
		else if (c >= 'a' && c <= 'f') 
			c = (c - 'a') + 10;
		else if (c >=  'A' && c <= 'F') 
			c = (c - 'A') + 10;
		else 
			break;
		v = (v * 0x10) + c;
	}
	return v;
}

#ifdef WIN32
char * __cdecl strcasestr(
        const char * str1,
        const char * str2
        )
{
        char *cp = (char *) str1;
        char *s1, *s2;

        if ( !*str2 )
            return((char *)str1);

        while (*cp)
        {
                s1 = cp;
                s2 = (char *) str2;

                while ( *s1 && *s2 && 
					(!(*s1-*s2) || 
						( (*s1>='A' && *s1<='Z' && *s2>='a' && *s2<='z' && !(*s2-32-*s1)) || 
						  (*s1>='a' && *s1<='z' && *s2>='A' && *s2<='Z' && !(*s1-32-*s2)) )
					))
                        s1++, s2++;

                if (!*s2)
                        return(cp);

                cp++;
        }

        return(NULL);
}
#endif

/*
	rfc1952.txt
		ID1 (IDentification 1)
        ID2 (IDentification 2)
            These have the fixed values ID1 = 31 (0x1f, \037), ID2 = 139
            (0x8b, \213), to identify the file as being in gzip format.
*/
int UncompressData( const BYTE* abSrc, int nLenSrc, BYTE* abDst, int nLenDst )
{
 	if(nLenSrc<2)
		return -1;
	
	if(*(abSrc+0)!=0x1f && *(abSrc+0)!=0x8b)
		return -1;
	
	z_stream zInfo ={0};
    zInfo.total_in=  zInfo.avail_in=  nLenSrc;
    zInfo.total_out= zInfo.avail_out= nLenDst;
    zInfo.next_in= (BYTE*)abSrc;
    zInfo.next_out= abDst;

    int nErr, nRet= -1;
    nErr= inflateInit( &zInfo );               // zlib function
    if ( nErr == Z_OK ) {
        nErr= inflate( &zInfo, Z_FINISH );     // zlib function
        if ( nErr == Z_STREAM_END ) {
            nRet= zInfo.total_out;
        }
    }
    inflateEnd( &zInfo );   // zlib function
    return( nRet ); // -1 or len of output
}

/*
Call:
	char* dest = new char[dwBytes*10];
	memset(dest, 0, sizeof(char)*dwBytes*10);
	int totalsize = 0;
	int ret = inflate_read((char*)(*pBuffer), dwBytes, &dest, totalsize, 1);
	if(ret==Z_OK && totalsize<BUFFER_LEN_2MB)
	{
		strncpy((char*)*pBuffer, dest, totalsize);
		BYTE* pBufTmp = (BYTE*)*pBuffer;
		*(pBufTmp + totalsize) = 0;
	}
	delete[] dest;
	dest = NULL;
*/
int inflate_read(char *source,int len, char **dest, int& totalsize, int gzip=0) 
{ 
	int ret; 
	unsigned have; 
	z_stream strm; 
	const int CHUNK = 1024;
	unsigned char out[CHUNK]; 
	
	/* allocate inflate state */ 
	strm.zalloc = Z_NULL; 
	strm.zfree = Z_NULL; 
	strm.opaque = Z_NULL; 
	strm.avail_in = 0; 
	strm.next_in = Z_NULL; 
	
	if(gzip)
		ret = inflateInit2(&strm, 47); 
	else 
		ret = inflateInit(&strm); 
	
	if (ret != Z_OK) 
		return ret; 
	
	strm.avail_in = len; 
	strm.next_in = (BYTE*)source; //......
	
	/* run inflate() on input until output buffer not full */ 
	do { 
		strm.avail_out = CHUNK; 
		strm.next_out = out; 
		ret = inflate(&strm, Z_NO_FLUSH); 
		switch (ret) { 
		case Z_NEED_DICT: 
			ret = Z_DATA_ERROR; /* and fall through */ 
		case Z_DATA_ERROR: 
		case Z_MEM_ERROR: 
			inflateEnd(&strm); 
			return ret; 
		} 
		have = CHUNK - strm.avail_out; 
		totalsize += have; 
		*dest = (char*)realloc(*dest,totalsize); //....
		memcpy(*dest + totalsize - have,out,have); 
	} while (strm.avail_out == 0); 
	
	/* clean up and return */ 
	(void)inflateEnd(&strm); 
	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR; 
} 

#define windowBits 15
#define ENABLE_ZLIB_GZIP 32
int ungzip( const BYTE* abSrc, int nLenSrc, BYTE* abDst, int nLenDst )
{
	if(nLenSrc<2)
		return -1;
	
	if(*(abSrc+0)!=0x1f && *(abSrc+0)!=0x8b)
		return -1;
	
	z_stream zInfo ={0};
    zInfo.total_in=  zInfo.avail_in=  nLenSrc;
    zInfo.total_out= zInfo.avail_out= nLenDst;
    zInfo.next_in= (BYTE*)abSrc;
    zInfo.next_out= abDst;

    int nErr, nRet= -1;
    nErr= inflateInit2 (& zInfo, windowBits | ENABLE_ZLIB_GZIP);               // zlib function
    if ( nErr == Z_OK ) {
        nErr= inflate( &zInfo, Z_FINISH );     // zlib function
        if ( nErr == Z_STREAM_END ) {
            nRet= zInfo.total_out;
        }
    }
    inflateEnd( &zInfo );   // zlib function
    return( nRet ); // -1 or len of output
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHttpBase::CHttpBase()
{

}

CHttpBase::~CHttpBase()
{

}


string CHttpBase::BuildHeader(emHttpType type, const char* method, const char* szHost, const char* szUrl, 
						   const char* cookie, const char* szUserData)
{
    //char* header	= pBuffer;
	string header;
    header = method;
    header +=  " ";
    header += szUrl;
    header += " HTTP/1.1";
    header += "\r\nHost: ";
    header += szHost;

    header += "\r\nAccept: */*"
        "\r\nCache-Conctrol: nocache"
        "\r\nConnection: Keep-Alive"
        "\r\nUser-Agent: Mozilla/99.9[en](Apple&Adriod)"
        "\r\nContent-Type: application/x-www-form-urlencoded"
		"\r\nAccept-Encoding:gzip,deflate";

	if(cookie && strlen(cookie) > 0)
	{
		header += "\r\nCookie: ";
		header += cookie;
	}

	bool bPost = ((0 == stricmp("POST", method)) && szUserData && strlen(szUserData) > 0);
	if(bPost)
	{
		char buff[100];
		_snprintf(buff, 100-1, "\r\nContent-Length: %d", strlen(szUserData));
		header += buff;
	}

	if(type==_http_type_file)
	{
		header += "\r\nReferer: http://";
		header +=  szHost;
		header += "/";
	}else if(type==_http_type_body){
	}

    header += "\r\n\r\n";

	if(bPost)
	{
		header += szUserData;
	}

	return header;
}


BOOL CHttpBase::GetHttpHeadInfo(const char *str0, HeadInfo* pHi)
{
	if(NULL==pHi)
		return FALSE;
	
	const char* p = strstr(str0, "\r\n\r\n");
	if(!p)
		return -1;
	pHi->pBodyStart		= p+strlen("\r\n\r\n");
	pHi->lenHead = p - str0 + strlen("\r\n\r\n");// + 1;
	const char* strHead = str0;	
	const char* p0 = NULL;

	//HTTP/1.1 200 OK
	p0 = strcasestr(strHead, "HTTP/");
	if(p0)
	{
		pHi->ver = atof(p0+strlen("HTTP/"));
		pHi->retCode = atoi(p0+strlen("HTTP/1.1 "));
	}

	//Content-Encoding: gzip 或 Content-Type: application/x-gzip
	p0 = strcasestr(strHead, "Content-Encoding:");
	if(p0)
	{
		pHi->bGzip = true;
	}
	
	//Location: 
	if(pHi->retCode==301 || pHi->retCode==302)
	{
		if((p0=strcasestr(strHead, "Location:")))
		{
			p0 += strlen("Location:");
			const char* p1 = strstr(p0, "\r\n");
			int nLenLoc = p1-p0;
			if(p1 && nLenLoc>0)
			{
				if(pHi->Location)
					delete[] pHi->Location;
				pHi->Location = new char[nLenLoc+1];
				strncpy(pHi->Location, p0, nLenLoc);
				pHi->Location[nLenLoc] = '\0';
			}
		}
	}

	p0 = strcasestr(strHead, "Transfer-Encoding:");
	if(p0)
	{
		pHi->bChucked = true;
	}
	else if((p0=strcasestr(strHead, "Content-Length:")))
	{
		pHi->bChucked = false;
		p0 += strlen("Content-Length:");
		const char* p1 = strstr(p0, "\r\n");
		if(p1)
		{
			pHi->lenContent = _atoi64(p0);
		}
	}

	return TRUE;
}

int CHttpBase::Connect(const char* szHost, WORD wPort, DWORD dwTimeout)
{
    int socket1;
    struct sockaddr_in addr;
    if((socket1 = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) <0)
    {
		_tprintf(_T("Create socket error, err:%s,errno:%d\n"), strerror(errno), errno);
        return 0;
    }

	// get ip
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    struct hostent *phostent = gethostbyname(szHost);
	for(int n=0;phostent->h_addr_list[n];n++) 
	{ 
		memcpy(&addr.sin_addr.s_addr, phostent->h_addr_list[n], phostent->h_length); 
		//_tprintf(_T("address:%s\n"), inet_ntoa(addr.sin_addr)); 
		break;
	}
    addr.sin_port = htons(wPort);

    if(connect(socket1, (struct sockaddr*)&addr, sizeof(addr))<0)
    {
        _tprintf(_T("Connect error: %s, no:%d\n"), strerror(errno), errno);
		closesocket(socket1);
        return 0;
    }

	if(dwTimeout > 0)
	{
		//int timeout = dwTimeout * 1000;
		//setsockopt(socket1 ,SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
	}

	return socket1;
}

int CHttpBase::UnZip(BYTE* pBuffer, int lenBuff, int lenSrc)
{
	int nLenUnzip = 0;
	BYTE *pSrc = (BYTE*)alloca(lenSrc);
	if(pSrc)
	{
		memcpy(pSrc, pBuffer, lenSrc);
		nLenUnzip = ungzip(pSrc, lenSrc, pBuffer, lenBuff);
	}

	return nLenUnzip;
}


/*
	str0: source string
	name: attrib name, like "Set-Cookie:", or "Server:"
*/
string CHttpBase::getHeadAttr(const char* str0, const char* name)
{
	if(NULL==str0 || NULL==name){
		return "";
	}
	
	const char* p;
	if(NULL==(p = strstr(str0, name))){
		return "";
	}

	// skip space
	p += strlen(name);
	if(*p != ' ')
		return "";

	string path;
	const char* start = ++p;
	const char* end = strstr(start, "\r\n");
	
	while(start && end && end > start)
	{
		char* buff = (char*)alloca(end-start+2);
		if(buff)
		{
			memcpy(buff, start, end-start);
			buff[end-start] = '\0';
			path += buff;
			//path += "; ";
		}

		if(NULL==(p= strstr(end + 1, name)))
			break;

		start	= p + strlen(name) + 1;
		end		= strstr(start, "\r\n");
	}

	return path;
}

string CHttpBase::GetRevHeaderOfBody()
{
	return m_strRevHeader;
}

string CHttpBase::GetSendHeader()
{
	return m_strSendHeader;
}

// Get http content
// parameters:
//		pBuffer: if type==file, this is path of dest file
//		llSize: pBuffer's size
// 返回: 文件大小
__int64 CHttpBase::fnGetBuffer(emHttpType type, HeadInfo* pHi, const char* method, const char* szHost0, const char* szUrl, 
	const char* szHead, const char* szUserData, const char* cookie, BYTE *pBuffer, __int64 llSize, BOOL bAutoRedir, WORD wPort/*=80*/)
{
	if(NULL==pHi || NULL==szHost0 || NULL==szUrl || NULL==method)
        return ((__int64)false);

	// Alloca memery
    char* recbuf	= NULL;
	int lenrecb		= 0;
	if(type==_http_type_header)
	{
		recbuf	= (char*)pBuffer;
		lenrecb	= llSize;
	}
	else
	{
		recbuf = (char*) alloca(C_RECVBUFF_LENGTH);
		if(recbuf)
		{
			memset(recbuf, 0, sizeof(char)*C_RECVBUFF_LENGTH);
			lenrecb = C_RECVBUFF_LENGTH;
		}
	}

	if(NULL==recbuf)
		return ((__int64)false);

	int socket1 = 0;
	int nLenFree = 0;

	// Connect
	string strHost = (NULL!=szHead) ? getHeadAttr(szHead, "Host:") : szHost0;
	do
	{
		ATLTRACE("connect...");
		socket1 = Connect(strHost.c_str(), wPort);
		if(socket1<1)
			return ((__int64)false);

		// Build header
		m_strSendHeader =  (NULL==szHead) ? BuildHeader(type, method, strHost.c_str(), szUrl, cookie, szUserData) : szHead;

		// Send
		ATLTRACE("Send head...");
		int nLenSend = Send(socket1, m_strSendHeader.c_str(), m_strSendHeader.length(), 0);
		if(nLenSend<0)
		{
			_tprintf(_T("Send error, %s, no:%d\n"), strerror(errno), errno);
			CloseSocket(socket1);
			return ((__int64)false);
		}

		// Receive header
		ATLTRACE("Receive head data...");
		nLenFree = recv_head_data(socket1, recbuf, lenrecb, pHi, type);
		if( nLenFree < 0 )
		{
			return ((__int64)false);
		}

		if(type==_http_type_header)
		{
			return pHi->lenHead;
		}

		// Copy header
		m_strRevHeader = "";
		ATLTRACE("Copy header...");
		if(pHi->lenHead > 0 && pHi->lenHead < lenrecb)
		{
			char* bufHead = (char*) alloca(sizeof(char) * (pHi->lenHead+1));
			if(bufHead)
			{
				memcpy(bufHead, recbuf, sizeof(char)*pHi->lenHead);
				bufHead[pHi->lenHead] = '\0';
				m_strRevHeader = bufHead;
			}
		}

		if(pHi->retCode==301 || pHi->retCode==302)
		{
			ATLTRACE("301 or 302...");
			if(bAutoRedir)// Redirect
			{
				closesocket(socket1);

				int lenHost, lenGet;
				if(GetHostUrl(pHi->Location, NULL, NULL, &lenHost, &lenGet))
				{
					char* szHost2 = (char*)alloca((lenHost+1)*sizeof(char));
					char* szUrl2  = (char*)alloca((lenGet+1)*sizeof(char));
					GetHostUrl(pHi->Location, szHost2, szUrl2, &lenHost, &lenGet);
					strHost		= szHost2;
					szUrl		= szUrl2;
					continue;
				}
			}
		}

		if(200!=pHi->retCode)
			return ((__int64)false);
		break;
	}while(bAutoRedir);

	// Receive File
	if(type==_http_type_file)
	{
		return recv_file_data(socket1, (char*)pBuffer, recbuf, nLenFree, pHi);
	}

	// Receive body
	ATLTRACE("Receive body data(chuck:%d)...", pHi->bChucked);
	__int64 llBufPos	= 0;
	if(pHi->bChucked) // chucked
	{
		llBufPos = recv_chuck_data(socket1, recbuf, nLenFree, (char*)pBuffer, llSize, pHi);
	}
	else // content-length
	{
		llBufPos = recv_content_data(socket1, recbuf, nLenFree, (char*)pBuffer, llSize, pHi);
	}

	if(pHi->bGzip)
	{
		llBufPos = UnZip(pBuffer, (int)llSize, (int)llBufPos);
	}
    CloseSocket(socket1);

	// fix bug: Any zip data is greater than received length.
	if (llBufPos < llSize && llBufPos > 0)
	{
		pBuffer[llBufPos] = '\0'; 
	}

	ATLTRACE("Receive OK...");
	return llBufPos;
}


bool CHttpBase::GetHostUrl(const char* url0, char* host, char* get, int* lenHost, int* lenGet)
{
	if(!lenHost || !lenGet ||!url0)
		return false;

	int nLenUrl = strlen(url0);
	const char* p0 = strstr(url0, "://");
	if(!p0)
		return false;

	p0 += strlen("://");

	const char* p1 = p0;
	for (; *p1!='/' && *p1!='\0'; p1++)
		;
	int len = (p1-p0);

	if(!host && !(host-get))
	{
		*lenHost = len;
		*lenGet = nLenUrl-len;
		return true;
	}

	if(*host && *get && *lenHost>0 && *lenGet>0)
	{
		if(*p1=='/')
		{
			strncpy(host, p0, *lenHost);
			host[*lenHost] = '\0';
			strncpy(get, p1, *lenGet);
			get[*lenGet] = '\0';
		}
		else
		{
			strncpy(host, p0, len);
			host[len] = '\0';
			strcpy(get, "/");
		}
		return true;
	}
	return false;
}

int CHttpBase::recv_content_data(int socket1, char* recbuf, int nLeftBytes, char* pBuffer, __int64 llSize, HeadInfo* pHi)
{
	if(NULL==recbuf)
		return -1;
	
	__int64 llBufPos = 0;
	int nStartRevBuf	= nLeftBytes;
	if(pHi->pBodyStart && nStartRevBuf>0 && nStartRevBuf<llSize)
	{
		memmove((pBuffer)+llBufPos, pHi->pBodyStart, nStartRevBuf);
		llBufPos += nStartRevBuf;
	}
	
	int revn			= 0;
	
	//ATLTRACE("llBufPos:%lld", llBufPos);
	while( llBufPos <pHi->lenContent && 
		(revn=Recv(socket1, recbuf, C_RECVBUFF_LENGTH, 0))>0 &&
		llBufPos+revn < llSize)
	{
		memcpy((pBuffer)+llBufPos, recbuf, revn);
		llBufPos += revn;
		
		if(pHi->ver<1.1 && !pHi->bChucked && pHi->lenContent<1){
			continue;
		}
	}

	return llBufPos;
}



int CHttpBase::recv_chuck_data(int socket1, char* recbuf, int nLeftBytes, char* pBuffer, __int64 llSize, HeadInfo* pHi)
{
	if(NULL==recbuf)
		return -1;

	__int64 llBufPos = 0;
	int chucklen		= 0;
	int l_total			= nLeftBytes;
	int l_copied		= 0;
	int nStartRevBuf	= 0;
	bool bStartNew		= true;
			
	if(pHi->pBodyStart && l_total>=0 && l_total<llSize)
	{
		chucklen = CopyChuckBuffer(pBuffer, llSize, llBufPos, 
			pHi->pBodyStart, l_total, chucklen, l_copied, bStartNew);
		if(chucklen<0)
		{
			nStartRevBuf = -1*chucklen;
			memmove(((char*)recbuf), pHi->pBodyStart+l_total-nStartRevBuf, nStartRevBuf);
		}
		else
		{
			nStartRevBuf = 0;
		}
		//_tprintf(_T("Loop,chucklen:%d, hi.length:%lld\n"), chucklen, hi.length);
	}

	int revn;
	while((0!=chucklen || (0==chucklen && 0==llBufPos) ) &&
		(revn=Recv(socket1, recbuf+nStartRevBuf, C_RECVBUFF_LENGTH-nStartRevBuf, 0))>0 &&
		llBufPos+revn < llSize)
	{
		chucklen = CopyChuckBuffer(pBuffer, llSize, llBufPos, 
			recbuf, revn, chucklen, l_copied, bStartNew);
		if(chucklen<0)
		{
			nStartRevBuf = -1*chucklen;
			memmove(((char*)recbuf), ((char*)recbuf)+revn-nStartRevBuf, nStartRevBuf);
			continue;
		}

		//_tprintf(_T("Loop,chucklen:%d, hi.length:%lld, revn:%d\n"), chucklen, hi.length, revn);
		nStartRevBuf	= 0;
		//if(0==chucklen)
		//	break;
	}
	return llBufPos;
}



// Copy chuck data
// Parameters:
//	pOutbuff:		目标缓冲区
//	llOutBuffSize:	目标缓冲区大小
//	llOutBuffPos:	目标缓冲区数据指针
//	src:			源数据
//	lenSrc:			源数据长度
//	l_chucksize:	last chucksize, 上一个chuck块的长度
//	l_copied:		last copied size, 上一个chuck块已复制到目标缓冲区的长度. 
//	bStartNew:		start new chuck
//
// Return:	chucked length
//				0:	last chuck
//				-1: less than minichuck size
//				>0: chuck size
// 注：不要删除 printf 的注释，调试程序必须的
//
int CHttpBase::CopyChuckBuffer(char *pOutbuff, __int64 llOutBuffSize, __int64& llOutBuffPos, 
							  const char *src, int lenSrc, 
							  int l_chucksize, int& l_copied, bool& bStartNew)
{
	if(NULL==pOutbuff || NULL==src)
		return 0;
	if(lenSrc<(int)(3*sizeof(char)))
		return (-1*lenSrc);

	int l_left		= lenSrc;
	int chuckLen	= l_chucksize;
	if(lenSrc>0 && lenSrc<llOutBuffSize-llOutBuffPos)
	{
		int pos		= lenSrc-l_left;
		if(bStartNew)
		{
			bStartNew	= false;
			pos			= ReadCRLF(src, pos, lenSrc);
			chuckLen	= strtol(src + pos, NULL, 16); //hextoi(src + pos);//strtol()
			pos			= SkipChuck(src, pos, lenSrc);
			//_tprintf(_T("---Start, new chuck size:%d, srcLen:%d, pos:%d----\n"), chuckLen, lenSrc, pos);
		}
		for(; pos<lenSrc && chuckLen>0; )
		{
			l_left				= lenSrc - pos;
			const int nLeft		= chuckLen-l_copied;
			int copyLen			= (nLeft>l_left) ? l_left : nLeft;
			memcpy(pOutbuff + llOutBuffPos, src + pos, copyLen);

			llOutBuffPos	+= copyLen;
			pos				+= copyLen;		//already copied.
			l_copied		+= copyLen;		//Length of last chucked copied.
			//_tprintf(_T("\tCopied %d bytes, \tsrcLen:%d\tpos:%d\n"), copyLen, lenSrc, pos);

			if(l_copied==chuckLen) //filished one chuck data.
			{
				//_tprintf(_T("\t\tl_copied %d bytes\n"), l_copied);
				l_copied = 0;
				int pos2 = ReadCRLF(src, pos, lenSrc);
				if(pos==pos2) // End chuck
				{
					bStartNew = true;
					//_tprintf(_T("\tEnd Flag, Not found CRLF.\n"));
					break;
				}

				if(lenSrc == pos2) // OK, next continue
				{
					bStartNew = true;
					//_tprintf(_T("\tCopied end, lenSrc:%d bytes, pos: %d\n"), lenSrc, pos2);
					break;
				}

				if(lenSrc - pos2 < (int)strlen("xxxx\r")) // Not any data to ansl, xxxx\r\n or 0\r\n\r\n
				{
					_tprintf(_T("Not any data, chuck size:%d, lenSrc:%d, Pos:%d.\n"), chuckLen, lenSrc, pos);
					return (-1*(lenSrc-pos));
				}

				pos = pos2;
				chuckLen = strtol(src + pos, NULL, 16); //hextoi(src + pos);
				pos	= SkipChuck(src, pos, lenSrc);
				//_tprintf(_T("Continue, new chuck size:%d, lenSrc:%d, Pos:%d.\n"), chuckLen, lenSrc, pos);
				bStartNew = false;
			}
		}
	}
	else
	{
		_tprintf(_T("error, chuckLen:%d, l_copied:%d\n"), chuckLen, l_copied);
	}
	return chuckLen;
}

int CHttpBase::SkipChuck(const char *src, int pos, int lenSrc) const
{
	for(int i=pos; i<lenSrc-1; i++, pos++)
	{
		if(*(src+i)=='\r' && *(src+i+1)=='\n')
		{
			pos += 2;
			break;
		}
	}
	
	return pos;
}

int CHttpBase::ReadCRLF(const char *src, int pos, int lenSrc) const
{
	if((lenSrc-pos)>=2)
	{
		if(*(src+pos+0)=='\r' && *(src+pos+1)=='\n')
		{
			pos += 2;
		}
	}
	
	return pos;
}


__int64 CHttpBase::recv_file_data(int socket1, const char* szFilePath, char* recbuf, int nLeftBytes, HeadInfo* pHi)
{
	if(NULL==recbuf || !pHi)
		return -1;

	const char* chRdType = pHi->bGzip ? "wb+" : "wb";
	FILE* fp = fopen(szFilePath, chRdType);
	if(NULL==fp)
	{
		CloseSocket(socket1);
		_tprintf(_T("Error, Open file failse.\n"));
		return ((__int64)false);
	}

	__int64 llBufPos = 0;
	int nLenZipMax = C_RECVBUFF_LENGTH * 50;
	if(pHi->bChucked)
	{
		__int64 llSize = nLenZipMax;
		char* pBuffer = new char[llSize];
		if(pBuffer)
		{
			llBufPos = recv_chuck_data(socket1, recbuf, nLeftBytes, pBuffer, llSize, pHi);
			if(llBufPos>0)
			{
				int nLenZip = (pHi->bGzip ? UnZip((BYTE*)pBuffer, llSize, llBufPos) : 0);
				fwrite((BYTE*)pBuffer, nLenZip>0 ? nLenZip : llBufPos, 1, fp);
			}
			delete[] pBuffer;
		}
	}
	else
	{
		if(nLeftBytes>0) // BUG; the head is not include xxxx\r\n
		{	
			fwrite((BYTE*)pHi->pBodyStart, nLeftBytes, 1, fp);
			llBufPos += nLeftBytes;
		}

		int revn = 0;
		while(llBufPos < pHi->lenContent && 
			(revn=Recv(socket1, recbuf, C_RECVBUFF_LENGTH, 0))>0 )
		{
			fwrite((BYTE*)recbuf, revn, 1, fp);
			llBufPos += revn;
		}

		if(pHi->bGzip && llBufPos>0 && llBufPos<nLenZipMax/5)
		{
			char* pBuffer = new char[nLenZipMax];
			if(pBuffer)
			{
				fseek(fp, 0, SEEK_SET);
				fread(pBuffer, 1, llBufPos, fp);
				int nLenZip = UnZip((BYTE*)pBuffer, nLenZipMax, llBufPos);
				fseek(fp, 0, SEEK_SET);
				fwrite((BYTE*)pBuffer, nLenZip>0 ? nLenZip : llBufPos, 1, fp);
				delete[] pBuffer;
			}
		}
	}
	fclose(fp);

	return llBufPos;
}


int CHttpBase::recv_head_data(int socket1, char* recbuf, int bufsize, HeadInfo* pHi, emHttpType type)
{
	int revn			= 0;
	int pos				= 0;
	int bFound			= false;
	while( pos < bufsize &&
				(revn=Recv(socket1, recbuf+pos, bufsize-pos-1, 0))>0)
	{
		pos	 += revn;
		recbuf[pos] = '\0';
		if(GetHttpHeadInfo(recbuf, pHi))
		{
			bFound = true;
			break;
		}
    }

	if(!bFound)
	  return -1;

	return (pos - pHi->lenHead);
}

// 获取头
//	return 0: ok, other failse;
int CHttpBase::getRevHeader(const char* szHost, const char* szUrl, const char* szHead, BYTE *pBuffer, DWORD dwSize, WORD dwPort)
{
	HeadInfo hi;
	if(pBuffer)
	{
		int len = fnGetBuffer(_http_type_header, &hi, "HEAD", szHost, szUrl, szHead, NULL, NULL, pBuffer, 
			(const __int64)dwSize, FALSE, dwPort);
		return 0;
	}

	return -1;
}


int CHttpBase::HttpAjax(const char* szHost, const char* szMethod, const char* szUrl, 
					   const char* szHead, const char* szUserData, const char* cookie,
					   BYTE *pBuffer, DWORD dwSize, WORD wPort/*=80*/, BOOL bAutoRedir/*=FALSE*/)
{
	HeadInfo hi; 
	return (int)fnGetBuffer(_http_type_body, &hi, szMethod, szHost, szUrl, 
		szHead, szUserData, cookie,
		pBuffer, (const __int64)dwSize, bAutoRedir, wPort);
}

