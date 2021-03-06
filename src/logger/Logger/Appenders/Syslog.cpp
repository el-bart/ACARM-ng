/*
 * Syslog.cpp
 *
 */
#include <syslog.h>

#include "Logger/Appenders/Syslog.hpp"

namespace Logger
{
namespace Appenders
{

void Syslog::appendImpl(const std::string &str)
{
  syslog(LOG_INFO, "%s", str.c_str() );
}

void Syslog::reinitImpl(void)
{
  // nothing to be done here
}

const char *Syslog::getTypeNameImpl(void) const
{
  return Syslog::getThisTypeName();
}

} // namespace Appenders
} // namespace Logger
