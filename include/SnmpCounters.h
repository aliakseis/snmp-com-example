#pragma once


#include <boost/variant/variant.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/vector.hpp>

namespace boost
{
namespace serialization
{

template<class Archive>
void serialize(Archive& ar, _timeb& t, const unsigned int version)
{
    ar & t.dstflag & t.millitm & t.time & t.timezone;
}

} // namespace serialization
} // namespace boost

struct CSnmpCounters
{
    typedef std::vector<boost::variant<std::string, LONG, _timeb> > CountersList;

	CountersList m_coreCounters;
	CountersList m_homeCounters;
	CountersList m_remoteCounters;

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
        ar & m_coreCounters & m_homeCounters & m_remoteCounters;
	}
};
