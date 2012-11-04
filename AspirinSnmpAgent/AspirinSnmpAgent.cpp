
#include "stdafx.h"
#include <snmp.h>
#include "CounterTables.h"
#include <fstream>

#include <atlbase.h>
#include <atlstr.h>

#include "..\include\SnmpCounters.h"
#include "..\include\StreamImpl.h"


std::ofstream logger;

#import "..\AspirinSnmpCommunicator\_AspirinSnmpCommunicator.tlb" no_namespace, raw_interfaces_only


//////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////

int GetNextRequest(TAspirinCounterTables& aspirinCounterTables, SnmpVarBind* pVarBind);
int GetRequest(TAspirinCounterTables& aspirinCounterTables, SnmpVarBind* pVarBind);

#define OID_SIZEOF( Oid )      ( sizeof Oid / sizeof(UINT) )
// this is the our branch starting point (clled prefix)
UINT g_unMyOIDPrefix[]	= {1, 3, 6, 1, 4, 1, 15};

UINT AspirinEntryOIDPrefix[]	= {1, 3, 6, 1, 4, 1, 15, 1 };
UINT AspirinTablesEntryOIDPrefix[]	= {1, 3, 6, 1, 4, 1, 15, 1, 1, 1 };
UINT AspirinNextOIDPrefix[]	= {1, 3, 6, 1, 4, 1, 15, 3};

#define ASPIRIN_OID_PREFIX_TEMPL {1, 3, 6, 1, 4, 1, 15, 1, 1, 1, 1, 1 }

const int TABLE_INDEX_POSITION = OID_SIZEOF(AspirinEntryOIDPrefix);
const int ENTRY_INDEX_POSITION = TABLE_INDEX_POSITION + 1;
const int FIELD_INDEX_POSITION = TABLE_INDEX_POSITION + 2;
const int ROW_INDEX_POSITION = TABLE_INDEX_POSITION + 3;
const int MAX_OID_LENGTH = 12;

AsnObjectIdentifier AspirinTablesOids[MAX_TABLE_COUNT];

AsnObjectIdentifier MIB_OidPrefix = { OID_SIZEOF(g_unMyOIDPrefix), g_unMyOIDPrefix };

BOOL APIENTRY DllMain(HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


void LogEvent(CString data)
{
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	logger << SystemTime.wMonth << '/'
		   << SystemTime.wDay << '/'
		   << SystemTime.wYear << '\t'
		   << SystemTime.wHour << ':'
		   << SystemTime.wMinute << ':'
		   << SystemTime.wSecond << '\t'
		   << data << std::endl;
}

void LogComError(CString sMsg, HRESULT hr)
{
	char buff[512];

	BOOL bRet = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hr, 0, buff, sizeof(buff), 0);

	CString res = "Error: " + sMsg + &buff[0];

	LogEvent(res);
}

void InitLogger()
{
	MEMORY_BASIC_INFORMATION mbi;
	HMODULE hExecutingModule = (VirtualQuery(SnmpExtensionInit, &mbi, sizeof(mbi)) != 0)
							   ? (HMODULE) mbi.AllocationBase : NULL;

	TCHAR szLibrary[MAX_PATH] = _T("");
	GetModuleFileName(hExecutingModule, szLibrary, MAX_PATH);
	PathRemoveFileSpec(szLibrary);

	std::string logFile = szLibrary;
	logFile += "\\Request.log";

	logger.open(logFile.c_str(), std::ios::out | std::ios::app);

}
// When exported function will be called during DLL loading and initialization
BOOL SNMP_FUNC_TYPE SnmpExtensionInit(DWORD dwUptimeReference, HANDLE* phSubagentTrapEvent, AsnObjectIdentifier* pFirstSupportedRegion)
{
	InitLogger();
	LogEvent("SnmpExtensionInit");
	*pFirstSupportedRegion = MIB_OidPrefix;

	CComUsageScope scope;

	CComPtr<ICommunicator> spComm;

	HRESULT hr = spComm.CoCreateInstance(__uuidof(CCommunicator));
	if (FAILED(hr))
	{
		LogComError("Creation of CCommunicator: ", hr);
		return SNMPAPI_ERROR;
	}

	return SNMPAPI_NOERROR;
}

void SetExitOid(SnmpVarBind* pVarBind)
{
	AsnObjectIdentifier tmp;
	tmp.idLength = TABLE_INDEX_POSITION + 1;
	tmp.ids = &AspirinNextOIDPrefix[0];
	SnmpUtilOidFree(&pVarBind->name);
	SnmpUtilOidCpy(&pVarBind->name, &tmp);
}

void GetDataFromAspirin(TAspirinCounterTables& AspirinCounterTables)
{
	LogEvent("Getting data from aspirins");
	AspirinTable& coreCounters = AspirinCounterTables[0];
	AspirinTable& homeCounters = AspirinCounterTables[1];
	AspirinTable& remoteCounters = AspirinCounterTables[2];

	coreCounters.clear();
	homeCounters.clear();
	remoteCounters.clear();

	CComUsageScope scope;

	CComPtr<ICommunicator> spComm;

	HRESULT hr = spComm.CoCreateInstance(__uuidof(CCommunicator));
	if (FAILED(hr))
	{
		LogComError("Creation of CCommunicator: ", hr);
		return;
	}

	SAFEARRAY* pResponse = NULL;
	hr = spComm->FireEvent(&pResponse);
	if (FAILED(hr))
	{
		LogComError("Getting communicator response: ", hr);
		return;
	}

	if (pResponse != NULL)
	{
		CSnmpCounters data;

		LONG iLBound, iUBound;
		HRESULT hr = SafeArrayGetLBound(pResponse, 1, &iLBound);
		if (FAILED(hr))
			return;
		hr = SafeArrayGetUBound(pResponse, 1, &iUBound);
		if (FAILED(hr))
			return;

		for (long i = iLBound; i <= iUBound; i++)
		{
			VARIANT theArray;
			VariantInit(&theArray);
			theArray.vt = VT_ARRAY | VT_UI1;

			SafeArrayGetElement(pResponse, &i, &theArray);

			SAFEARRAY* psa = theArray.parray;

			LONG iLBound1, iUBound1;
			HRESULT hr = SafeArrayGetLBound(psa, 1, &iLBound1);
			if (FAILED(hr))
				return;
			hr = SafeArrayGetUBound(psa, 1, &iUBound1);
			if (FAILED(hr))
				return;

			size_t size = iUBound1 - iLBound1 + 1;

			void* pData = NULL;

			SafeArrayAccessData(psa, &pData);

			ArrayInputStream istream((char*)pData, size);

			boost::archive::binary_iarchive ia(istream);
			ia >> data;

			SafeArrayUnaccessData(psa);

			SafeArrayDestroy(psa);

			data.m_coreCounters.insert(data.m_coreCounters.begin(), i + 1);
			data.m_homeCounters.insert(data.m_homeCounters.begin(), i + 1);
			data.m_remoteCounters.insert(data.m_remoteCounters.begin(), i + 1);
			coreCounters.push_back(data.m_coreCounters);
			homeCounters.push_back(data.m_homeCounters);
			remoteCounters.push_back(data.m_remoteCounters);
		}

		SafeArrayUnaccessData(pResponse);

		SafeArrayDestroy(pResponse);

	}
}

// this export is to query the MIB table and fields
BOOL SNMP_FUNC_TYPE SnmpExtensionQuery(BYTE bPduType, SnmpVarBindList* pVarBindList, AsnInteger32* pErrorStatus, AsnInteger32* pErrorIndex)
{
	*pErrorStatus = SNMP_ERRORSTATUS_NOERROR;
	*pErrorIndex = 0;

	TAspirinCounterTables aspirinCounterTables;
	GetDataFromAspirin(aspirinCounterTables);

	for (UINT i = 0; i < pVarBindList->len; i++)
	{
		*pErrorStatus = SNMP_ERRORSTATUS_NOERROR;

		if (SnmpUtilOidCmp(&MIB_OidPrefix, &pVarBindList->list[i].name) > 0)
		{
			if (bPduType == SNMP_PDU_GETNEXT && pVarBindList->len == 1)
			{
				AsnObjectIdentifier tmp;
				tmp.idLength = OID_SIZEOF(AspirinEntryOIDPrefix);
				tmp.ids = &AspirinEntryOIDPrefix[0];
				SnmpUtilOidFree(&pVarBindList->list[i].name);
				SnmpUtilOidCpy(&pVarBindList->list[i].name, &tmp);
			}
			else
			{
				SetExitOid(&pVarBindList->list[i]);
				*pErrorStatus = SNMP_ERRORSTATUS_NOSUCHNAME;
				*pErrorIndex++;
				continue;
			}
		}


		// what type of request we are getting?
		switch (bPduType)
		{
		case SNMP_PDU_GET:// // gets the variable value passed variable in pVarBindList
			*pErrorStatus = GetRequest(aspirinCounterTables, &pVarBindList->list[i]);
			if (*pErrorStatus != SNMP_ERRORSTATUS_NOERROR)
				*pErrorIndex++;
			break;
		case SNMP_PDU_GETNEXT: // gets the next variable related to the passed variable in pVarBindList
			*pErrorStatus = GetNextRequest(aspirinCounterTables, &pVarBindList->list[i]);
			if (*pErrorStatus != SNMP_ERRORSTATUS_NOERROR)
				*pErrorIndex++;
			break;
		case SNMP_PDU_SET: // sets a variable
			*pErrorStatus = SNMP_ERRORSTATUS_NOTWRITABLE;
			if (*pErrorStatus != SNMP_ERRORSTATUS_NOERROR)
				*pErrorIndex++;
			break;
		default:
			*pErrorStatus = SNMP_ERRORSTATUS_NOSUCHNAME;
			*pErrorIndex++;
		};
	}

	return SNMPAPI_NOERROR;
}

void LogSnmpVariableName(std::string str, SnmpVarBind* pVarBind)
{
	logger << str;
	for (UINT i = 0; i < pVarBind->name.idLength; i++)
	{
		logger << pVarBind->name.ids[i] << ".";
	}
	logger << std::endl;
}

// get the value of the variable passed
int GetRequest(TAspirinCounterTables& aspirinCounterTables, SnmpVarBind* pVarBind)
{
	int nRet = SNMP_ERRORSTATUS_NOSUCHNAME;

	if (pVarBind->name.idLength < TABLE_INDEX_POSITION
			|| aspirinCounterTables[0].empty()
			|| pVarBind->name.idLength != MAX_OID_LENGTH
			|| pVarBind->name.ids[ENTRY_INDEX_POSITION] != 1)
		return nRet;

	int tableIndex = pVarBind->name.ids[TABLE_INDEX_POSITION] - 1;
	int fieldIndex = pVarBind->name.ids[FIELD_INDEX_POSITION] - 1;
	int rowIndex = pVarBind->name.ids[ROW_INDEX_POSITION] - 1;

	if (tableIndex >= MAX_TABLE_COUNT || tableIndex < 0 ||
			rowIndex < 0 || aspirinCounterTables[tableIndex].size() <= rowIndex ||
			fieldIndex < 0 || fieldIndex >= aspirinCounterTables[tableIndex][rowIndex].size()
	   )
		return nRet;

	Converter cnvt(pVarBind->value);
	aspirinCounterTables[tableIndex][rowIndex][fieldIndex].apply_visitor(cnvt);

	return SNMP_ERRORSTATUS_NOERROR;
}

// get the next variable & value related to the passed variable
int GetNextRequest(TAspirinCounterTables& aspirinCounterTables, SnmpVarBind* pVarBind)
{
	if (aspirinCounterTables[0].empty())
	{
		// return other branch
		SetExitOid(pVarBind);
		return SNMP_ERRORSTATUS_NOERROR;
	}

	int tableIndex = 0;
	int fieldIndex = 0;
	int rowIndex = 0;

	if (pVarBind->name.idLength >= (TABLE_INDEX_POSITION)
			&& pVarBind->name.ids[TABLE_INDEX_POSITION - 1] == 0)
	{
		AsnObjectIdentifier tmp;
		tmp.idLength = OID_SIZEOF(AspirinEntryOIDPrefix);
		tmp.ids = &AspirinEntryOIDPrefix[0];
		SnmpUtilOidFree(&pVarBind->name);
		SnmpUtilOidCpy(&pVarBind->name, &tmp);
	}

	if (pVarBind->name.idLength >= (ENTRY_INDEX_POSITION + 1)
			&& pVarBind->name.ids[ENTRY_INDEX_POSITION] != 1)
	{
		UINT entryVal = pVarBind->name.ids[ENTRY_INDEX_POSITION];
		UINT tableVal = pVarBind->name.ids[TABLE_INDEX_POSITION];

		AsnObjectIdentifier tmp;
		tmp.idLength = OID_SIZEOF(AspirinTablesEntryOIDPrefix);
		tmp.ids = &AspirinTablesEntryOIDPrefix[0];
		SnmpUtilOidFree(&pVarBind->name);
		SnmpUtilOidCpy(&pVarBind->name, &tmp);

		if (entryVal > 1)
			pVarBind->name.ids[TABLE_INDEX_POSITION] = tableVal + 1;
		else
			pVarBind->name.ids[TABLE_INDEX_POSITION] = tableVal;
	}

	if (pVarBind->name.idLength == MAX_OID_LENGTH)
	{

		tableIndex = pVarBind->name.ids[TABLE_INDEX_POSITION] - 1;

		fieldIndex = pVarBind->name.ids[FIELD_INDEX_POSITION] - 1;

		rowIndex = pVarBind->name.ids[ROW_INDEX_POSITION] - 1;
		rowIndex++;

	}
	else
	{
		if (pVarBind->name.idLength >= TABLE_INDEX_POSITION + 1)
			tableIndex = pVarBind->name.ids[TABLE_INDEX_POSITION] - 1;

		if (pVarBind->name.idLength >= FIELD_INDEX_POSITION + 1)
			fieldIndex = pVarBind->name.ids[FIELD_INDEX_POSITION] - 1;

		if (pVarBind->name.idLength >= ROW_INDEX_POSITION + 1)
			rowIndex = pVarBind->name.ids[ROW_INDEX_POSITION] - 1;
	}

	if (tableIndex >= MAX_TABLE_COUNT)
	{
		// return other branch
		SetExitOid(pVarBind);
		return SNMP_ERRORSTATUS_NOERROR;
	}

	if (tableIndex < 0)
	{
		tableIndex = 0;
		rowIndex = 0;
		fieldIndex = 0;
	}

	if (aspirinCounterTables[tableIndex].size() <= rowIndex)
	{
		rowIndex = 0;
		++fieldIndex;
	}

	if (rowIndex < 0)
	{
		rowIndex = 0;
		fieldIndex = 0;
	}

	if (fieldIndex >= aspirinCounterTables[tableIndex][rowIndex].size())
	{
		fieldIndex = 0;
		rowIndex = 0;
		++tableIndex;
	}

	if (fieldIndex < 0)
	{
		fieldIndex = 0;
	}

	if (tableIndex >= MAX_TABLE_COUNT)
	{
		// return other branch
		SetExitOid(pVarBind);
		return SNMP_ERRORSTATUS_NOERROR;
	}

	AspirinTableCell& theCell = aspirinCounterTables[tableIndex][rowIndex][fieldIndex];

	Converter cnvt(pVarBind->value);
	theCell.apply_visitor(cnvt);

    UINT AspirinOIDPrefixTempl[] = ASPIRIN_OID_PREFIX_TEMPL;
	AspirinOIDPrefixTempl[TABLE_INDEX_POSITION] = tableIndex + 1;
	AspirinOIDPrefixTempl[FIELD_INDEX_POSITION] = fieldIndex + 1;
	AspirinOIDPrefixTempl[ROW_INDEX_POSITION] = rowIndex + 1;

	AsnObjectIdentifier tmp;
	tmp.idLength = MAX_OID_LENGTH;
	tmp.ids = &AspirinOIDPrefixTempl[0];
	SnmpUtilOidFree(&pVarBind->name);
	SnmpUtilOidCpy(&pVarBind->name, &tmp);

	return SNMP_ERRORSTATUS_NOERROR;
}

BOOL SNMP_FUNC_TYPE SnmpExtensionTrap(AsnObjectIdentifier* pEnterpriseOid,
									  AsnInteger32* pGenericTrapId,
									  AsnInteger32* pSpecificTrapId,
									  AsnTimeticks* pTimeStamp,
									  SnmpVarBindList* pVarBindList)
{
	return FALSE;
}
