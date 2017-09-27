#pragma once

#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <string>
#include <vector>

#include <sys/timeb.h>
#include <time.h>

typedef boost::variant<std::string, LONG, _timeb> AspirinTableCell;
typedef std::vector<AspirinTableCell> AspirinTableRecord;
typedef std::vector<AspirinTableRecord> AspirinTable;
const int MAX_TABLE_COUNT = 3;
typedef AspirinTable TAspirinCounterTables[MAX_TABLE_COUNT];



class Converter : public boost::static_visitor<>
{
public:
	explicit Converter(AsnAny& val) : m_val(val) {}

	void operator()(LONG val) const
	{
		m_val.asnType = ASN_INTEGER;
		m_val.asnValue.number = val;
	}
	void operator()(const std::string& val) const
	{
		m_val.asnType = ASN_OCTETSTRING;

		m_val.asnValue.string.length = (UINT)val.length();
		m_val.asnValue.string.stream = (unsigned char*)SnmpUtilMemAlloc(m_val.asnValue.string.length * sizeof(char));

		memcpy(m_val.asnValue.string.stream, val.data(), m_val.asnValue.string.length);
		m_val.asnValue.string.dynamic = TRUE;
	}

	void operator()(const _timeb& val) const
	{
		char* time;
		if (val.time == 0)
		{
			time = "n/a";
		}
		else
		{
#if _MSC_VER >= 1400
			char timeline[26];
			ctime_s(timeline, 26, & (val.time));
#else
			char* timeline = ctime(&val.time);
#endif
			time = timeline;
		}

		size_t length = strlen(time);

		m_val.asnType = ASN_OCTETSTRING;

		m_val.asnValue.string.length = (UINT)length;
		m_val.asnValue.string.stream = (unsigned char*)SnmpUtilMemAlloc(m_val.asnValue.string.length * sizeof(char));

		memcpy(m_val.asnValue.string.stream, time, m_val.asnValue.string.length);
		m_val.asnValue.string.dynamic = TRUE;
	}

private:
	AsnAny& m_val;
};