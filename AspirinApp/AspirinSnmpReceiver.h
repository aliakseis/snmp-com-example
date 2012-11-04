#pragma once

#if _MSC_VER < 1400
extern CComModule _Module;
#endif

#include <atlcom.h>

#import "..\AspirinSnmpCommunicator\_AspirinSnmpCommunicator.tlb" no_namespace, raw_interfaces_only


static _ATL_FUNC_INFO OnSnmpRequestInfo =  {CC_STDCALL, VT_I4, 1, {VT_ARRAY | VT_BYREF}};

class CAspirinSnmpReceiver : public IDispEventSimpleImpl<1, CAspirinSnmpReceiver, &__uuidof(IEvents)>
{

public:
	CAspirinSnmpReceiver(IUnknown* pSource);
	~CAspirinSnmpReceiver(void);

	STDMETHOD(OnSnmpRequest)(SAFEARRAY** ppValue);

	virtual ULONG STDMETHODCALLTYPE AddRef()
	{
		return 1;
	}
	virtual ULONG STDMETHODCALLTYPE Release()
	{
		return 1;
	}

	BEGIN_SINK_MAP(CAspirinSnmpReceiver)
	SINK_ENTRY_INFO(1, __uuidof(IEvents), 1, OnSnmpRequest, &OnSnmpRequestInfo)
	END_SINK_MAP()

private:
	void HookEvent(IUnknown* pSource);
	void UnhookEvent();

	CComPtr<IUnknown> m_pSource;
};
