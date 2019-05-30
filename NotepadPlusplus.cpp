// NotepadPlusplus.cpp: implementation of the CNotepadPlusplus class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NotepadPlusplus.h"
#include <windows.h>
#include "inc/tinyxml/tinyxml.h"
#include "resource.h"
#include <vector>
using namespace std;
#include <math.h>


#define				MARGIN_SCRIPT_NUMBER_INDEX  0
#define				MARGIN_SCRIPT_FOLD_INDEX	1
#define				execute						SendEditor
//#define				WINDOW_ID					900
#define	STRNCPY(dest, src, num) {strncpy(dest, src, num-1); dest[num-1] = '\0';}
#define				C_MAXKEY					8

// color
static const COLORREF black   = 0x000000;
static const COLORREF white   = 0xFFFFFF;
static const COLORREF blue    = RGB(0, 0, 0xFF);
static const COLORREF purple  = RGB(0xFF, 0, 0xFF);
static const COLORREF orange  = RGB(0xFF, 128, 0);

static const int SCE_STYLE_BLACK  = 10;
static const int SCE_STYLE_ORANGE = 11;
static const int SCE_STYLE_PURPLE = 12;
static const int SCE_STYLE_BLUE   = 13;

// style
const int FONTSTYLE_NONE = 0;
const int FONTSTYLE_BOLD = 1;
const int FONTSTYLE_ITALIC = 2;
const int FONTSTYLE_UNDERLINE = 4;

typedef int (__cdecl* LP_fnSciCB)(void *,int,int,int);
typedef sptr_t Document;
enum LangType {L_TEXT, L_PHP , L_C, L_CPP, L_CS, L_OBJC, L_JAVA, L_RC,\
			   L_HTML, L_XML, L_MAKEFILE, L_PASCAL, L_BATCH, L_INI, L_ASCII, L_USER,\
			   L_ASP, L_SQL, L_VB, L_JS, L_CSS, L_PERL, L_PYTHON, L_LUA,\
			   L_TEX, L_FORTRAN, L_BASH, L_FLASH, L_NSIS, L_TCL, L_LISP, L_SCHEME,\
			   L_ASM, L_DIFF, L_PROPS, L_PS, L_RUBY, L_SMALLTALK, L_VHDL, L_KIX, L_AU3,\
			   L_CAML, L_ADA, L_VERILOG, L_MATLAB, L_HASKELL, L_INNO, L_SEARCHRESULT,\
			   L_CMAKE, L_YAML, L_COBOL, L_GUI4CLI, L_D, L_POWERSHELL, L_R, L_JSP,\
			   L_COFFEESCRIPT,\
			   // The end of enumated language type, so it should be always at the end
			   L_EXTERNAL};

TCHAR g_workdir[MAX_PATH];						// work dir
LP_fnSciCB g_fn				= NULL;
void* g_ptr					= NULL;

LangType		m_Lang				= L_TEXT;
int				m_curLineWidth		= 0;
int				m_nCurPos			= -1;
int				m_nVisibleLine		= -1;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNotepadPlusplus::CNotepadPlusplus()
{

}

CNotepadPlusplus::~CNotepadPlusplus()
{

}

void CNotepadPlusplus::Init(HINSTANCE hInst, HWND hWndMain, int ctlId)
{
	HINSTANCE hmod = NULL;
	hmod = LoadLibrary("SciLexer.DLL");
	if (hmod==NULL)
	{
		MessageBox(hWndMain,
			"The Scintilla DLL could not be loaded.",
			"Error loading Scintilla",
			MB_OK | MB_ICONERROR);
		return;
	}

	GetModuleFileName(NULL, g_workdir, MAX_PATH);
	for(TCHAR* p=g_workdir+_tcslen(g_workdir)-1; p>g_workdir; p--)
	{
		if(*p=='\\' || *p=='/')
		{
			*p = '\0';
			break;
		}
	}

	// 创建窗口
	RECT rc;
	GetClientRect(hWndMain, &rc);
	m_hWnd = CreateWindowEx(WS_EX_ACCEPTFILES | WS_EX_CLIENTEDGE,
		"Scintilla","", 
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN,
		0, 0, rc.right, rc.bottom, hWndMain, (HMENU)ctlId, hInst, NULL);

	//EnableDropfile(m_hWnd);

	// 设置属性
	g_fn = (int (__cdecl *)(void *,int,int,int))SendMessage(m_hWnd,SCI_GETDIRECTFUNCTION,0,0);
	g_ptr = (void *)SendMessage(m_hWnd,SCI_GETDIRECTPOINTER,0,0);

	ReloadStyles("html");
}

int SendEditor(int cmd, int wparam=0, int lparam=0)
{
	return g_fn(g_ptr, cmd, wparam, lparam);
}

LangType InitLanguageType(const TCHAR* language)
{
	LangType type = L_TEXT;
	int nType = SCLEX_NULL;
	if(0==_tcsicmp(language, _T("html"))){
		type  = L_HTML;
		nType = SCLEX_HTML;
	}else if(0==_tcsicmp(language, _T("php"))){
		type  = L_PHP;
		nType = SCLEX_HTML;
	}else if(0==_tcsicmp(language, _T("asp"))){
		type  = L_ASP;
		nType = SCLEX_HTML;
	}else if(0==_tcsicmp(language, _T("jsp"))){
		type  = L_JSP;
		nType = SCLEX_HTML;
	}else if(0==_tcsicmp(language, _T("xml"))){
		type  = L_XML;
		nType = SCLEX_XML;
	}else if(0==_tcsicmp(language, _T("css"))){
		type  = L_CSS;
		nType = SCLEX_CSS;
	}else if(0==_tcsicmp(language, _T("javascript"))){
		type  = L_JS;
		nType = SCLEX_CPP;
	}else if(0==_tcsicmp(language, _T("c"))){
		type  = L_C;
		nType = SCLEX_CPP;
	}else if(0==_tcsicmp(language, _T("cpp"))){
		type  = L_CPP;
		nType = SCLEX_CPP;
	}else if(0==_tcsicmp(language, _T("cs"))){// c#
		type  = L_CS;
		nType = SCLEX_CPP;
	}else if(0==_tcsicmp(language, _T("cmake"))){
		type  = L_CMAKE;
		nType = SCLEX_CMAKE;
	}else if(0==_tcsicmp(language, _T("cobol"))){
		type  = L_COBOL;
		nType = SCLEX_COBOL;
	}else if(0==_tcsicmp(language, _T("objc"))){
		type  = L_OBJC;
		nType = SCLEX_CPP;
	}else if(0==_tcsicmp(language, _T("java"))){
		type  = L_JAVA;
		nType = SCLEX_CPP;
	}else if(0==_tcsicmp(language, _T("matlab"))){
		type  = L_MATLAB;
		nType = SCLEX_MATLAB;
	}else if(0==_tcsicmp(language, _T("fortran"))){
		type  = L_FORTRAN;
		nType = SCLEX_FORTRAN;
	}else if(0==_tcsicmp(language, _T("makefile"))){
		type  = L_MAKEFILE;
		nType = SCLEX_MAKEFILE;
	}else if(0==_tcsicmp(language, _T("ini"))){
		type  = L_INI;
		nType = SCLEX_PROPERTIES;
	}else if(0==_tcsicmp(language, _T("pascal"))){
		type  = L_PASCAL;
		nType = SCLEX_PASCAL;
	}else if(0==_tcsicmp(language, _T("perl"))){
		type  = L_PERL;
		nType = SCLEX_PERL;
	}else if(0==_tcsicmp(language, _T("python"))){
		type  = L_PYTHON;
		nType = SCLEX_PYTHON;
	}else if(0==_tcsicmp(language, _T("ruby"))){
		type  = L_RUBY;
		nType = SCLEX_RUBY;
	}else if(0==_tcsicmp(language, _T("shell"))){
		type  = L_POWERSHELL;
		nType = SCLEX_POWERSHELL;
	}else if(0==_tcsicmp(language, _T("sql"))){
		type  = L_SQL;
		nType = SCLEX_SQL;
	}else if(0==_tcsicmp(language, _T("vb"))){
		type  = L_VB;
		nType = SCLEX_VB;
	}
	SendEditor(SCI_SETLEXER, nType);

	return type;
}

void SetStyle(const TCHAR* name, int id, int fgColor, int bgColor, int fonttype)
{
	if(NULL==name)
		return;

	int fgColor2 = (RGB((fgColor >> 16) & 0xFF, (fgColor >> 8) & 0xFF, fgColor & 0xFF)) | (fgColor & 0xFF000000);
	int bgColor2 = (RGB((bgColor >> 16) & 0xFF, (bgColor >> 8) & 0xFF, bgColor & 0xFF)) | (bgColor & 0xFF000000);
	execute(SCI_STYLESETFORE, id, fgColor2);
	execute(SCI_STYLESETBACK, id, bgColor2);
	execute(SCI_STYLESETBOLD, id, fonttype & FONTSTYLE_BOLD);
	execute(SCI_STYLESETITALIC, id, fonttype & FONTSTYLE_ITALIC);
	execute(SCI_STYLESETUNDERLINE, id, fonttype & FONTSTYLE_UNDERLINE);
}

void fnReadStyle(TiXmlElement *root, const TCHAR* language)
{
	for(TiXmlNode *node=root->FirstChild(); NULL!= node; node=node->NextSibling())
	{   
		if (node->Type() == TiXmlNode::ELEMENT && 0==_tcsicmp(node->Value(), _T("LexerStyles")) )
		{   
			for(TiXmlNode *node2=node->FirstChild(); NULL!=node2; node2=node2->NextSibling())
			{
				if (node2->Type() == TiXmlNode::ELEMENT && 0==stricmp(node2->Value(), _T("LexerType")) )
				{
					const TCHAR* scName = node2->ToElement()->Attribute(_T("name"));
					if(0==_tcsicmp(scName, language))
					{
						for(TiXmlNode *node3=node2->FirstChild(); NULL!=node3; node3=node3->NextSibling()){
							if (node3->Type() == TiXmlNode::ELEMENT && 0==_tcsicmp(node3->Value(), _T("WordsStyle")) )
							{
								const TCHAR* strName	= node3->ToElement()->Attribute("name");
								const TCHAR* strId		= node3->ToElement()->Attribute("styleID");
								const TCHAR* fgColor	= node3->ToElement()->Attribute("fgColor");
								const TCHAR* bgColor	= node3->ToElement()->Attribute("bgColor");
								const TCHAR* fontStyle	= node3->ToElement()->Attribute("fontStyle");
								int id = _ttoi(strId);
								SetStyle(strName, id, 
									_tcstol(fgColor, NULL, 16), 
									_tcstol(bgColor, NULL, 16),
									_ttoi(fontStyle)
									);
							}
						}//end for
						break;
					}
				}
			}
		}   
    }
}

void LoadStylesXML(const TCHAR* language, LangType type)
{
	HRSRC hr = ::FindResource(NULL, MAKEINTRESOURCE(IDR_XML_STYLE), "XML");  
	if (NULL == hr)  
	{  
		int ierr = GetLastError();  
		return;  
	}  
	ULONG nResSize = ::SizeofResource(NULL, hr);  // Data size/length  
	HGLOBAL hG= ::LoadResource(NULL, hr);  
	if (NULL == hG || nResSize <= 0)  
	{  
		//fail  
	}  
	LPBYTE pData = (LPBYTE)LockResource(hG);    // Data Ptr  

	TiXmlDocument doc;
	doc.Parse((char*)pData, 0); 

	FreeResource(hG);
	
	TiXmlElement *root = doc.RootElement();  
    if(!root)
        return;
	
	if(type==L_JS)
		fnReadStyle(root, "cpp");
	else 
		fnReadStyle(root, language);

	if ((type == L_HTML) || (type == L_PHP) || (type == L_ASP) || (type == L_JSP))
	{
		if(type != L_HTML)
			fnReadStyle(root, "html");
		if(type != L_PHP)
			fnReadStyle(root, "php");
		if(type != L_ASP)
			fnReadStyle(root, "asp");
		if(type != L_JSP)
			fnReadStyle(root, "jsp");
		fnReadStyle(root, "javascript");
	}

	return;
}

void _loadKeywordsXml(vector<string>& lstKeywords, const TCHAR* language)
{
	HRSRC hr = ::FindResource(NULL, MAKEINTRESOURCE(IDR_XML_LANG), "XML");  
	if (NULL == hr)  
	{  
		int ierr = GetLastError();  
		return;  
	}  
	ULONG nResSize = ::SizeofResource(NULL, hr);  // Data size/length  
	HGLOBAL hG= ::LoadResource(NULL, hr);  
	if (NULL == hG || nResSize <= 0)  
	{  
		//fail  
	}  
	LPBYTE pData = (LPBYTE)LockResource(hG);    // Data Ptr  

	TiXmlDocument doc;
	doc.Parse((char*)pData, 0); 
	FreeResource(hG);
	
	TiXmlElement *root = doc.RootElement();  
    if(!root)
        return;
	
	for(TiXmlNode *node=root->FirstChild(); NULL!= node; node=node->NextSibling())
	{   
		if (node->Type() == TiXmlNode::ELEMENT && 0==stricmp(node->Value(), _T("Languages")) )
		{   
			for(TiXmlNode *node2=node->FirstChild(); NULL!=node2; node2=node2->NextSibling())
			{
				if (node2->Type() == TiXmlNode::ELEMENT && 0==stricmp(node2->Value(), _T("Language")) )
				{
					const TCHAR* scName = node2->ToElement()->Attribute("name");
					if(0==_tcsicmp(scName, language)){
						for(TiXmlNode *node3=node2->FirstChild(); NULL!=node3; node3=node3->NextSibling())
						{
							if (node3->Type() == TiXmlNode::ELEMENT && 0==stricmp(node3->Value(), _T("Keywords")) )
							{
								TiXmlNode *kwVal = node3->FirstChild();
								if (kwVal)
								{
									const TCHAR* keyWords = kwVal->Value();
									lstKeywords.push_back(keyWords);
									if(lstKeywords.size()>=C_MAXKEY)
										return;
								}
							}
						}//end for
						break;
					}
				}
			}
		}   
    }  
}

void fnSetxxwords(const TCHAR* language, int& No, int num=-1)
{
	vector<string> lstKeywords;
	_loadKeywordsXml(lstKeywords, language);
	vector<string>::iterator it = lstKeywords.begin();
	int i = num>=0 ? num :0;
	for(; it!=lstKeywords.end(); it++)
	{
		if(i>=lstKeywords.size())
			break;
		execute(SCI_SETKEYWORDS, No, reinterpret_cast<LPARAM>(lstKeywords[i++].c_str()));
		if(++No>=C_MAXKEY || num>=0)
			break;
	}
}

void LoadKeywordsXml(const TCHAR* language, LangType type)
{
	if (type == L_XML)
	{
		for (int i = 0 ; i < 4 ; ++i)
			execute(SCI_SETKEYWORDS, i, reinterpret_cast<LPARAM>(TEXT("")));
	}
	else if ((type == L_HTML) || (type == L_PHP) || (type == L_ASP) || (type == L_JSP))
	{
		int n = 0;
		fnSetxxwords(_T("html"), n);
		
		fnSetxxwords(_T("javascript"), n);	// 1
		execute(SCI_STYLESETEOLFILLED, SCE_HJ_DEFAULT, true);
		execute(SCI_STYLESETEOLFILLED, SCE_HJ_COMMENT, true);
		execute(SCI_STYLESETEOLFILLED, SCE_HJ_COMMENTDOC, true);
	
		fnSetxxwords(_T("vb"), n);	// 2
	    execute(SCI_STYLESETEOLFILLED, SCE_HBA_DEFAULT, true);

		fnSetxxwords(_T("php"), ++n);	// 4, must is 4
		execute(SCI_STYLESETEOLFILLED, SCE_HPHP_DEFAULT, true);
		execute(SCI_STYLESETEOLFILLED, SCE_HPHP_COMMENT, true);

		execute(SCI_SETPROPERTY, reinterpret_cast<WPARAM>("fold"), reinterpret_cast<LPARAM>("1"));
		execute(SCI_SETPROPERTY, reinterpret_cast<WPARAM>("fold.compact"), reinterpret_cast<LPARAM>("0"));
		execute(SCI_SETPROPERTY, reinterpret_cast<WPARAM>("fold.html"), reinterpret_cast<LPARAM>("1"));
		execute(SCI_SETPROPERTY, reinterpret_cast<WPARAM>("fold.hypertext.comment"), reinterpret_cast<LPARAM>("1"));
	}
	else
	{
		int n = 0;
		switch(type)
		{
		case L_JS:
			{
				fnSetxxwords(_T("cpp"), n, 0);
				fnSetxxwords(language, n);
			}
			break;
		default:
			fnSetxxwords(language, n);
			break;
		}
	}
	
}

void CNotepadPlusplus::ReloadStyles(const TCHAR* language)
{
	if(NULL==g_fn)
		return;

	SendEditor(SCI_CLEARDOCUMENTSTYLE);

	int canundo = SendEditor(SCI_CANUNDO, 0, 0);
	SendEditor(SCI_STYLESETFORE, STYLE_DEFAULT, black);
	SendEditor(SCI_STYLESETBACK, STYLE_DEFAULT, white);
	SendEditor(SCI_STYLESETSIZE, STYLE_DEFAULT, 10); 
	SendEditor(SCI_STYLESETBOLD, STYLE_DEFAULT, false); 
	SendEditor(SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM)"Courier new"); 
	SendEditor(SCI_STYLECLEARALL);
	
	// init language
	LangType type	= InitLanguageType(language);
	m_Lang			= type;

	// set keywords
	LoadKeywordsXml(language, type);

	// load styles from xml file
	LoadStylesXML(language, type);

	// importent set
	SendEditor(SCI_SETSTYLEBITS, 8);	// importent

	// line wrap
	SendEditor(SCI_SETWRAPMODE, SC_WRAP_WORD); 

	// height light
	SendEditor(SCI_SETCARETLINEVISIBLE, true);
	SendEditor(SCI_SETCARETLINEBACK, 0xececed);	
	execute(SCI_MARKERENABLEHIGHLIGHT, true);

	// Indicator
	execute(SCI_SETINDICATORCURRENT,	SCE_UNIVERSAL_FOUND_STYLE_SMART);	// double click selected
	execute(SCI_INDICSETSTYLE,			SCE_UNIVERSAL_FOUND_STYLE_SMART, INDIC_ROUNDBOX);
	execute(SCI_INDICSETALPHA,			SCE_UNIVERSAL_FOUND_STYLE_SMART, 100);
	execute(SCI_INDICSETOUTLINEALPHA,	SCE_UNIVERSAL_FOUND_STYLE_SMART, 120);
	execute(SCI_INDICSETFORE,			SCE_UNIVERSAL_FOUND_STYLE_SMART, RGB(0, 0xff, 0));

	execute(SCI_INDICSETSTYLE,			SCE_UNIVERSAL_FOUND_STYLE, INDIC_ROUNDBOX);
	execute(SCI_INDICSETALPHA,			SCE_UNIVERSAL_FOUND_STYLE, 100);
	execute(SCI_INDICSETOUTLINEALPHA,	SCE_UNIVERSAL_FOUND_STYLE, 120);
	execute(SCI_INDICSETFORE,			SCE_UNIVERSAL_FOUND_STYLE, RGB(0x00, 0x66, 0xff));

	execute(SCI_INDICSETSTYLE,			SCE_UNIVERSAL_TAGMATCH, INDIC_ROUNDBOX); // match [](){} etc.
	execute(SCI_INDICSETALPHA,			SCE_UNIVERSAL_TAGMATCH, 100);
	execute(SCI_INDICSETOUTLINEALPHA,	SCE_UNIVERSAL_TAGMATCH, 120);
	execute(SCI_INDICSETFORE,			SCE_UNIVERSAL_TAGMATCH, 0xe22B8a);

	execute(SCI_INDICSETUNDER, SCE_UNIVERSAL_FOUND_STYLE_SMART, true);	// set the indicset to background
	execute(SCI_INDICSETUNDER, SCE_UNIVERSAL_FOUND_STYLE, true);
	//execute(SCI_INDICSETUNDER, SCE_UNIVERSAL_FOUND_STYLE_INC, true);
	execute(SCI_INDICSETUNDER, SCE_UNIVERSAL_TAGMATCH, true);
	//execute(SCI_INDICSETUNDER, SCE_UNIVERSAL_TAGATTR, true);

	// Other
	SendEditor(SCI_SETTABWIDTH, 4);
	//SendEditor(SCI_SETSELFORE, true, 0x33ee44);
	//SendEditor(SCI_SETSELBACK, true, RGB(128, 0xff, 128));

	// Brace match
	SendEditor(STYLE_BRACELIGHT, true);
	SendEditor(SCI_STYLESETFORE, STYLE_BRACELIGHT, RGB(0xff, 0, 0));
	SendEditor(SCI_STYLESETBOLD, STYLE_BRACELIGHT, true);
	SendEditor(SCI_STYLESETSIZE, STYLE_BRACELIGHT, 12);
 	//SendEditor(SCI_SETSEARCHFLAGS, SCFIND_REGEXP);

	// Line number
	SendEditor(SCI_SETMARGINTYPEN,  MARGIN_SCRIPT_NUMBER_INDEX, SC_MARGIN_NUMBER);	// LINE
	if(0==m_curLineWidth)
		SendEditor(SCI_SETMARGINWIDTHN, MARGIN_SCRIPT_NUMBER_INDEX, 30);
	SendEditor(SCI_STYLESETFORE, STYLE_LINENUMBER, RGB(123,123,123));				// 行号颜色
	

	// fold
	const int _markersArray[][7] = {
		{SC_MARKNUM_FOLDEROPEN, SC_MARKNUM_FOLDER, SC_MARKNUM_FOLDERSUB, SC_MARKNUM_FOLDERTAIL, SC_MARKNUM_FOLDEREND,        SC_MARKNUM_FOLDEROPENMID,     SC_MARKNUM_FOLDERMIDTAIL},
		{SC_MARK_MINUS,         SC_MARK_PLUS,      SC_MARK_EMPTY,        SC_MARK_EMPTY,         SC_MARK_EMPTY,               SC_MARK_EMPTY,                SC_MARK_EMPTY},
		{SC_MARK_ARROWDOWN,     SC_MARK_ARROW,     SC_MARK_EMPTY,        SC_MARK_EMPTY,         SC_MARK_EMPTY,               SC_MARK_EMPTY,                SC_MARK_EMPTY},
		{SC_MARK_CIRCLEMINUS,   SC_MARK_CIRCLEPLUS,SC_MARK_VLINE,        SC_MARK_LCORNERCURVE,  SC_MARK_CIRCLEPLUSCONNECTED, SC_MARK_CIRCLEMINUSCONNECTED, SC_MARK_TCORNERCURVE},
		{SC_MARK_BOXMINUS,      SC_MARK_BOXPLUS,   SC_MARK_VLINE,        SC_MARK_LCORNER,       SC_MARK_BOXPLUSCONNECTED,    SC_MARK_BOXMINUSCONNECTED,    SC_MARK_TCORNER}
	};

	if(type==L_TEXT)
	{
		SendEditor(SCI_SETMARGINWIDTHN, MARGIN_SCRIPT_FOLD_INDEX, 0);
	}
	else
	{
		SendEditor(SCI_SETPROPERTY, (WPARAM)"fold", (LPARAM)"1");
		SendEditor(SCI_SETPROPERTY, (WPARAM)"fold.compact", (LPARAM)"0");
		SendEditor(SCI_SETPROPERTY, (WPARAM)"fold.comment", (LPARAM)"1");
		SendEditor(SCI_SETPROPERTY, (WPARAM)"fold.preprocessor", (LPARAM)"1");
		
		SendEditor(SCI_SETMARGINTYPEN,  MARGIN_SCRIPT_FOLD_INDEX, SC_MARGIN_SYMBOL);	// FOLD
		SendEditor(SCI_SETMARGINMASKN,  MARGIN_SCRIPT_FOLD_INDEX, SC_MASK_FOLDERS);
		SendEditor(SCI_SETMARGINWIDTHN, MARGIN_SCRIPT_FOLD_INDEX, 14);
		
		const int k = 4;
		for (int j = 0 ; j < 7 ; ++j){
			SendEditor(SCI_MARKERDEFINE, _markersArray[0][j],  _markersArray[k][j]);
			execute(SCI_MARKERSETFORE, _markersArray[0][j], RGB(0xff,0xff,0xcc));		// 折叠框填充色
			execute(SCI_MARKERSETBACK, _markersArray[0][j], RGB(0x66,0x66,0x66));		// 折叠框边线色
			execute(SCI_MARKERENABLEHIGHLIGHT, false);
		}
		
		SendEditor(SCI_SETFOLDFLAGS, 16, 0);
		SendEditor(SCI_SETSCROLLWIDTHTRACKING, true);
		//SendEditor(SCI_SETSCROLLWIDTH, 1);	// scroll width
		
		SendEditor(SCI_SETMARGINSENSITIVEN, MARGIN_SCRIPT_FOLD_INDEX, 1);// start fold
	}
}

void CNotepadPlusplus::SetContent(const char *str, int lenOfStr)
{
	execute(SCI_CLEARALL);
/*	Document doc = execute(SCI_GETDOCPOINTER);
	if(doc)
	{
		execute(SCI_RELEASEDOCUMENT, 0, doc);
		execute(SCI_SETDOCPOINTER, 0, 0);
	}	
*/
	execute(SCI_APPENDTEXT, lenOfStr, (LPARAM) str);
}

int findTextPos(const TCHAR* str, int start, int end, int flag=0)
{
	Sci_TextToFind sci;
	sci.lpstrText = const_cast<TCHAR *>(str);
	sci.chrg.cpMin = start;
	sci.chrg.cpMax = end;
	int pos = execute(SCI_FINDTEXT, flag, reinterpret_cast<LPARAM>(&sci));
	return pos;
}


int getText(TCHAR*& str, int start, int end)
{
	Sci_TextToFind sci;
	sci.lpstrText = str;
	sci.chrg.cpMin = start;
	sci.chrg.cpMax = end;
	int len = execute(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&sci));
	return len;
}

void HilightXmlMatch(int pos0, int pos2, int len0, int len2)
{
	execute(SCI_INDICATORFILLRANGE,  pos0, len0);
	execute(SCI_INDICATORFILLRANGE,  pos2, len2);

	if(pos0<pos2)
		execute(SCI_BRACEHIGHLIGHT,  pos0, pos2+len2-1);
	else
		execute(SCI_BRACEHIGHLIGHT,  pos2, pos0+len0-1);
}


// forword search
bool fnBraceXmlMatch1(int lengthDoc, int pos0, int pos1, int pos)
{
	int lenWord = 0;
	int lenHead = pos1 - pos0 + 1;
	bool bHasArrt = false;
	while(pos0<pos && pos<=pos1)
	{
		// get head
		char* text2FindA = (char*)alloca(lenHead+1);
		if(NULL==text2FindA)
			return false;
		
		int lenHead2 = getText(text2FindA, pos0, pos1+1); 
		if(lenHead2>2)
		{
			if(!lenWord)
			{
				for(; lenWord<lenHead2; lenWord++)
				{
					if(text2FindA[lenWord]==' ' || text2FindA[lenWord]=='\r' || text2FindA[lenWord]=='\n')
					{
						text2FindA[lenWord] = '\0';
						break;
					}
					else if(text2FindA[lenWord]=='>')
					{
						bHasArrt = true;
						break;
					}
				}
			}

			// php or jsp or asp script
			if(text2FindA[lenHead2-2]=='?' || text2FindA[lenHead2-2]=='%')
			{
				pos1 = findTextPos(">", pos1+1, lengthDoc);
				continue;	
			}
			// single tag
			else if(text2FindA[lenHead2-2]=='/') 
			{
				HilightXmlMatch(pos0, pos0+lenHead2-2, lenWord, 2);
				break;
			}
			
			// get detail
			int pos2 = pos1;
			int numOfFind = 1;
			char* text2FindA2 = (char*)alloca(lenWord+1);
			if(NULL==text2FindA2)
				return false;
			for(; pos2>=0;)
			{
				pos2 = findTextPos("<", pos2+1, lengthDoc);
				if(pos2<0 || pos2>=lengthDoc)
					break;
				
				int styleAt = execute(SCI_GETSTYLEAT, pos2+1);
				if(SCE_H_DOUBLESTRING==styleAt || SCE_H_SINGLESTRING==styleAt)
					continue;

				int lenTail = getText(text2FindA2, pos2+1, pos2+lenWord+1);
				if(lenTail>=0)
				{
					if(text2FindA2[0]=='/')
					{
						if(0==_strnicmp(&text2FindA2[1], &text2FindA[1], lenWord-1))
						{//OK
							numOfFind--;
							if(0==numOfFind)
							{
								TCHAR ch;
								int posEnd = pos2+lenWord+1;
								while(posEnd<lengthDoc && (ch = execute(SCI_GETCHARAT, (WPARAM)posEnd++))!='>')
								{}
								HilightXmlMatch(pos0, pos2, bHasArrt?lenWord+1:lenWord, posEnd-pos2);
								break;
							}
						}
					}
					else if(text2FindA2[0]=='?')
					{
						pos2 = findTextPos("?>", pos2+2, lengthDoc);
					}
					else if(text2FindA2[0]=='%')
					{
						pos2 = findTextPos("%>", pos2+2, lengthDoc);
					}
					else if(0==_strnicmp(text2FindA2, &text2FindA[1], lenWord-1))
					{
						numOfFind++;
					}
				}
			}
		}
		break;
	}
	return true;
}


// back search
bool fnBraceXmlMatch2(int lengthDoc, int pos0, int pos1, int pos)
{
	int lenWord = 0;
	int lenHead = pos1 - pos0 + 1;
	while(pos0<pos && pos<=pos1)
	{
		// get tail
		char* text2FindA = (char*)alloca(lenHead+1);
		if(NULL==text2FindA)
			return false;
		
		int lenHead2 = getText(text2FindA, pos0, pos1+1); 
		if(lenHead2>2)
		{
			if(!lenWord)
			{
				int i=0;
				for(; lenWord<lenHead2; lenWord++)
				{
					if(i==0 && text2FindA[lenWord]!='/')
					{
						i = lenWord;
					}

					if(i>0 && (text2FindA[lenWord]==' '||text2FindA[lenWord]=='>'))
					{
						text2FindA[lenWord] = '\0';
						break;
					}					
				}
				lenWord -= i;
			}
			if(lenWord<=0)
				break;
			

			// get head
			int pos2 = pos0;
			int numOfFind = 1;
			char* text2FindA2 = (char*)alloca(lenWord+1);
			if(NULL==text2FindA2)
				return false;
			for(; pos2>0;)
			{
				pos2 = findTextPos("<", pos2-1, 0);
				if(pos2<0)
					break;

				int styleAt = execute(SCI_GETSTYLEAT, pos2+1);
				if(SCE_H_DOUBLESTRING==styleAt || SCE_H_SINGLESTRING==styleAt)
					continue;
				
				int lenTail = getText(text2FindA2, pos2+1, pos2+lenWord+2);
				if(lenTail>=0)
				{
					if(text2FindA2[0]=='/')
					{
						if(0==_strnicmp(&text2FindA2[1], &text2FindA[2], lenWord))
						{
							numOfFind++;
						}
					}
					else if(text2FindA2[0]=='?' || text2FindA2[0]=='%')
					{
						continue;
					}
					else if(0==_strnicmp(text2FindA2, &text2FindA[2], lenWord))
					{
						numOfFind--;
						if(0==numOfFind)
						{//OK
							bool bHasArrt = text2FindA2[lenWord]=='>' ? true : false;
							HilightXmlMatch(pos0, pos2, pos1-pos0+1, bHasArrt?lenWord+2:lenWord+1);
							break;
						}
					}
				}
			}
		}
		break;
	}
	return true;
}


// Match html tag
bool braceXmlMatch()
{
	const int pos = m_nCurPos;
	const int lengthDoc = execute(SCI_GETLENGTH);
	
	if(lengthDoc>1)
	{
		execute(SCI_SETINDICATORCURRENT, SCE_UNIVERSAL_TAGMATCH);
		int length1 = execute(SCI_GETLENGTH);
		execute(SCI_INDICATORCLEARRANGE, 0, length1);

		bool bForword = true;
		int pos1 = -1;
		int pos0 = pos;
		while((pos0 = findTextPos("<", pos0, 0)) >=0)
		{
			int styleAt = execute(SCI_GETSTYLEAT, pos0);
			if(SCE_H_DOUBLESTRING==styleAt || SCE_H_SINGLESTRING==styleAt)
				continue;

			TCHAR ch = execute(SCI_GETCHARAT, (WPARAM)pos0+1);
			if(ch=='%' || ch=='?')
				continue;
			else if(ch=='/')
			{
				bForword = false;
			}
			
			if(pos0>=0)
			{
				pos1 = pos0+1;
				for(;(pos1 = findTextPos(">", pos1, lengthDoc)) >=0; pos1++)
				{
					int styleAt = execute(SCI_GETSTYLEAT, pos1);
					if(SCE_H_DOUBLESTRING==styleAt || SCE_H_SINGLESTRING==styleAt)
						continue;
					TCHAR ch2 = execute(SCI_GETCHARAT, (WPARAM)pos1-1);
					if(ch2=='%' || ch2=='?')
						continue;
					break;
				}
			}
			break;
		}

		if(bForword)
			fnBraceXmlMatch1(lengthDoc, pos0, pos1, pos);
		else
			fnBraceXmlMatch2(lengthDoc, pos0, pos1, pos);
	}

	return true;
}

bool Isalphanum(TCHAR ch)
{
	return (ch>='a' && ch<='z' ||
		ch>='A' && ch<='Z' ||
		ch>='0' && ch<='9' ||
		ch=='_'
		);
}

int braceWord(int len)
{
	int nCountOfSel	= 0;
	if(len>1)
	{
		int length1 = execute(SCI_GETLENGTH);
		execute(SCI_SETINDICATORCURRENT, SCE_UNIVERSAL_FOUND_STYLE_SMART);
		execute(SCI_INDICATORCLEARRANGE, 0, length1);

		int line0 = execute(SCI_GETFIRSTVISIBLELINE);
		int lines = execute(SCI_LINESONSCREEN);
		int pos0 = execute(SCI_POSITIONFROMLINE, line0);
		int pos1 = execute(SCI_POSITIONFROMLINE, line0+lines);
		int lengthDoc = pos1>0 ? pos1 : execute(SCI_GETLENGTH);

		char* text2FindA = (char*)alloca(len+1);
		if(NULL==text2FindA)
			return false;
		memset(text2FindA, 0, sizeof(char)*(len+1));
		execute(SCI_GETSELTEXT, 0, (LPARAM)text2FindA);

		int targetFound = findTextPos(text2FindA, pos0, lengthDoc);
		while(targetFound>=0)
		{
			char ch = execute(SCI_GETCHARAT, targetFound+len);
			char ch2 = targetFound>0 ? execute(SCI_GETCHARAT, targetFound-1) : '$';
			if(!Isalphanum(ch) && !Isalphanum(ch2))
			{
				nCountOfSel++;
				execute(SCI_INDICATORFILLRANGE,  targetFound, len);
			}
			targetFound = findTextPos(text2FindA, targetFound+len, lengthDoc);
		}
	}

	return nCountOfSel;
}

void findMathFlag(int& braceAtCaret, int& braceOpposite)
{
	braceAtCaret = -1;
	braceOpposite = -1;
	int caretPos = execute(SCI_GETCURRENTPOS);
	
	if(caretPos>0)
	{
		TCHAR charBefore = (TCHAR)execute(SCI_GETCHARAT, caretPos - 1, 0);
		if(charBefore && _tcschr(_T("[](){}"), charBefore))
		{
			braceAtCaret = caretPos -1;
		}
	}
	
	if(braceAtCaret<0)
	{
		TCHAR charAfter = TCHAR(execute(SCI_GETCHARAT, caretPos, 0));
		if (charAfter && _tcschr(_T("[](){}"), charAfter))
        {
			braceAtCaret = caretPos;
		}
	}

	if(braceAtCaret>=0)
		braceOpposite = execute(SCI_BRACEMATCH, braceAtCaret, 0);

}

void braceFlags()
{
	// Clear match
	execute(SCI_BRACEBADLIGHT, INVALID_POSITION);

	// find new match
	int braceAtCaret;
	int braceOpposite;
	findMathFlag(braceAtCaret, braceOpposite);
	if(braceAtCaret+braceOpposite>0)
	{
		execute(SCI_BRACEHIGHLIGHT,  braceAtCaret, braceOpposite);
	}
}


void CNotepadPlusplus::UpdateUI()
{
	int curPos = SendEditor(SCI_GETCURRENTPOS);
	int lenSel = SendEditor(SCI_GETCURRENTPOS) - execute(SCI_GETANCHOR);
	if(lenSel>0)
	{
		int lineVisible = execute(SCI_GETFIRSTVISIBLELINE);
		if(m_nVisibleLine!=lineVisible)
		{
			m_nVisibleLine = lineVisible;
			int totallines = braceWord(lenSel);
		}
	}
	else
	{
		if(curPos!=m_nCurPos) // clear
		{
			int length1 = execute(SCI_GETLENGTH);
			execute(SCI_SETINDICATORCURRENT, SCE_UNIVERSAL_FOUND_STYLE_SMART);
			execute(SCI_INDICATORCLEARRANGE, 0, length1);
		}
	}

	if(curPos!=m_nCurPos)
	{
		m_nCurPos = curPos;
		braceFlags();
		
		if(m_Lang == L_HTML || m_Lang==L_PHP || m_Lang==L_ASP || m_Lang==L_JSP ||
			m_Lang==L_XML)
			braceXmlMatch();
	}
	
}

void CNotepadPlusplus::OnDblClick(NMHDR* nmhdr)
{
	SCNotification* notify = (SCNotification*)nmhdr;
	int lenSel = SendEditor(SCI_GETCURRENTPOS) - execute(SCI_GETANCHOR);
	braceWord(lenSel);
}

void CNotepadPlusplus::OnMarginClicked(NMHDR* nmhdr)
{
	SCNotification* notify = (SCNotification*)nmhdr;
	
	const int modifiers = notify->modifiers;
	const int position = notify->position;
	const int margin = notify->margin;
	const int line_number = SendEditor(SCI_LINEFROMPOSITION, position, 0);
	
	switch (margin) {
    case MARGIN_SCRIPT_FOLD_INDEX:
		{
			SendEditor(SCI_TOGGLEFOLD, line_number, 0);
		}
		break;
	}
}

void CNotepadPlusplus::OnStyleNeeded(NMHDR* nmhdr)
{
	SCNotification* notify = (SCNotification*)nmhdr;
	const int line_number = SendEditor(SCI_LINEFROMPOSITION, SendEditor(SCI_GETENDSTYLED));
	const int start_pos = SendEditor(SCI_POSITIONFROMLINE, (WPARAM)line_number);
	const int end_pos = notify->position;
	
	int line_length = SendEditor(SCI_LINELENGTH, (WPARAM)line_number);
	if (line_length > 0) {
		char first_char = SendEditor(SCI_GETCHARAT, (WPARAM)start_pos);
		
		// The SCI_STARTSTYLING here is important
		SendEditor(SCI_STARTSTYLING, start_pos, 0x1f);
		switch (first_char)
		{
		case '-':
			SendEditor(SCI_SETSTYLING, line_length, SCE_STYLE_ORANGE);
			break;
			
		case '/':
			SendEditor(SCI_SETSTYLING, line_length, SCE_STYLE_PURPLE);
			break;
			
		case '*':
			SendEditor(SCI_SETSTYLING, line_length, SCE_STYLE_BLUE);
			break;
			
		default:
			SendEditor(SCI_SETSTYLING, line_length, SCE_STYLE_BLACK);
			break;
		}
	}
}

void CNotepadPlusplus::OnModified(NMHDR* nmhdr)
{
	int line = execute(SCI_GETLINECOUNT);
	int width = 10*((int)log10(line)+0.8);
	if(width<30)
		width = 30;
	if(m_curLineWidth!=width)
	{
		SendEditor(SCI_SETMARGINWIDTHN, MARGIN_SCRIPT_NUMBER_INDEX, width);
		m_curLineWidth = width;
	}
}