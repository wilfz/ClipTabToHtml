
// ClipTabToHtml.cpp : Defines the class behaviors for the application.
//

#include "framework.h"
#include "ClipTabToHtml.h"
//#include "ClipTabToHtmlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int BuildHtmlTable(const TCHAR* source, TCHAR* target);
void CopyHTML(char* html);

// CClipTabToHtmlApp

BEGIN_MESSAGE_MAP(CClipTabToHtmlApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CClipTabToHtmlApp construction

CClipTabToHtmlApp::CClipTabToHtmlApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CClipTabToHtmlApp object

CClipTabToHtmlApp theApp;


// CClipTabToHtmlApp initialization

BOOL CClipTabToHtmlApp::InitInstance()
{
	/*
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	*/

	CWinApp::InitInstance();

	ConvertClipBoard();

	/*
	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("ClipTabToHtml"));

	CClipTabToHtmlDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
		ConvertClipBoard();
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif
	*/

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


// CopyHtml() - Copies given HTML to the clipboard.
// The HTML/BODY blanket is provided, so you only need to
// call it like CallHtml("<b>This is a test</b>");
void CopyHTML(char* html)
{
	// Create temporary buffer for HTML header...
	char* buf = new char[400 + strlen(html)];
	if (!buf) return;

	// Get clipboard id for HTML format...
	static int cfid = 0;
	if (!cfid) cfid = RegisterClipboardFormat(TEXT("HTML Format"));

	// Create a template string for the HTML header...
	strcpy(buf,
		"Version:0.9\r\n"
		"StartHTML:00000000\r\n"
		"EndHTML:00000000\r\n"
		"StartFragment:00000000\r\n"
		"EndFragment:00000000\r\n"
		"<html><body>\r\n"
		"<!--StartFragment -->\r\n");

	// Append the HTML...
	strcat(buf, html);
	strcat(buf, "\r\n");
	// Finish up the HTML format...
	strcat(buf,
		"<!--EndFragment-->\r\n"
		"</body>\r\n"
		"</html>");

	// Now go back, calculate all the lengths, and write out the
	// necessary header information. Note, wsprintf() truncates the
	// string when you overwrite it so you follow up with code to replace
	// the 0 appended at the end with a '\r'...
	char* ptr = strstr(buf, "StartHTML");

	// TODO: Do not use. Consider using one of the following functions instead: 
	// StringCbPrintf, StringCbPrintfEx, StringCchPrintf, or StringCchPrintfEx. 
	// See Security Considerations.
	wsprintfA(ptr + 10, "%08u", strstr(buf, "<html>") - buf);
	*(ptr + 10 + 8) = '\r';

	ptr = strstr(buf, "EndHTML");
	wsprintfA(ptr + 8, "%08u", strlen(buf));
	*(ptr + 8 + 8) = '\r';

	ptr = strstr(buf, "StartFragment");
	wsprintfA(ptr + 14, "%08u", strstr(buf, "<!--StartFrag") - buf);
	*(ptr + 14 + 8) = '\r';

	ptr = strstr(buf, "EndFragment");
	wsprintfA(ptr + 12, "%08u", strstr(buf, "<!--EndFrag") - buf);
	*(ptr + 12 + 8) = '\r';

	// Now you have everything in place ready to put on the clipboard.
	// Open the clipboard...
	if (OpenClipboard(0))
	{
		// Empty what's in there...
		//EmptyClipboard();

		// Allocate global memory for transfer...
		HGLOBAL hText = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, strlen(buf) + 4);

		// Put your string in the global memory...
		char* ptr = (char*)GlobalLock(hText);
		strcpy(ptr, buf);
		GlobalUnlock(hText);

		::SetClipboardData(cfid, hText);

		CloseClipboard();
		// Free memory...
		GlobalFree(hText);
	}
	// Clean up...
	delete[] buf;
}

int BuildHtmlTable(const TCHAR* source, TCHAR* target)
{
	int tsize = 0;
	// html table
	// having style tag inside of body is not W3C-conform, but works.
	static const TCHAR table_open[] = TEXT("<style> th, td { border: thin solid; padding-right: 4px; padding-left: 4px; } </style><table style=\"border-collapse: collapse;\">");
	static const TCHAR table_close[] = TEXT("</table>");
	BOOL bTable = FALSE;
	// table header
	static const TCHAR thead_open[] = TEXT("<thead>");
	static const TCHAR thead_close[] = TEXT("</thead>");
	BOOL bTableHead = FALSE;
	// table body
	static const TCHAR tbody_open[] = TEXT("<tbody>");
	static const TCHAR tbody_close[] = TEXT("</tbody>");
	BOOL bTableBody = FALSE;
	// table row
	static const TCHAR tr_open[] = TEXT("<tr>");
	static const TCHAR tr_close[] = TEXT("</tr>");
	BOOL bTableRow = FALSE;
	// header and data item
	static const TCHAR th_open[] = TEXT("<th>");
	static const TCHAR th_close[] = TEXT("</th>");
	static const TCHAR td_open[] = TEXT("<td>");
	static const TCHAR td_close[] = TEXT("</td>");
	BOOL bTableItem = FALSE;

	TCHAR c, d;
	int i = 0;
	// skip empty lines
	while ((c = source[i]) != '\0')
	{
		if (c != '\n' && c != '\r')
		{
			if (target)
				lstrcpy(target + tsize, table_open);
			tsize += lstrlen(table_open);
			bTable = TRUE;
			if (target)
				lstrcpy(target + tsize, tr_open);
			tsize += lstrlen(tr_open);
			bTableRow = TRUE;
			if (target)
				lstrcpy(target + tsize, th_open);
			tsize += lstrlen(th_open);
			bTableItem = TRUE;
			break;
		}
		i++;
	}

	// header row
	while ((c = source[i++]) != '\0')
	{
		if (c == '\n' || c == '\r')
		{
			if (c == '\r' && (d = source[i]) == '\n')
				c = source[i++];
			else if (c == '\n' && (d = source[i]) == '\r')
				c = source[i++];

			if (target)
				lstrcpy(target + tsize, th_close);
			tsize += lstrlen(th_close);
			bTableItem = FALSE;
			if (target)
				lstrcpy(target + tsize, tr_close);
			tsize += lstrlen(tr_close);
			bTableRow = FALSE;
			break;
		}

		switch (c)
		{
		case '\t':
			if (target)
				lstrcpy(target + tsize, th_close);
			tsize += lstrlen(th_close);
			if (target)
				lstrcpy(target + tsize, th_open);
			tsize += lstrlen(th_open);
			bTableItem = TRUE;
			break;
		case '<':
			if (target)
				lstrcpy(target + tsize, TEXT("&lt;"));
			tsize += lstrlen(TEXT("&lt;"));
			break;
		case '>':
			if (target)
				lstrcpy(target + tsize, TEXT("&gt;"));
			tsize += lstrlen(TEXT("&gt;"));
			break;
		case '&':
			if (target)
				lstrcpy(target + tsize, TEXT("&amp;"));
			tsize += lstrlen(TEXT("&amp;"));
			break;
		default:
			if (target)
				target[tsize] = c;
			tsize++;
		}
	}

	if ((c = source[i]) == '\0') // only header, no data rows
	{
		// close everything and return
		if (bTableItem)
		{
			if (target)
				lstrcpy(target + tsize, th_close);
			tsize += lstrlen(th_close);
			bTableItem = FALSE;
		}
		if (bTableRow)
		{
			if (target)
				lstrcpy(target + tsize, tr_close);
			tsize += lstrlen(tr_close);
			bTableRow = FALSE;
		}
		if (bTable)
		{
			if (target)
				lstrcpy(target + tsize, table_close);
			tsize += lstrlen(table_close);
			bTable = FALSE;
		}

		if (target)
			target[tsize] = '\0';
		tsize++;

		return tsize;
	}
	else
	{
		// data row follows: open first row, first item
		if (target)
			lstrcpy(target + tsize, tr_open);
		tsize += lstrlen(tr_open);
		bTableRow = TRUE;
		if (target)
			lstrcpy(target + tsize, td_open);
		tsize += lstrlen(td_open);
		bTableItem = TRUE;
	}

	// data rows
	while ((c = source[i++]) != '\0')
	{
		if (c == '\n' || c == '\r')
		{
			if (c == '\r' && (d = source[i]) == '\n')
				i++;
			else if (c == '\n' && (d = source[i]) == '\r')
				i++;

			// close the current oitem and row
			if (bTableItem)
			{
				if (target)
					lstrcpy(target + tsize, td_close);
				tsize += lstrlen(td_close);
				bTableItem = FALSE;
			}
			if (bTableRow)
			{
				if (target)
					lstrcpy(target + tsize, tr_close);
				tsize += lstrlen(tr_close);
				bTableRow = FALSE;
			}
			continue;
		}

		if (!bTableRow)
		{
			if (target)
				lstrcpy(target + tsize, tr_open);
			tsize += lstrlen(tr_open);
			bTableRow = TRUE;
		}
		if (!bTableItem)
		{
			if (target)
				lstrcpy(target + tsize, td_open);
			tsize += lstrlen(td_open);
			bTableItem = TRUE;
		}

		switch (c)
		{
		case '\t':
			if (target)
				lstrcpy(target + tsize, td_close);
			tsize += lstrlen(td_close);
			if (target)
				lstrcpy(target + tsize, td_open);
			tsize += lstrlen(td_open);
			break;
		case '<':
			if (target)
				lstrcpy(target + tsize, TEXT("&lt;"));
			tsize += lstrlen(TEXT("&lt;"));
			break;
		case '>':
			if (target)
				lstrcpy(target + tsize, TEXT("&gt;"));
			tsize += lstrlen(TEXT("&gt;"));
			break;
		case '&':
			if (target)
				lstrcpy(target + tsize, TEXT("&amp;"));
			tsize += lstrlen(TEXT("&amp;"));
			break;
		default:
			if (target)
				target[tsize] = c;
			tsize++;
		}
	}

	if (bTableItem)
	{
		if (target)
			lstrcpy(target + tsize, td_close);
		tsize += lstrlen(td_close);
		bTableItem = FALSE;
	}
	if (bTableRow)
	{
		if (target)
			lstrcpy(target + tsize, tr_close);
		tsize += lstrlen(tr_close);
		bTableRow = FALSE;
	}
	if (bTable)
	{
		if (target)
			lstrcpy(target + tsize, table_close);
		tsize += lstrlen(table_close);
		bTable = FALSE;
	}

	if (target)
		target[tsize] = '\0';
	tsize++;

	return tsize;
}

void CClipTabToHtmlApp::ConvertClipBoard()
{
	//open the clipboard
#ifdef _UNICODE
	if (::IsClipboardFormatAvailable(CF_UNICODETEXT) && ::OpenClipboard(NULL))
	{
		HANDLE hData = ::GetClipboardData(CF_UNICODETEXT);
		WCHAR* buffer = (WCHAR*)::GlobalLock(hData);
		int tablesize = BuildHtmlTable(buffer, nullptr); // find out how much memory is needed
		WCHAR* wtable = new WCHAR[tablesize]; // allocate that memory
		BuildHtmlTable(buffer, wtable); // wtable is widechar
		int len = (int)wcslen(wtable) + 1; // include trailing '\0'
		// each WCHAR may expand to maximal 4 byte in UTF-8
		char* utf8table = new char[len * 4];
		int cnt = ::WideCharToMultiByte(CP_UTF8, 0, wtable, len, utf8table, len * 4, NULL, FALSE);
		ASSERT(cnt > 0);
		delete[] wtable;
		ASSERT(cnt >= 0);
		::GlobalUnlock(hData);
		::CloseClipboard();
		if (cnt > 0 && cnt <= len * 4)
			CopyHTML(utf8table); // create as clipboard "HTML Format"
		delete[] utf8table;
	}
	//	else
#endif
/*
		if (IsClipboardFormatAvailable(CF_TEXT) && ::OpenClipboard(NULL))
		{
			HANDLE hData = GetClipboardData(CF_TEXT);
			char* buffer = (char*)GlobalLock(hData);
			sClipboard = buffer;
			GlobalUnlock(hData);
			CloseClipboard();
		}
*/
}
