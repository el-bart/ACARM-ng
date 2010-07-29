/*
 * QueueRestorer.cpp
 *
 */
#include <cassert>

#include "Logger/Logger.hpp"
#include "Core/QueueRestorer.hpp"

using namespace Persistency::IO;


namespace Core
{

QueueRestorer::QueueRestorer(Persistency::IO::ConnectionPtrNN conn, Core::Types::SignedNodesFifo &queue)
{
  const Logger::Node log("core.queuerestorer");
  try
  {
    LOGMSG_INFO(log, "restoring data base's content");

    // preapre environment
    assert( conn.get()!=NULL );
    Transaction           t( conn->createNewTransaction("meta_alerts_queue_restore") );
    RestorerAutoPtr       rest=conn->restorer(t);
    // read data base content
    Restorer::NodesVector tmp;
    LOGMSG_DEBUG(log, "reading meta-alerts");
    rest->restoreAllInUse(tmp);
    t.commit();
    // add all elements to queue
    LOGMSG_DEBUG_S(log)<<tmp.size()<<" meta-alerts read from persistency; appending to queue";
    for(Restorer::NodesVector::const_iterator it=tmp.begin(); it!=tmp.end(); ++it)
      // TODO: this is hardcoded value of unknown origin of the alert - it should be rethought...
      queue.push( Types::SignedNode(*it, "*unknown*") );

    LOGMSG_INFO_S(log)<<"restoring "<<tmp.size()<<" elements successful";
  }
  catch(const std::exception &ex)
  {
    LOGMSG_ERROR_S(log)<<"unable to restore data base's content; error was: "
                       <<ex.what()<<"; continuing any way...";
  }
}

} // namespace Core
