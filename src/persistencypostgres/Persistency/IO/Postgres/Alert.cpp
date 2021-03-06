/*
 * Alert.hpp
 *
 */
#include "Persistency/IO/Postgres/Alert.hpp"
#include "Persistency/IO/Postgres/TryCatchInAPI.hpp"

using namespace Persistency::IO::Postgres::detail;

namespace Persistency
{
namespace IO
{
namespace Postgres
{

Alert::Alert(Persistency::AlertPtrNN  alert,
             Transaction             &t,
             DBHandlePtrNN            dbHandle):
  IO::Alert(alert, t),
  log_("persistency.io.postgres.alert"),
  dbHandle_(dbHandle)
{
}

void Alert::saveImpl(Transaction &t)
{
  TRYCATCH_BEGIN
    EntrySaver                 es(t,*dbHandle_);
    //get Alert
    const Persistency::Alert  &a=*get();
    //save Alert
    DataBaseID                 alertID=es.saveAlert(a);
    //add Alert to cache
    dbHandle_->getIDCache()->add( get(), alertID );
    //save source hosts
    Persistency::Alert::Hosts SourceHosts( a.getSourceHosts() );
    saveHosts(es, alertID, HostType::SRC, SourceHosts);
    //save target hosts
    Persistency::Alert::Hosts TargetHosts( a.getTargetHosts() );
    saveHosts(es, alertID, HostType::DST, TargetHosts);
    //get Analyzers from Alert
    Persistency::Alert::Analyzers analyzers( a.getAnalyzers() );
    //save Analyzers
    for(Persistency::Alert::Analyzers::iterator it = analyzers.begin(); it != analyzers.end(); ++it)
    {
      //save Analyzer
      const Persistency::Analyzer &analyzer = *it->get();
      const DataBaseID             anlzID = es.saveAnalyzer(analyzer);
      es.saveAlertToAnalyzers(alertID, anlzID);
    }
  TRYCATCH_END
}

void Alert::saveHosts(EntrySaver                &es,
                      DataBaseID                 alertID,
                      HostType                   type,
                      Persistency::Alert::Hosts &hosts)
{
  // loop through all hosts
  for(Persistency::Alert::Hosts::iterator it = hosts.begin(); it!=hosts.end() ; ++it)
  {
    DataBaseID hostID;
    // save reported host
    switch( type.toInt() )
    {
      case HostType::SRC:
        LOGMSG_DEBUG(log_, "save source host with");
        hostID = es.saveSourceHost(alertID, *it->get() );
      break;

      case HostType::DST:
        LOGMSG_DEBUG(log_, "save target host with");
        hostID = es.saveTargetHost(alertID, *it->get() );
      break;

      default:
        hostID = 0;
        assert(!"we should never reach here! invalid unknwon host type - update the code!");
      break;
    } // switch(host_type)
    dbHandle_->getIDCache()->add(*it , hostID);
    LOGMSG_DEBUG_S(log_)<<"saved host with ID "<<hostID;

    // get reported services from host
    const Persistency::Host::Services &services( (*it->get()).getServices() );
    LOGMSG_DEBUG_S(log_)<<"save reported services for alert with ID: "<<alertID;
    for(Persistency::Host::Services::const_iterator it_s = services.begin(); it_s!=services.end(); ++it_s)
      es.saveService(hostID, *it_s->get() );

    // get reported processes from host
    const Persistency::Host::Processes &processes((*it->get()).getProcesses() );
    LOGMSG_DEBUG_S(log_)<<"save reported processes for alert with ID: "<<alertID;
    for(Persistency::Host::Processes::const_iterator it_p = processes.begin(); it_p!=processes.end(); ++it_p)
      es.saveProcess(hostID, *it_p->get() );
  } // for(hosts)
}

} // namespace Postgres
} // namespace IO
} // namespace Persistency
