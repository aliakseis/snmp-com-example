#pragma once

#include <sys/timeb.h>

struct CSnmpCounters;


class CAspirinCounters
{
public:
	CCriticalSection m_dataCritSec;

	_timeb m_startTime;
	_timeb m_inboundCallCompleteTime;
	_timeb m_outboundCallCompleteTime;
	_timeb m_lastRequestCompleteTime;

	DWORD m_openAspirins, m_started, m_running;
	CString m_connString;

	DWORD m_adaptersNumber, m_linesNumber, m_inboundLines, m_outboundLines;

	DWORD m_inboundLinesInUse;
	DWORD m_outboundLinesInUse;
	DWORD m_processedInboundCalls;
	DWORD m_processedOutboundCalls;

	CString m_inboundCallAspirinName;
	CString m_outboundCallAspirinName;

	DWORD m_failedOutboundCalls;
	DWORD m_failedInboundLogins;

	DWORD m_portNum;

	DWORD m_remoteProcessedRequests;
	CString m_lastProcessedAspirinName;

	DWORD m_lastRemote10Min, m_lastRemote30Min, m_lastRemote120Min, m_lastRemote6Hours, m_lastRemote24Hours;

public:
	CAspirinCounters(void);
	~CAspirinCounters(void);

	void GetCounters(CSnmpCounters& snmpData);
};
