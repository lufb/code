// argo_clientDlg.h : header file
//

#if !defined(AFX_ARGO_CLIENTDLG_H__865C10DA_BE30_4391_8DAA_98C63305BABE__INCLUDED_)
#define AFX_ARGO_CLIENTDLG_H__865C10DA_BE30_4391_8DAA_98C63305BABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CArgo_clientDlg dialog

#define WM_USER_REFRESH_EDIT_OUTPUT WM_USER + 1

class CArgo_clientDlg : public CDialog
{
// Construction
public:
	HANDLE m_hProcess;
	BOOL m_isExit;
	HANDLE m_hThread;
	BOOL m_isInit;
	CRITICAL_SECTION m_csPipe;
	CString m_strPipe;
	BOOL BootConsoleCmd();
	HANDLE m_hCWrite;
	HANDLE m_hCRead;
	HANDLE m_hGWrite;
	HANDLE m_hGRead;
	CArgo_clientDlg(CWnd* pParent = NULL);	// standard constructor
	virtual void OnOK(void) {}
	virtual void OnCancel(void) {}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

// Dialog Data
	//{{AFX_DATA(CArgo_clientDlg)
	enum { IDD = IDD_ARGO_CLIENT_DIALOG };
	CIPAddressCtrl	m_ctrlIPv4Addr;
	CEdit	m_ctrlOutput;
	CButton	m_btnStop;
	CButton	m_btnBoot;
	CString	m_strRootPath;
	CString	m_strIPv4Port;
	CString	m_strOutput;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArgo_clientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CArgo_clientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonBoot();
	afx_msg void OnButtonStop();
	afx_msg void OnButtonExit();
	afx_msg void OnDestroy();
	afx_msg void OnButtonRootDir();
	afx_msg void OnButtonSave();
	//}}AFX_MSG

	afx_msg LRESULT OnRefreshEditOuput(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARGO_CLIENTDLG_H__865C10DA_BE30_4391_8DAA_98C63305BABE__INCLUDED_)
