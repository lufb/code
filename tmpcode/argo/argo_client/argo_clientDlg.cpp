// argo_clientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "argo_client.h"
#include "argo_clientDlg.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArgo_clientDlg dialog

CArgo_clientDlg::CArgo_clientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CArgo_clientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CArgo_clientDlg)
	m_strRootPath = _T("");
	m_strIPv4Port = _T("");
	m_strOutput = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hCRead	= INVALID_HANDLE_VALUE;
	m_hCWrite	= INVALID_HANDLE_VALUE;
	m_hGRead	= INVALID_HANDLE_VALUE;
	m_hGWrite	= INVALID_HANDLE_VALUE;
	m_hThread	= INVALID_HANDLE_VALUE;
	m_hProcess	= INVALID_HANDLE_VALUE;
	m_isInit	= FALSE;
	m_isExit	= FALSE;
	InitializeCriticalSection(&m_csPipe);
}

void CArgo_clientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArgo_clientDlg)
	DDX_Control(pDX, IDC_IPADDRESS_IPV4_ADDR, m_ctrlIPv4Addr);
	DDX_Control(pDX, IDC_EDIT_OUTPUT, m_ctrlOutput);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_btnStop);
	DDX_Control(pDX, IDC_BUTTON_BOOT, m_btnBoot);
	DDX_Text(pDX, IDC_EDIT_ROOT_DIR, m_strRootPath);
	DDX_Text(pDX, IDC_EDIT_IPV4_PORT, m_strIPv4Port);
	DDX_Text(pDX, IDC_EDIT_OUTPUT, m_strOutput);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CArgo_clientDlg, CDialog)
	//{{AFX_MSG_MAP(CArgo_clientDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BOOT, OnButtonBoot)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnButtonExit)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_ROOT_DIR, OnButtonRootDir)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER_REFRESH_EDIT_OUTPUT, OnRefreshEditOuput)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArgo_clientDlg message handlers

static
void ReadPipeWin32Thread(void * pvParam)
{
	char				szBuffer[8192];
	CArgo_clientDlg	*	dlg = (CArgo_clientDlg*)pvParam;
	DWORD				dwNumberOfBytesRead;
	BOOL				bResult;
	
	for(;;)
	{
		if(dlg->m_isExit)
			break;

		memset(szBuffer, 0, sizeof(szBuffer));
		bResult = ReadFile(dlg->m_hGRead, szBuffer,
			sizeof(szBuffer) - 1, &dwNumberOfBytesRead, NULL);
		if(FALSE == bResult)
			break;
		
		EnterCriticalSection(&(dlg->m_csPipe));
		dlg->m_strPipe += szBuffer;
		LeaveCriticalSection(&(dlg->m_csPipe));
		
		::PostMessage(dlg->m_hWnd, WM_USER_REFRESH_EDIT_OUTPUT, 0, 0);
	}
}

BOOL CArgo_clientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	//			 提取配置文件
	m_btnBoot.EnableWindow(FALSE);
	m_btnStop.EnableWindow(FALSE);

	char	buf[1024];
	GetPrivateProfileString("config", "main_dir", "", buf, sizeof(buf), "D:\\argo_client\\argo_client.ini");
	m_strRootPath = buf;

	GetPrivateProfileString("config", "port", "", buf, sizeof(buf), "D:\\argo_client\\argo_client.ini");
	m_strIPv4Port = buf;

	GetPrivateProfileString("config", "ip", "", buf, sizeof(buf), "D:\\argo_client\\argo_client.ini");
	m_ctrlIPv4Addr.SetWindowText(buf);

	UpdateData(FALSE);

	//	初始化2组管道句柄
	SECURITY_ATTRIBUTES		sa;

	sa.nLength				= sizeof(sa);
	sa.bInheritHandle		= TRUE;
	sa.lpSecurityDescriptor	= NULL;

	if(!CreatePipe(&m_hGRead, &m_hCWrite, &sa, 0 ))
	{
		CString	strError;

		strError.Format("无法建立通讯管道,错误代码: %u",  GetLastError());

		MessageBox(strError, "错误消息", MB_OK | MB_ICONERROR);

		return FALSE;
	}
	if(!CreatePipe(&m_hCRead, &m_hGWrite, &sa, 0 ))
	{
		CString	strError;

		strError.Format("无法建立通讯管道,错误代码: %u",  GetLastError());

		MessageBox(strError, "错误消息", MB_OK | MB_ICONERROR);

		return FALSE;
	}

	m_hThread = (HANDLE)_beginthread(ReadPipeWin32Thread, 0, this);
	if(INVALID_HANDLE_VALUE == m_hThread)
	{
		MessageBox("无法初始化", "致命错误", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	m_btnBoot.EnableWindow(TRUE);
	m_btnStop.EnableWindow(FALSE);

	m_isInit = TRUE;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CArgo_clientDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	CString	m_strIPv4Addr;

	UpdateData(TRUE);
	m_ctrlIPv4Addr.GetWindowText(m_strIPv4Addr);

	WritePrivateProfileString("config", "main_dir", m_strRootPath, "D:\\argo_client\\argo_client.ini");
	WritePrivateProfileString("config", "ip", m_strIPv4Addr, "D:\\argo_client\\argo_client.ini");
	WritePrivateProfileString("config", "port", m_strIPv4Port, "D:\\argo_client\\argo_client.ini");
	
}

void CArgo_clientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CArgo_clientDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CArgo_clientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

LRESULT CArgo_clientDlg::OnRefreshEditOuput(WPARAM wParam, LPARAM lParam)
{
	if(m_strOutput.GetLength() >= 80 * 1000)
	{
		m_strOutput.Empty();
	}

    EnterCriticalSection(&m_csPipe);
	m_strOutput += m_strPipe;m_strPipe="";
	LeaveCriticalSection(&m_csPipe);

	UpdateData(FALSE);
	m_ctrlOutput.SetSel(0xffffff, 0xffffff);

    return 0;    
}

void CArgo_clientDlg::OnButtonBoot() 
{
	// TODO: Add your control notification handler code here
	//::SendMessage(this->m_hWnd, WM_USER_REFRESH_EDIT_OUTPUT, 0, 0);
	//;
	m_btnBoot.EnableWindow(FALSE);
	m_btnStop.EnableWindow(FALSE);

	if(!BootConsoleCmd())
	{
		m_btnBoot.EnableWindow(TRUE);
		m_btnStop.EnableWindow(FALSE);
		return;
	}

	m_btnStop.EnableWindow(TRUE);
}

void CArgo_clientDlg::OnButtonStop() 
{
	// TODO: Add your control notification handler code here
	m_btnBoot.EnableWindow(FALSE);
	m_btnStop.EnableWindow(FALSE);

	if(m_hProcess != INVALID_HANDLE_VALUE)
	{
		TerminateProcess(m_hProcess, 0);//终止服务进程
		m_hProcess = INVALID_HANDLE_VALUE;
	}

	m_btnBoot.EnableWindow(TRUE);
	m_btnStop.EnableWindow(FALSE);
}


BOOL CArgo_clientDlg::BootConsoleCmd()
{
	STARTUPINFO				si;
	PROCESS_INFORMATION		pi;

	memset(&pi, 0, sizeof(pi));
	memset(&si, 0, sizeof(si));

	si.cb			= sizeof(si);
	si.dwFlags		= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.hStdInput	= m_hCRead; 
	si.hStdOutput	= m_hCWrite;
	si.hStdError	= m_hCWrite; 
	si.wShowWindow	= SW_HIDE;

	if(!CreateProcess(
		"d:\\file_monitor.exe", NULL, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
	{
		CString	strError;

		strError.Format("无法创建服务进程,错误代码: %u",  GetLastError());

		MessageBox(strError, "错误消息", MB_OK | MB_ICONERROR);

		return FALSE;
	}

	m_hProcess = pi.hProcess;

	return TRUE;
}

void CArgo_clientDlg::OnButtonExit() 
{
	// TODO: Add your control notification handler code here
	if(IDYES == MessageBox("你确定要退出吗?",
		"提示消息", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2))
	{
		CDialog::OnOK();
	}
}


void CArgo_clientDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	if(m_isInit)
	{
		DWORD	nNumberOfBytesWritten;
		m_isExit = TRUE;

		//	终止服务进程,如果已经启动了的话
		if(m_hProcess != INVALID_HANDLE_VALUE)
		{
			TerminateProcess(m_hProcess, 0);
			m_hProcess = INVALID_HANDLE_VALUE;
		}

		//	写管道,让读线程的线程退出
		WriteFile(m_hCWrite, "exit", 4, &nNumberOfBytesWritten,NULL);
		WaitForSingleObject(m_hThread, 10*1000);
	}
}

void CArgo_clientDlg::OnButtonRootDir() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	BROWSEINFO bi;
	char path[MAX_PATH];
	bi.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = path; //此参数如为NULL则不能显示对话框
	bi.lpszTitle = "选择一个目录";
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.iImage = 0;   //初始化入口参数bi结束
	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);//调用显示选择对话框
	if(pIDList)
	{
		SHGetPathFromIDList(pIDList, path);
		//取得文件夹路径到path里
		m_strRootPath = path;//将路径保存在一个CString对象里，m_strPathName为编辑框绑定的一个值类
		//型变量
	}
	UpdateData(FALSE);    //将选择的文件夹路径显示在编辑框中
}

//////////////////////////////////////////////////////////////////////////