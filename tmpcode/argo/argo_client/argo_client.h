// argo_client.h : main header file for the ARGO_CLIENT application
//

#if !defined(AFX_ARGO_CLIENT_H__85EC1EBD_B96E_4B55_B505_7228BEA85CF1__INCLUDED_)
#define AFX_ARGO_CLIENT_H__85EC1EBD_B96E_4B55_B505_7228BEA85CF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CArgo_clientApp:
// See argo_client.cpp for the implementation of this class
//

class CArgo_clientApp : public CWinApp
{
public:
	CArgo_clientApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArgo_clientApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CArgo_clientApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARGO_CLIENT_H__85EC1EBD_B96E_4B55_B505_7228BEA85CF1__INCLUDED_)
