#include "stdafx.h"
#include "AspirinSnmpReceiver.h"
#include "AspirinCounters.h"
#include "..\include\SnmpCounters.h"
#include "..\include\StreamImpl.h"

#include "AspirinApp.h"

CAspirinSnmpReceiver::CAspirinSnmpReceiver(IUnknown* pSource)
{
	HookEvent(pSource);
}

CAspirinSnmpReceiver::~CAspirinSnmpReceiver(void)
{
	UnhookEvent();
}

void CAspirinSnmpReceiver::HookEvent(IUnknown* pSource)
{
	HRESULT hr = DispEventAdvise(pSource);
	if (SUCCEEDED(hr))
		m_pSource = pSource;
}

void CAspirinSnmpReceiver::UnhookEvent()
{
	if (m_pSource != NULL)
		DispEventUnadvise(m_pSource);
}

STDMETHODIMP CAspirinSnmpReceiver::OnSnmpRequest(SAFEARRAY** ppValue)
{
	if (NULL == ppValue)
		return E_POINTER;

	*ppValue = NULL;

	CAspirinApp* app = (CAspirinApp*)AfxGetApp();

	CSnmpCounters data;
	app->GetAspirinCounters().GetCounters(data);

	StreamSizeCounter sizeCounter;

	try
	{
		boost::archive::binary_oarchive oa(sizeCounter);
		oa << data;
	}
	catch (std::exception& e)
	{
		//ERRORLOG(e.what());
		return S_FALSE;
	}

	size_t size = sizeCounter.GetSize();

	SAFEARRAY* psa = SafeArrayCreateVector(VT_UI1, 0, size);
	if (psa == NULL)
		return E_OUTOFMEMORY;

	void* pData = NULL;

	HRESULT hr = SafeArrayAccessData(psa, &pData);
	if (!SUCCEEDED(hr))
		return hr;

	try
	{
		ArrayOutputStream ostream((char*)pData, size);

		boost::archive::binary_oarchive oa2(ostream);
		oa2 << data;
	}
	catch (std::exception& e)
	{
		//ERRORLOG(e.what());
		return S_FALSE;
	}

	SafeArrayUnaccessData(psa);

	*ppValue = psa;

	return S_OK;
}