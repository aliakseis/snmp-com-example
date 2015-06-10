#include "stdafx.h"
#include "AspirinCounters.h"
#include "..\include\SnmpCounters.h"

#include <string>

using std::string;

CAspirinCounters::CAspirinCounters()
	: m_openAspirins(0), m_started(0), m_running(0),
	  m_adaptersNumber(0), m_linesNumber(0), m_inboundLines(0), m_outboundLines(0),
	  m_inboundLinesInUse(0),
	  m_outboundLinesInUse(0),
	  m_processedInboundCalls(0),
	  m_processedOutboundCalls(0),
	  m_failedOutboundCalls(0),
	  m_failedInboundLogins(0),
	  m_portNum(0),
	  m_remoteProcessedRequests(0),
	  m_lastRemote10Min(0), m_lastRemote30Min(0), m_lastRemote120Min(0), m_lastRemote6Hours(0), m_lastRemote24Hours(0)

{
	memset(&m_startTime, 0, sizeof(m_startTime));
	memset(&m_inboundCallCompleteTime, 0, sizeof(m_inboundCallCompleteTime));
	memset(&m_outboundCallCompleteTime, 0, sizeof(m_outboundCallCompleteTime));
	memset(&m_lastRequestCompleteTime, 0, sizeof(m_lastRequestCompleteTime));
}

CAspirinCounters::~CAspirinCounters()
{
}

///////////////////////////////////////////////////////////////////////

void CAspirinCounters::GetCounters(CSnmpCounters& snmpData)
{
	CSingleLock singleLock(&m_dataCritSec, TRUE);

	CString szFileName;
	GetModuleFileName(NULL, szFileName.GetBuffer(MAX_PATH), MAX_PATH);
	szFileName.ReleaseBuffer();
	int index = szFileName.ReverseFind('\\');
	szFileName.Delete(0, index + 1);
	CString szAppName;
	szAppName.Format(_T("%s (%u)"), szFileName, GetCurrentProcessId());
	string appName = CStringA(szAppName);

	snmpData.m_coreCounters.push_back(appName);
	snmpData.m_coreCounters.push_back(m_startTime);
	snmpData.m_coreCounters.push_back(m_openAspirins);
	snmpData.m_coreCounters.push_back(string(CStringA(m_connString)));
	snmpData.m_coreCounters.push_back(m_started);
	snmpData.m_coreCounters.push_back(m_running);

	snmpData.m_homeCounters.push_back(appName);
	snmpData.m_homeCounters.push_back(m_adaptersNumber);
	snmpData.m_homeCounters.push_back(m_linesNumber);
	snmpData.m_homeCounters.push_back(m_inboundLines);
	snmpData.m_homeCounters.push_back(m_outboundLines);
	snmpData.m_homeCounters.push_back(m_inboundLinesInUse);
	snmpData.m_homeCounters.push_back(m_outboundLinesInUse);
	snmpData.m_homeCounters.push_back(m_processedInboundCalls);
	snmpData.m_homeCounters.push_back(m_processedOutboundCalls);
	snmpData.m_homeCounters.push_back(m_inboundCallCompleteTime);
	snmpData.m_homeCounters.push_back(m_outboundCallCompleteTime);
	snmpData.m_homeCounters.push_back(string(CStringA(m_inboundCallAspirinName)));
	snmpData.m_homeCounters.push_back(string(CStringA(m_outboundCallAspirinName)));
	snmpData.m_homeCounters.push_back(m_failedOutboundCalls);
	snmpData.m_homeCounters.push_back(m_failedInboundLogins);

	snmpData.m_remoteCounters.push_back(appName);
	string condition = m_portNum ? "UP" : "DOWN";
	snmpData.m_remoteCounters.push_back(condition);
	snmpData.m_remoteCounters.push_back(m_portNum);
	snmpData.m_remoteCounters.push_back(m_remoteProcessedRequests);
	snmpData.m_remoteCounters.push_back(m_lastRequestCompleteTime);
	snmpData.m_remoteCounters.push_back(string(CStringA(m_lastProcessedAspirinName)));
	snmpData.m_remoteCounters.push_back(m_lastRemote10Min);
	snmpData.m_remoteCounters.push_back(m_lastRemote30Min);
	snmpData.m_remoteCounters.push_back(m_lastRemote120Min);
	snmpData.m_remoteCounters.push_back(m_lastRemote6Hours);
	snmpData.m_remoteCounters.push_back(m_lastRemote24Hours);
}
