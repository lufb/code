// argo_client.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "argo_client.h"
#include "argo_clientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArgo_clientApp

BEGIN_MESSAGE_MAP(CArgo_clientApp, CWinApp)
	//{{AFX_MSG_MAP(CArgo_clientApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArgo_clientApp construction

CArgo_clientApp::CArgo_clientApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CArgo_clientApp object

CArgo_clientApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CArgo_clientApp initialization

BOOL CArgo_clientApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	::CreateMutex( NULL, FALSE, "Global\\ARGO_CLIENT" );
	if ( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		MessageBox(NULL, "argo_client 已经启动", "错误消息", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	CArgo_clientDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
