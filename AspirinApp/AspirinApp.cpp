
// AspirinApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AspirinApp.h"
#include "AspirinAppDlg.h"

#include "AspirinCounters.h"
#include "AspirinSnmpReceiver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAspirinAppApp

BEGIN_MESSAGE_MAP(CAspirinApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAspirinAppApp construction

CAspirinApp::CAspirinApp()
	: m_pAspirinCounters(new CAspirinCounters())
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CAspirinAppApp object

CAspirinApp theApp;


// CAspirinAppApp initialization

BOOL CAspirinApp::InitInstance()
{
	InitializeSnmpReceiver();

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	//AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CAspirinAppDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
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

CAspirinCounters& CAspirinApp::GetAspirinCounters()
{
	return *m_pAspirinCounters;
}


BOOL CAspirinApp::InitializeSnmpReceiver()
{
	CComPtr<IUnknown> spComm;

	HRESULT hr = spComm.CoCreateInstance(__uuidof(CCommunicator));

	if (FAILED(hr))
	{
		//ERRORLOG(hr);
		return FALSE;
	}

	m_pSnmpReceiver.reset(new CAspirinSnmpReceiver(spComm));

	return TRUE;
}
