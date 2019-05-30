// Cookie.cpp: implementation of the CCookie class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Cookie.h"
#include <time.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCookie::CCookie()
{

}

CCookie::~CCookie()
{
	Clear();
}

void CCookie::push(const char *str0)
{
	if(NULL==str0){
		return;
	}
	
	const char* p;
	const char* name = "Set-Cookie:";
	if(NULL==(p = strstr(str0, name))){
		return;
	}

	// skip space
	p += strlen(name);
	if(*p != ' ')
		return;

	const char* start = ++p;
	const char* end = strstr(start, "\r\n");
	
	while(start && end && end > start)
	{
		char* buff = (char*)alloca(end-start+1);
		if(buff)
		{
			memcpy(buff, start, end-start+1);
			buff[end-start+0] = '\0';
			
			for(int i=0; i<end-start+0; i++)
			{
				if(buff[i] == ';')
				{
					buff[i] = '\0';
					break;
				}
			}
			
			split_attr(buff);
		}

		if(NULL==(p= strstr(end + 1, name)))
			break;

		start	= p + strlen(name) + 1;
		end		= strstr(start, "\r\n");
	}
}

time_t ToTimeStamp(string strTime)  
{  
    tm _tm;  
    int year, month, day, hour, minute,second;  
  
    sscanf(strTime.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);  
    _tm.tm_year  = year - 1900;  
    _tm.tm_mon   = month - 1;  
    _tm.tm_mday  = day;  
    _tm.tm_hour  = hour;  
    _tm.tm_min   = minute;  
    _tm.tm_sec   = second;  
    _tm.tm_isdst = 0;  
    time_t t = mktime(&_tm);  
  
    return t;  
} 

/*
Set-Cookie: PSTM=1479103688; expires=Thu, 31-Dec-37 23:55:55 GMT; max-age=2147483647; path=/; domain=.baidu.com
Set-Cookie: BDSVRTM=0; path=/
Set-Cookie: BD_HOME=0; path=/
*/
void CCookie::split_attr(char *str)
{
	char* p0 = str;
	char* p = p0;
	vector<string> lstAtt;
	for(; *p!='\0'; p++)
	{
		if(*p == ';')
		{
			*p = '\0';
			lstAtt.push_back(p0);

			p0 = p + 1;
			if(*p0 == ' ')
				p0++;
		}
	}
	
	if(*p == '\0' && p0!=p)
		lstAtt.push_back(p0);

	//
	vector<string>::iterator it = lstAtt.begin();
	for(; it!=lstAtt.end(); it++)
	{
		const char* pName = NULL;
		p = (char*)alloca(it->length()+1);
		if(p)
		{
			COOKIE* pci = (COOKIE*)malloc(sizeof(COOKIE));
			if(NULL==pci)
				continue;

			memcpy(p, it->c_str(), it->length());
			p[it->length()] = '\0';
			p0 = p;
			for(; *p!='\0'; p++)
			{
				if(*p=='=')
				{
					*p = '\0';
					p++;

					if(0==stricmp(p0, "expires"))
					{
						pci->expires = ToTimeStamp(p);//
					}
					else if(0==stricmp(p0, "max-age"))
					{
						pci->max_age = atoi(p);					
					}
					else if(0==stricmp(p0, "path"))
					{
						strncpy(pci->path, p, 50);
						pci->domain[50-1] = '\0';
					}
					else if(0==stricmp(p0, "domain"))
					{
						strncpy(pci->domain, p, MAX_PATH);
						pci->domain[MAX_PATH-1] = '\0';
					}
					else
					{
						pName = p0;
						strncpy(pci->value, p, MAX_PATH);
						pci->value[MAX_PATH-1] = '\0';
					}

					break;
				}
			}

			if(NULL!=pName)
			{
				map<string, COOKIE*>::iterator itM = m_lstCookies.find(pName);
				if(itM!=m_lstCookies.end())
				{
					free(itM->second);
					itM->second = pci;
				}
				else
				{
					m_lstCookies.insert(pair<string, COOKIE*>(pName, pci));
				}
			}
			else
			{
				free(pci);
			}
		}
	}

}

string CCookie::getCookie()
{
	string str;
	map<string, COOKIE*>::iterator itM = m_lstCookies.begin();
	for(; itM!=m_lstCookies.end(); itM++)
	{
		str += itM->first;
		str += "=";
		str += itM->second->value;
		str += "; ";
	}
	return str;
}

void CCookie::Clear()
{
	map<string, COOKIE*>::iterator it = m_lstCookies.begin();
	for(; it!=m_lstCookies.end(); it++)
	{
		if(it->second)
		{
			free(it->second);
		}
	}
	m_lstCookies.clear();
}
