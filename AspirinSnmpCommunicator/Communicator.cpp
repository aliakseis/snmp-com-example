#include "stdafx.h"
#include "Communicator.h"
#include <vector>


STDMETHODIMP CCommunicator::FireEvent(SAFEARRAY** value)
{
	if (NULL == value)
		return E_POINTER;

	*value = NULL;

	HRESULT hrRet = S_OK;
	IConnectionPointImpl<CCommunicator, &__uuidof(IEvents), CComDynamicUnkArray>* p = this;

	std::vector<VARIANT> varArray;

	int idx = 0;
	for (;;)
	{
		Lock();
		IUnknown** pp = p->m_vec.begin() + idx;
		if (pp >= p->m_vec.end())
		{
			Unlock();
			break;
		}
		CComPtr<IDispatch> pDispatch = (IDispatch*) *pp;
		Unlock();

		if (pDispatch != NULL)
		{
			SAFEARRAY* pResponse = NULL;

			VARIANT rgvars[1];
			::VariantInit(&rgvars[0]);
			rgvars[0].vt = (VT_ARRAY | VT_UI1 | VT_BYREF);
			V_ARRAYREF(&rgvars[0]) = &pResponse;
			DISPPARAMS disp = { rgvars, NULL, 1, 0 };
			VARIANT ret_val;
			HRESULT hr = __ComInvokeEventHandler(pDispatch, 1, 1, &disp, &ret_val);

			if (FAILED(hr))
			{
				hrRet = hr;
				if (0x800706BA == hr) //The RPC server is unavailable
				{
					Lock();
					p->m_vec.Remove(p->m_vec.GetCookie(pp)); // Does not shift array items
					Unlock();
				}
			}
			else if (pResponse != NULL)
			{
				VARIANT v_arr;
				VariantInit(&v_arr);
				v_arr.vt = VT_ARRAY | VT_UI1;
				v_arr.parray = pResponse;
				varArray.push_back(v_arr);
			}
		}
		idx++;
	}

	size_t size = varArray.size();
	if (size != 0)
	{
		SAFEARRAY* psa = SafeArrayCreateVector(VT_VARIANT, 0, size);

        void* pData = NULL;
        SafeArrayAccessData(psa, &pData);

        memcpy(pData, &varArray[0], sizeof(VARIANT) * size);
        
        SafeArrayUnaccessData(psa); 

        *value = psa;
	}
	return hrRet;
}
