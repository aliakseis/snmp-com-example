
// AspirinAppDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AspirinApp.h"
#include "AspirinAppDlg.h"
#include "AspirinCounters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAspirinAppDlg dialog




CAspirinAppDlg::CAspirinAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAspirinAppDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CAspirinAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	CAspirinApp* app = static_cast<CAspirinApp*>(AfxGetApp());
	CAspirinCounters& counters = app->GetAspirinCounters();

	CSingleLock singleLock(&counters.m_dataCritSec, TRUE);

	DDX_Text(pDX, IDC_openAspirins, counters.m_openAspirins);
	DDX_Text(pDX, IDC_connString, counters.m_connString);
	DDX_Text(pDX, IDC_started, counters.m_started);
	DDX_Text(pDX, IDC_running, counters.m_running);
	DDX_Text(pDX, IDC_adaptersNumber, counters.m_adaptersNumber);
	DDX_Text(pDX, IDC_linesNumber, counters.m_linesNumber);
	DDX_Text(pDX, IDC_inboundLines, counters.m_inboundLines);
	DDX_Text(pDX, IDC_outboundLines, counters.m_outboundLines);
	DDX_Text(pDX, IDC_InboundLinesInUse, counters.m_inboundLinesInUse);
	DDX_Text(pDX, IDC_OutboundLinesInUse, counters.m_outboundLinesInUse);
	DDX_Text(pDX, IDC_ProcessedInboundCalls, counters.m_processedInboundCalls);
	DDX_Text(pDX, IDC_ProcessedOutboundCalls, counters.m_processedOutboundCalls);
	DDX_Text(pDX, IDC_InboundCallAspirinName, counters.m_inboundCallAspirinName);
	DDX_Text(pDX, IDC_OutboundCallAspirinName, counters.m_outboundCallAspirinName);
	DDX_Text(pDX, IDC_FailedOutboundCalls, counters.m_failedOutboundCalls);
	DDX_Text(pDX, IDC_FailedInboundLogins, counters.m_failedInboundLogins);
	DDX_Text(pDX, IDC_portNum, counters.m_portNum);
	DDX_Text(pDX, IDC_remoteProcessedRequests, counters.m_remoteProcessedRequests);
	DDX_Text(pDX, IDC_LastProcessedAspirinName, counters.m_lastProcessedAspirinName);
	DDX_Text(pDX, IDC_lastRemote10Min, counters.m_lastRemote10Min);
	DDX_Text(pDX, IDC_lastRemote30Min, counters.m_lastRemote30Min);
	DDX_Text(pDX, IDC_lastRemote120Min, counters.m_lastRemote120Min);
	DDX_Text(pDX, IDC_lastRemote6Hours, counters.m_lastRemote6Hours);
	DDX_Text(pDX, IDC_lastRemote24Hours, counters.m_lastRemote24Hours);

}

BEGIN_MESSAGE_MAP(CAspirinAppDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_openAspirins, IDC_lastRemote24Hours, onNotifyKillFocus)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_START, &CAspirinAppDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_INBOUND_COMPLETE, &CAspirinAppDlg::OnBnClickedInboundComplete)
	ON_BN_CLICKED(IDC_OUTBOUND_COMPLETE, &CAspirinAppDlg::OnBnClickedOutboundComplete)
	ON_BN_CLICKED(IDC_REQUEST_COMPLETE, &CAspirinAppDlg::OnBnClickedRequestComplete)
END_MESSAGE_MAP()


// CAspirinAppDlg message handlers

BOOL CAspirinAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAspirinAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAspirinAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAspirinAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAspirinAppDlg::onNotifyKillFocus(UINT id)
{
	TRACE(__FUNCTION__);
	UpdateData();
}


void CAspirinAppDlg::OnBnClickedStart()
{
	CAspirinApp* app = static_cast<CAspirinApp*>(AfxGetApp());
	CAspirinCounters& counters = app->GetAspirinCounters();

	CSingleLock singleLock(&counters.m_dataCritSec, TRUE);

	_ftime(&counters.m_startTime);
}


void CAspirinAppDlg::OnBnClickedInboundComplete()
{
	CAspirinApp* app = static_cast<CAspirinApp*>(AfxGetApp());
	CAspirinCounters& counters = app->GetAspirinCounters();

	CSingleLock singleLock(&counters.m_dataCritSec, TRUE);

	_ftime(&counters.m_inboundCallCompleteTime);
}


void CAspirinAppDlg::OnBnClickedOutboundComplete()
{
	CAspirinApp* app = static_cast<CAspirinApp*>(AfxGetApp());
	CAspirinCounters& counters = app->GetAspirinCounters();

	CSingleLock singleLock(&counters.m_dataCritSec, TRUE);

	_ftime(&counters.m_outboundCallCompleteTime);
}


void CAspirinAppDlg::OnBnClickedRequestComplete()
{
	CAspirinApp* app = static_cast<CAspirinApp*>(AfxGetApp());
	CAspirinCounters& counters = app->GetAspirinCounters();

	CSingleLock singleLock(&counters.m_dataCritSec, TRUE);

	_ftime(&counters.m_lastRequestCompleteTime);
}
