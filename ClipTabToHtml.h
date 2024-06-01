
// ClipTabToHtml.h : main header file for the PROJECT_NAME application
//

#pragma once

//#ifndef __AFXWIN_H__
//	#error "include 'pch.h' before including this file for PCH"
//#endif

#include "resource.h"		// main symbols


// CClipTabToHtmlApp:
// See ClipTabToHtml.cpp for the implementation of this class
//

class CClipTabToHtmlApp : public CWinApp
{
public:
	CClipTabToHtmlApp();

// Overrides
public:
	virtual BOOL InitInstance();

	void ConvertClipBoard();

	DECLARE_MESSAGE_MAP()

// Implementation
protected:
	CString m_Clipboard;
};

extern CClipTabToHtmlApp theApp;
