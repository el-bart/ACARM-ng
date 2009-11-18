/*
 * Host.cpp
 *
 */

#include "Persistency/Postgres/Host.hpp"

namespace Persistency
{
namespace Postgres
{
Host::Host(const IPv4              &ip,
           const Netmask_v4        *mask,
           const OperatingSystem    os,
           ReferenceURLPtr          url,
           const ReportedServices  &services,
           const ReportedProcesses &processes):
  Persistency::Host(ip, mask, os, url, services, processes)
{
}

Host::Host(const IPv6              &ip,
           const Netmask_v6        *mask,
           const OperatingSystem    os,
           ReferenceURLPtr          url,
           const ReportedServices  &services,
           const ReportedProcesses &processes):
  Persistency::Host(ip, mask, os, url, services, processes)
{
}

Host::Name Host::getName(void) const
{
  // TODO
  return NULL;
}

void Host::setName(const Host::Name &/*name*/)
{
  // TODO
}

} // namespace Postgres
} // namespace Persistency
