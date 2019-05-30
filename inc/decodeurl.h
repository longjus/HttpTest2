#if !defined(_DECODEURL__INCLUDED_)
#define _DECODEURL__INCLUDED_
#pragma once

#include <string>
using namespace std;
#include "xchar.h"

#ifdef WIN32
	#include <Shlwapi.h>	//StrStrI
	#pragma comment(lib, "shlwapi")
	#define strstri		StrStrI
#else
	#include <iconv.h>
	#include <errno.h>
#endif	

// _istalnum 的自己版本， _istalnum 工作不正常原因不明， 故自己写个
static bool Istalnum(TCHAR const c)
{
	return (('a' <= c &&  c <= 'z') || ('0' <= c && c <= '9') || ('A' <= c && c<='Z') );
}

static inline bool is_base64(TCHAR const c) 
{
	return (Istalnum(c) || (c == '+') || (c == '/'));
}

static const t_string base64_chars = 
_T("ABCDEFGHIJKLMNOPQRSTUVWXYZ")
_T("abcdefghijklmnopqrstuvwxyz")
_T("0123456789+/");

// 注意： Unicode編碼後，不存在unsigned TCHAR 這種類型
static t_string base64_decode(const TCHAR * encoded_string, unsigned int in_len)
{
	int i = 0;
	int j = 0;
	int in_ = 0;
	int char_array_4[4], char_array_3[3];
	t_string ret;

	while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i ==4) {
			for (i = 0; i <4; i++)
				char_array_4[i] = (int)base64_chars.find(char_array_4[i]);////????

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++)
			char_array_4[j] = (int)base64_chars.find(char_array_4[j]);///////////////????

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}

	return ret;
}

static t_string DecodeBase64(const TCHAR* lpsz)
{
	if (!lpsz)
		return t_string(_T(""));

	t_string str = base64_decode(lpsz, (unsigned int) _tcslen((TCHAR*)lpsz));

	return str;
}

/*
 * 先要找到一个wchar_t的strcasestr, strstri才能用本函数
static t_string Decode3URL(LPCTSTR url)
{
	t_string tmp;

	LPCTSTR p;
	if (	( p = strstri(url,_T("thunder://")) )	)
	{
		p = p+_tcslen(_T("thunder://"));
		tmp = DecodeBase64(p);

		if (tmp.length() > 4 && 0==tmp.compare(0, 2, _T("AA")) &&
		 0==tmp.compare(tmp.length()-2, 2, _T("ZZ")) )
		{
			tmp = tmp.substr(2,tmp.length() - 4);
		}
	}
	else if ((p = strstri(url,_T("Flashget://"))))
	{
		p = p+_tcslen(_T("Flashget://"));
		tmp = DecodeBase64(p);

		if (tmp.length() > 20 && 0==tmp.compare(0, 10, _T("[FLASHGET]")) &&
		 0==tmp.compare(tmp.length()-10, 10, _T("[FLASHGET]")) )
		{
			tmp = tmp.substr(10, tmp.length() - 20);
		}
	}
	else if ((p = strstri(url,_T("qqdl://"))))
	{
		p = p+_tcslen(_T("qqdl://"));
		tmp = DecodeBase64(p);
	}
	return tmp;
}

*/
///////////////////////////////////////////////////////////////////////////////
//  下面是编解码
//
#ifndef WIN32
// 这个也是可以的，不过参数复杂些
static size_t code_convert(const char *from_charset, const char *to_charset, 
		const char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	char **pin = (char**)&inbuf; //pin的内容会改变，这样做是防止inbuf被改变
	char **pout = &outbuf; //这是防止outbuf值改变，iconv在此处仅修改了pout的内容
	memset(outbuf, 0, outlen);

	//iconv_t iconv_open(const char *tocode, const char *fromcode);
	iconv_t cd = iconv_open(to_charset,from_charset);
	if((iconv_t)-1 == cd) 
	{
		 perror("iconv_open error\n");
		 return -1;
	}
	
	char* pin0 = (char*) inbuf;
	size_t len = outlen;
	size_t retsize = iconv(cd, pin, &inlen, pout, &outlen);
	if((size_t)-1 == retsize) 
	{
		 perror("iconv error\n");
		 len = 0;
	}
	iconv_close(cd);

	return (len-outlen);
}

// 这个调入较少的参数
static size_t code_convert2(char** sout, char *sin, char *encTo, char *encFrom)
{
	iconv_t c_pt;
	size_t inbytesleft, outbytesleft, ret;
	if ((c_pt = iconv_open(encTo, encFrom)) == (iconv_t)-1) {
		fprintf(stdout, "iconv_open false: %s => %s\n", encFrom, encTo);
		return -1;
	}
	//iconv(c_pt, NULL, NULL, NULL, NULL);

	inbytesleft  = strlen(sin);
	outbytesleft = inbytesleft*4 + sizeof(wchar_t);
	size_t len = outbytesleft;
	*sout = new char[outbytesleft];
	if(!*sout)
		return -1;
	char* sout0 = *sout;//保存缓冲首址, iconv会改变这个值
	memset(*sout, 0, outbytesleft);
	ret = iconv(c_pt, &sin, &inbytesleft, sout, &outbytesleft);
	if (ret == -1) {
		fprintf(stdout, "error ret:%d, errno:%d, iconv stoped. from:%s=>%s, len:%d,outbytesleft:%d\n", 
				ret, errno, encFrom, encTo, len, outbytesleft);
		//len = 0;
		//delete[] sout0;
		//sout0 = NULL;
	}
	iconv_close(c_pt);
	*sout = sout0; //恢复缓冲地址，让调用函数可以引用
	return (len-outbytesleft);
}
#endif

// ansi -> utf8, 
static size_t AnsiToUTF8(std::string& strRet, const CHAR* str)
{
#ifdef WIN32
   wchar_t *strSrc;
	CHAR *szRes;

	int i = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	if(i>0)
	{
		strSrc = new wchar_t[i];
		MultiByteToWideChar(CP_ACP, 0, str, -1, strSrc, i);
		i = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, NULL, 0, NULL, NULL);
		if(i>0)
		{
			szRes = new CHAR[i];
			int j=WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, szRes, i, NULL, NULL);
			strRet = szRes;
			delete []szRes;
		}
		delete []strSrc;
	}
   return i;
#else
	size_t inlen 	= strlen(str);
	size_t outlen 	= inlen * 4 + 1;
	char* pout 		= new char[outlen];
	
	size_t len = code_convert("GB18030", "UTF-8", str, inlen, pout, outlen);
	if(len>(size_t)0)
	{
		strRet = pout;
	}
	delete[] pout;	
	
	return len;
#endif   
}


// utf8 -> ansi, 
static size_t UTF8ToAnsi(string& result, const CHAR* str)
{
#ifdef WIN32
	wchar_t *strSrc;
	CHAR *szRes;

	int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	if(i>0)
	{
		strSrc = new wchar_t[i];
		MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);

		i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
		if(i>0)
		{
			szRes = new CHAR[i];
			WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

			result = szRes;
			delete []szRes;
		}
		delete []strSrc;
	}
	return i;
#else
	size_t inlen 	= strlen(str);
	size_t outlen 	= inlen * 4 + 1;
	char* pout 		= new char[outlen];
	
	size_t len = code_convert("UTF-8", "GB18030", str, inlen, pout, outlen);
	if(len>(size_t)0)
	{
		result = pout;
	}
	delete[] pout;	
	
	return len;
#endif 	
} 

//////////////////////////////////////////////////////////////////////
// Unicode relation
// UTF-8 -> Unicode
static int UTF8ToUnicode(wstring& strOut, const CHAR* strIn)
{
#ifdef WIN32	
	int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, strIn, -1, NULL, 0);
	if(wcsLen>0)
	{
		wchar_t* wszString = new wchar_t[wcsLen];
		wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, strIn, -1, wszString, wcsLen);
		//    wszString[wcsLen-1] = '\0';

		strOut = wszString;
		delete[] wszString;
	}

	return wcsLen;
#else
	char* pout 		= NULL;
	size_t len = code_convert2(&pout, (CHAR*)strIn, "ucs-4le", "utf-8");
	if(len>(size_t)0 && pout)
	{
		strOut = (wchar_t*)pout;
		delete[] pout;
	}
	
	return len;
#endif
}

// Ansi -> Unicode
static int AnsiToUnicode(wstring& strOut, const CHAR* strIn)
{
#ifdef WIN32	
	int wcsLen = ::MultiByteToWideChar(CP_ACP, NULL, strIn, -1, NULL, 0);
	if(wcsLen>0)
	{
		wchar_t* wszString = new wchar_t[wcsLen];
		wcsLen = ::MultiByteToWideChar(CP_ACP, NULL, strIn, -1, wszString, wcsLen);
		//    wszString[wcsLen-1] = '\0';

		strOut = wszString;
		delete[] wszString;
	}

	return wcsLen;
#else
	char* pout 		= NULL;
	size_t len = code_convert2(&pout, (CHAR*)strIn, "ucs-4le", "gb18030");
	if(len>(size_t)0 && pout)
	{
		strOut = (wchar_t*)pout;
		delete[] pout;
	}
	
	return len;
#endif
}

// Unicode -> UTF-8 
//		注： UTF-8 和 ANSI 都是字節流， 不能與Unicode等同
static int UnicodeToUTF8(string& szOut, const wchar_t* strIn)
{
#ifdef WIN32
	int u8Len = ::WideCharToMultiByte(CP_UTF8, NULL, strIn, (int)wcslen(strIn), NULL, 0, NULL, NULL);
	if(u8Len>0)
	{
		CHAR* szU8 = new CHAR[u8Len + 1];
		u8Len = ::WideCharToMultiByte(CP_UTF8, NULL, strIn, (int)wcslen(strIn), szU8, u8Len, NULL, NULL);
		szU8[u8Len] = '\0';
		szOut = szU8;
		delete[] szU8;
	}
	return u8Len;
#else
	size_t inlen = wcslen(strIn) * sizeof(wchar_t);
	size_t outlen 	= inlen;
	char* pout 		= new char[outlen];
	
	size_t len = code_convert("ucs-4le", "utf-8", (CHAR*)strIn, inlen, pout, outlen);
	if(len>(size_t)0)
	{
		szOut = pout;
	}
	delete[] pout;	

	return len;
#endif
}

// Unicode -> Ansi 
static int UnicodeToAnsi(string& szOut, const wchar_t* strIn)
{
#ifdef WIN32
	int u8Len = ::WideCharToMultiByte(CP_ACP, NULL, strIn, (int)wcslen(strIn), NULL, 0, NULL, NULL);
	if(u8Len>0)
	{
		CHAR* szU8 = new CHAR[u8Len + 1];
		u8Len = ::WideCharToMultiByte(CP_ACP, NULL, strIn, (int)wcslen(strIn), szU8, u8Len, NULL, NULL);
		szU8[u8Len] = '\0';
		szOut = szU8;
		delete[] szU8;
	}
	return u8Len;
#else
	size_t inlen = wcslen(strIn) * sizeof(wchar_t);
	size_t outlen 	= inlen;
	char* pout 		= new char[outlen];
	
	size_t len = code_convert("ucs-4le", "gb18030", (CHAR*)strIn, inlen, pout, outlen);
	if(len>(size_t)0)
	{
		szOut = pout;
	}
	delete[] pout;	

	return len;
#endif 
}
// End Unicode relation
//////////////////////////////////////////////////////////////////////

// UrlEncode_Base
static std::string UrlEncode_Base(const std::string& src)   
{   
    static    char hex[] = "0123456789ABCDEF";   
    std::string dst;   
  
    for (size_t i = 0; i < src.size(); i++)   
    {   
        const unsigned char ch = src[i];   
		if ('.'==ch || '/'==ch || ':'==ch || isalnum(ch)) // . / : 數字 字母 不轉
        {   
			dst += ch;   
		}   
		else if (src[i] == ' ')   
		{   
			dst += '+';   
		}   
		else  
		{   
			dst += '%';   
			dst += hex[ch >> 4];   
			dst += hex[ch % 16];   
		}   
	}   
	return dst;   
}   

// UrlEncode_Ansi
static t_string UrlEncode_Ansi(const t_string& strUrl0)   
{   
	string strUrl;
	t_string tszUrl;
#ifdef UNICODE
	UnicodeToAnsi(strUrl, strUrl0.c_str());
	strUrl = UrlEncode_Base(strUrl);
	AnsiToUnicode(tszUrl, strUrl.c_str());
#else
	strUrl = UrlEncode_Base(strUrl0);
	tszUrl = strUrl;
#endif
	return tszUrl;
}

// UrlEncode_UTF8
static t_string UrlEncode_UTF8(const t_string& strUrl0)
{
	string strUrl;
	t_string tszUrl;
#ifdef UNICODE
	UnicodeToUTF8(strUrl, strUrl0.c_str());
	strUrl = UrlEncode_Base(strUrl);
	UTF8ToUnicode(tszUrl, strUrl.c_str());
#else
	AnsiToUTF8(strUrl, strUrl0.c_str());
	strUrl = UrlEncode_Base(strUrl);
	tszUrl = strUrl;
#endif
	return tszUrl;
}


static int IsTextUtf8(const char* str,long length)
{
	int i;
	int nBytes=0;//UFT8可用1-6个字节编码,ASCII用一个字节
	unsigned char chr;
	bool bAllAscii=true; //如果全部都是ASCII, 说明不是UTF-8
	for(i=0;i<length;i++)
	{
		chr= *(str+i);
		if( (chr&0x80) != 0 ) // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
			bAllAscii= false;
		if(nBytes==0) //如果不是ASCII码,应该是多字节符,计算字节数
		{
			if(chr>=0x80)
			{
				if(chr>=0xFC&&chr<=0xFD)
					nBytes=6;
				else if(chr>=0xF8)
					nBytes=5;
				else if(chr>=0xF0)
					nBytes=4;
				else if(chr>=0xE0)
					nBytes=3;
				else if(chr>=0xC0)
					nBytes=2;
				else
				{
					return false;
				}
				nBytes--;
			}
		}
		else //多字节符的非首字节,应为 10xxxxxx
		{
			if( (chr&0xC0) != 0x80 )
			{
				return false;
			}
			nBytes--;
		}
	}
	
	if( nBytes > 0 ) //违返规则
	{
		return false;
	}
	
	if( bAllAscii ) //如果全部都是ASCII, 说明不是UTF-8
	{
		return false;
	}
	return true;
}

#endif
