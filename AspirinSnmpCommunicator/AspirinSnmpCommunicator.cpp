// CAspirinSnmpCommunicator.cpp : Implementation of WinMain


#include "stdafx.h"
#include "resource.h"


// The module attribute causes WinMain to be automatically implemented for you
[ module(SERVICE, uuid = "{B27C9EEB-987C-4E41-8D7A-69B18033153B}",
		 name = "AspirinSnmpCommunicator",
		 helpstring = "AspirinSnmpCommunicator 1.0 Type Library",
		 resource_name = "IDS_SERVICENAME") ]
class CAspirinSnmpCommunicatorModule
{
public:
	HRESULT InitializeSecurity() throw()
	{
		// TODO : Call CoInitializeSecurity and provide the appropriate security settings for
		// your service
		// Suggested - PKT Level Authentication,
		// Impersonation Level of RPC_C_IMP_LEVEL_IDENTIFY
		// and an appropiate Non NULL Security Descriptor.
		CoInitializeSecurity(NULL, -1, NULL, NULL,
							 RPC_C_AUTHN_LEVEL_NONE, RPC_C_IMP_LEVEL_IDENTIFY,
							 NULL, EOAC_NONE, NULL);

		return S_OK;
	}
};

