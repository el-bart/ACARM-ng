/*
 * timestampFromString.cpp
 *
 */
#include <ctime>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cassert>

#include "Persistency/IO/Postgres/timestampFromString.hpp"

using namespace boost::posix_time;

namespace Persistency
{
namespace IO
{
namespace Postgres
{

Timestamp timestampFromString(const std::string &str)
{
  const ptime  pt    =time_from_string(str);
  tm           tmTime=to_tm(pt);            // cannot be const, for mktime()
  const time_t tt    =mktime(&tmTime);
  assert(tmTime.tm_isdst>=0);               // daylight saving should be now determined
  // make some extra calculations to remove time zone effects generated by time_from_string()
  const int    gmtOff=tmTime.tm_gmtoff;     // time-zone offset
  const time_t final =tt + gmtOff;          // apply all fixes
  return Persistency::Timestamp(final);     // return final time
} // timestampFromString()

} // namespace Postgres
} // namespace IO
} // namespace Persistency
