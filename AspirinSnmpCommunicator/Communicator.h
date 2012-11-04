#pragma once
#include "resource.h"       // main symbols

#pragma comment(lib, "comsupp.lib")


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// IEvents
[ dual, uuid("782E8E34-E9B3-4a6c-BFFF-59BD8B798F71") ]
__interface IEvents
{
    //[id(1)] HRESULT HandleEvent([out, satype(unsigned char)] SAFEARRAY** value);
    [id(1)] HRESULT HandleEvent([out, satype("BYTE")] SAFEARRAY** value);
};

// ICommunicator
[
	object,
	uuid("2215980C-3D31-4D10-B330-3B757C50870F"),
	dual,	helpstring("ICommunicator Interface"),
	pointer_default(unique)
]
__interface ICommunicator : IDispatch
{
	//[id(1), helpstring("method FireEvent")] HRESULT FireEvent([out, satype(unsigned char)] SAFEARRAY** value);
	[id(1), helpstring("method FireEvent")] HRESULT FireEvent([out, satype(VARIANT)] SAFEARRAY** value);
};

// CCommunicator
[
	coclass,
	default(ICommunicator),
	threading(apartment),
	aggregatable(never),
	vi_progid("AspirinSnmp.Communicator"),
	progid("AspirinSnmp.Communicator.1"),
	version(1.0),
	uuid("A4A854A8-184F-4661-86B6-DBF069E289E1"),
	source(IEvents),
	helpstring("Communicator Class")
]
class ATL_NO_VTABLE CCommunicator :
	//public ICommunicator,
public IDispatchImpl<ICommunicator>,
	// the following base classes support added injected code
public IConnectionPointContainerImpl<CCommunicator>,
public IConnectionPointImpl<CCommunicator, &__uuidof(::IEvents), CComDynamicUnkArray>
{
public:

	DECLARE_CLASSFACTORY_SINGLETON(CCommunicator)

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	BEGIN_CONNECTION_POINT_MAP(CCommunicator)
	CONNECTION_POINT_ENTRY(__uuidof(::IEvents))
	END_CONNECTION_POINT_MAP()

	STDMETHOD(FireEvent)(SAFEARRAY** value);
};
