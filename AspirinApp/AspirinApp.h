
// AspirinApp.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#include <memory>

class CAspirinCounters;
class CAspirinSnmpReceiver;

class CComUsageScope
{
	bool m_bInitialized;
public:
	CComUsageScope(DWORD dwCoInit = COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY)
	{
		m_bInitialized = SUCCEEDED(CoInitializeEx(NULL, dwCoInit));
	}
	~CComUsageScope()
	{
		if (m_bInitialized)
			CoUninitialize();
	}
};

// CAspirinAppApp:
// See AspirinApp.cpp for the implementation of this class
//

class CAspirinApp : public CWinAppEx
{
public:
	CAspirinApp();

	// Overrides
public:
	virtual BOOL InitInstance();

	// Implementation

	DECLARE_MESSAGE_MAP()

public:
	CAspirinCounters& GetAspirinCounters();

private:
	BOOL InitializeSnmpReceiver();

private:
	std::auto_ptr<CAspirinCounters> m_pAspirinCounters;
	CComUsageScope m_comUsageScope;
	std::auto_ptr<CAspirinSnmpReceiver> m_pSnmpReceiver;
};

extern CAspirinApp theApp;