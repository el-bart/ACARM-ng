/*
 * IOStubs.t.cpp
 *
 */
#ifndef INCLUDE_IOSTUBS_T_HPP_FILE
#define INCLUDE_IOSTUBS_T_HPP_FILE

#include <tut.h>

#include "Persistency/IO/Connection.hpp"
#include "Persistency/IO/Transaction.hpp"
#include "Persistency/IO/TestTransactionAPI.t.hpp"
#include "Persistency/IO/IOStubs.t.hpp"
#include "Persistency/TestHelpers.t.hpp"

namespace
{

class IOAlert: public Persistency::IO::Alert
{
public:
  IOAlert(Persistency::AlertPtrNN       alert,
          Persistency::IO::Transaction &t):
    Persistency::IO::Alert(alert, t),
    alert_(alert),
    calls_(0)
  {
  }

  virtual void saveImpl(Persistency::IO::Transaction&)
  {
    ++calls_;
    tut::ensure("invalid object", &get()==alert_.get() );
  }

  Persistency::AlertPtrNN alert_;
  int                     calls_;
}; // class IOAlert


class IOHost: public Persistency::IO::Host
{
public:
  IOHost(Persistency::HostPtrNN        host,
         Persistency::IO::Transaction &t):
    Persistency::IO::Host(host, t),
    host_(host)
  {
  }

  virtual void setNameImpl(Persistency::IO::Transaction &,
                           const Persistency::Host::Name &/*name*/)
  {
    ++calls_;
    tut::ensure("invalid host", &get()==host_.get() );
  }

  Persistency::HostPtr host_;
  int                  calls_;
}; // class IOHost

class IOMetaAlert: public Persistency::IO::MetaAlert
{
public:
  IOMetaAlert(Persistency::MetaAlertPtrNN   ma,
              Persistency::IO::Transaction &t):
    Persistency::IO::MetaAlert(ma, t),
    ma_(ma)
  {
    tut::ensure("invalid meta alert", &get()==ma_.get() );

    for(unsigned int i=0; i<sizeof(called_)/sizeof(called_[0]); ++i)
      called_[i]=0;
  }

  virtual void saveImpl(Persistency::IO::Transaction &)
  {
    ++called_[0];
  }
  virtual void markAsTriggeredImpl(Persistency::IO::Transaction &, const std::string &)
  {
    ++called_[7];
  }
  virtual void markAsUsedImpl(Persistency::IO::Transaction &)
  {
    ++called_[1];
  }
  virtual void markAsUnusedImpl(Persistency::IO::Transaction &)
  {
    ++called_[2];
  }
  virtual void updateSeverityDeltaImpl(Persistency::IO::Transaction &, double /*delta*/)
  {
    ++called_[3];
  }
  virtual void updateCertaintyDeltaImpl(Persistency::IO::Transaction &, double /*delta*/)
  {
    ++called_[4];
  }
  virtual void addChildImpl(Persistency::IO::Transaction &, Persistency::MetaAlertPtrNN /*child*/)
  {
    ++called_[5];
  }
  virtual void associateWithAlertImpl(Persistency::IO::Transaction &, Persistency::AlertPtrNN /*alert*/)
  {
    ++called_[6];
  }

  Persistency::MetaAlertPtr ma_;
  int                       called_[8];
}; // class MetaAlert


struct TestIOConnection: public Persistency::IO::Connection
{
  TestIOConnection(void)
  {
    for(unsigned int i=0; i<sizeof(called_)/sizeof(called_[0]); ++i)
      called_[i]=0;
  }

  virtual Persistency::IO::TransactionAPIAutoPtr createNewTransactionImpl(
                                        Base::Threads::Mutex &mutex,
                                        const std::string    &name)
  {
    ++called_[0];
    return Persistency::IO::TransactionAPIAutoPtr( new TestTransactionAPI(mutex, name) );
  }
  virtual Persistency::IO::AlertAutoPtr alertImpl(
                                        Persistency::AlertPtrNN       alert,
                                        Persistency::IO::Transaction &t)
  {
    ++called_[1];
    return Persistency::IO::AlertAutoPtr( new IOAlert(alert, t) );
  }
  virtual Persistency::IO::HostAutoPtr hostImpl(
                                        Persistency::HostPtrNN        host,
                                        Persistency::IO::Transaction &t)
  {
    ++called_[2];
    return Persistency::IO::HostAutoPtr( new IOHost(host, t) );
  }
  virtual Persistency::IO::MetaAlertAutoPtr metaAlertImpl(
                                        Persistency::MetaAlertPtrNN   ma,
                                        Persistency::IO::Transaction &t)
  {
    ++called_[3];
    return Persistency::IO::MetaAlertAutoPtr( new IOMetaAlert(ma, t) );
  }

  int called_[4];
}; // struct TestIOConnection

} // unnamed namespace

#endif
