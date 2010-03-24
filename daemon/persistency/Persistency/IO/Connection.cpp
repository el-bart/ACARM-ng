/*
 * Connection.cpp
 *
 */
#include <cassert>

#include "Persistency/IO/Connection.hpp"

namespace Persistency
{
namespace IO
{

Connection::~Connection(void)
{
}

TransactionAPIAutoPtr Connection::createNewTransaction(const std::string &name)
{
  TransactionAPIAutoPtr ptr=createNewTransactionImpl(mutex_, name);
  assert( ptr.get()!=NULL );
  return ptr;
}

AlertAutoPtr Connection::alert(AlertPtrNN alert, Transaction &t)
{
  AlertAutoPtr ptr=alertImpl(alert, t);
  assert( ptr.get()!=NULL );
  return ptr;
}

HostAutoPtr Connection::host(HostPtrNN host, Transaction &t)
{
  HostAutoPtr ptr=hostImpl(host, t);
  assert( ptr.get()!=NULL );
  return ptr;
}

MetaAlertAutoPtr Connection::metaAlert(MetaAlertPtrNN ma, Transaction &t)
{
  MetaAlertAutoPtr ptr=metaAlertImpl(ma, t);
  assert( ptr.get()!=NULL );
  return ptr;
}

RestorerAutoPtr Connection::restorer(Transaction &t)
{
  RestorerAutoPtr ptr=restorerImpl(t);
  assert( ptr.get()!=NULL );
  return ptr;
}

size_t Connection::removeEntriesOlderThan(size_t days, Transaction &t)
{
  return removeEntriesOlderThanImpl(days, t);
}

} // namespace IO
} // namespace Persistency
