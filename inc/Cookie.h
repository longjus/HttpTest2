// Cookie.h: interface for the CCookie class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COOKIE_H__667D4B81_C8D5_4B53_AE53_1652DA0068C3__INCLUDED_)
#define AFX_COOKIE_H__667D4B81_C8D5_4B53_AE53_1652DA0068C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <string>
#include <vector>
using namespace std;

//Set-Cookie: PSTM=1479103688; expires=Thu, 31-Dec-37 23:55:55 GMT; max-age=2147483647; path=/; domain=.baidu.com
typedef struct tagCOOKIE
{
	char path[50];
	__int64 expires;
	int max_age;
	char domain[MAX_PATH];
	char value[MAX_PATH];
}COOKIE;

class CCookie  
{
public:
	void Clear();
	string getCookie();
	void push(const char* str0);
	CCookie();
	virtual ~CCookie();
	map<string, COOKIE*> m_lstCookies; // <NAME, VALUE>
protected:
	void split_attr(char* str);
};

#endif // !defined(AFX_COOKIE_H__667D4B81_C8D5_4B53_AE53_1652DA0068C3__INCLUDED_)
