/*
 * Host.hpp
 *
 */
#include "Persistency/IO/Postgres/Host.hpp"

namespace Persistency
{
namespace IO
{
namespace Postgres
{

Host::Host(Persistency::HostPtrNN  host,
           const Transaction      &t,
           DBSharedConnection      sc):
  IO::Host(host, t),
  sc_(sc)
{
}

void Host::setNameImpl(const Persistency::Host::Name &/*name*/)
{
  // TODO
}

} // namespace Postgres
} // namespace IO
} // namespace Persistency
