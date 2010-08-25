/*
 * Sources.cpp
 *
 */
#include "Logger/Logger.hpp"
#include "Persistency/IDAssigner.hpp"
#include "Persistency/IO/BackendFactory.hpp"
#include "Core/Sources.hpp"

using namespace Persistency;

namespace Core
{

namespace
{
Input::CommonDataPtrNN createInputsCommonData(Persistency::IO::ConnectionPtrNN conn)
{
  // in a signle transaction read content required for common data and return created object
  Persistency::IO::Transaction t( conn->createNewTransaction("input_commondata_read") );
  Input::CommonDataPtrNN       cdPtr( new Input::CommonData(conn, t) );
  t.commit();           // commit transaction to prevent strange warnings in logs.
  return cdPtr;
} // createInputsCommonData()
} // unnamed namespace

Sources::Sources(void):
  log_("core.sources"),
  conn_( IO::create() ),
  inputs_( Input::create( queue_, createInputsCommonData(conn_) ) )
{
  LOGMSG_INFO(log_, "created");
}

Sources::~Sources(void)
{
  LOGMSG_INFO(log_, "deallocating");
}

Persistency::GraphNodePtrNN Sources::read(void)
{
  // read alert from input(s)
  LOGMSG_DEBUG(log_, "awaiting new alert");
  const AlertPtrNN alert=readAlert();
  LOGMSG_DEBUG(log_, "got new alert");

  // write it to data base along with creating proper graph-node object.
  IO::Transaction t( conn_->createNewTransaction("core_save_graphnode") );
  LOGMSG_DEBUG(log_, "new transaction opened");
  GraphNodePtrNN  leaf( new GraphNode(alert, IDAssigner::get()->assign(conn_, t), conn_, t) );
  LOGMSG_DEBUG(log_, "creating object done - commiting transaction");
  t.commit();
  LOGMSG_INFO(log_, "alert and meta-alert successfuly written to data base");

  return leaf;
}

Persistency::AlertPtrNN Sources::readAlert(void)
{
  // loop here ensures that reading will not stop when alert has been read
  // but rejected by preprocessor
  for(;;)
  {
    LOGMSG_DEBUG(log_, "loop waiting for new alert, that preprocessor accepts");
    const AlertPtrNN alert=queue_.pop();
    if( preproc_.checkAccept(*alert) )
    {
      LOGMSG_DEBUG(log_, "alert accepted by preprocessor");
      return alert;
    }
    LOGMSG_DEBUG(log_, "alert rejected by preprocessor - awaiting new one");
  } // for(true)
}

} // namespace Core
