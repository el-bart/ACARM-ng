/*
 * EntrySaver.cpp
 *
 */
#include <sstream>
#include <cassert>

#include "Persistency/IO/Postgres/TransactionAPI.hpp"
#include "Persistency/IO/Postgres/detail/EntrySaver.hpp"
#include "Persistency/IO/Postgres/detail/Appender.hpp"

using namespace std;
using namespace pqxx;

namespace Persistency
{
namespace IO
{
namespace Postgres
{
namespace detail
{

namespace
{
template <typename T>
void addToSelect(std::stringstream &ss, const T *ptr)
{
  if(ptr!=NULL)
  {
    ss << " = ";
    Appender::append(ss, ptr->get() );
  }
  else
    ss << " IS NULL";
}
} //unnamed namespace

EntrySaver::EntrySaver(Transaction &t, DBHandler &dbh):
  dbh_(dbh),
  t_(t)
{
}

DataBaseID EntrySaver::saveProcess(DataBaseID reportedHostID, const Process &p)
{
  const DataBaseID procID=saveProcessData(p);
  return saveReportedProcessData(reportedHostID, procID, p);
}

DataBaseID EntrySaver::getID(const std::string &seqName)
{
  assert( seqName==pqxx::sqlesc(seqName) && "invalid sequence name" );

  const std::string sql="SELECT currval('" + seqName + "') as id;";
  result r=t_.getAPI<Postgres::TransactionAPI>().exec(sql);
  assert( r.size()==1 && "unable to read current sequence number" );

  DataBaseID id;
  r[0]["id"].to(id);
  return id;
}

DataBaseID EntrySaver::getSeverityID(const Alert &a)
{
  const DataBaseID id = a.getSeverity().getLevel().toInt();
  assert(id >= 1 && id <= 6);
  return id;
}

void EntrySaver::addReferenceURL(std::stringstream &ss, const ReferenceURL *url)
{
  if( url!=NULL )
  {
    const DataBaseID urlID=saveReferenceURL( *url );
    ss << urlID;
  }
  else
    ss << "NULL";
}

// TODO: return NullValue<> instead of auto_ptr<> - there is no need to make extra allocations here
std::auto_ptr<DataBaseID> EntrySaver::isAnalyzerInDataBase(const Analyzer &a)
{
  DataBaseID id;
  stringstream ss;
  ss << "SELECT * FROM analyzers WHERE name = ";
  Appender::append(ss, a.getName().get() );
  ss << " AND version ";
  addToSelect(ss, a.getVersion() );
  ss << " AND os";
  addToSelect(ss, a.getOS() );
  ss << " AND ip";
  Appender::append(ss, a.getIP() );
  ss << ";";
  // TODO: this variable should be const
  result r=t_.getAPI<Postgres::TransactionAPI>().exec(ss);
  if(r.empty() )
    return std::auto_ptr<DataBaseID>();
  else
  {
    r[0]["id"].to(id);
    return std::auto_ptr<DataBaseID>(new DataBaseID(id));
  }
}

DataBaseID EntrySaver::saveProcessData(const Process &p)
{
  stringstream ss;
  ss << "INSERT INTO procs(path, name, md5) VALUES (";
  Appender::append(ss, p.getPath().get() );
  ss << ",";
  Appender::append(ss, p.getName().get() );
  ss << ",";
  Appender::append(ss, (p.getMD5())?p.getMD5()->get():NULL);
  ss << ");";
  // insert object to data base.
  t_.getAPI<Postgres::TransactionAPI>().exec(ss);

  return getID("procs_id_seq");
}

DataBaseID EntrySaver::saveReportedProcessData(DataBaseID     reportedHostID,
                                               DataBaseID     procID,
                                               const Process &p)
{
  stringstream ss;
  ss << "INSERT INTO reported_procs("
        "id_reported_host, id_proc, pid, uid, username, arguments, id_ref"
        ") VALUES (";
  ss << reportedHostID << ",";
  ss << procID << ",";
  Appender::append(ss, p.getPID() );
  ss << ",";
  Appender::append(ss, p.getUID() );
  ss << ",";
  Appender::append(ss, p.getUsername().get() );
  ss << ",";
  Appender::append(ss, p.getParameters() );
  ss << ",";
  addReferenceURL(ss, p.getReferenceURL() );
  ss << ");";
  // insert object to data base.
  t_.getAPI<Postgres::TransactionAPI>().exec(ss);

  return getID("reported_procs_id_seq");
}

DataBaseID EntrySaver::saveReferenceURL(const ReferenceURL &url)
{
  stringstream ss;
  ss << "INSERT INTO reference_urls(name, url) VALUES (";
  Appender::append(ss, url.getName().get() );
  ss << ",";
  Appender::append(ss, url.getURL().get() );
  ss << ");";
  // insert object to data base.
  t_.getAPI<Postgres::TransactionAPI>().exec(ss);

  return getID("reference_urls_id_seq");
}

DataBaseID EntrySaver::saveHostData(const Persistency::Host &h)
{
  stringstream ss;
  ss << "INSERT INTO hosts(ip, mask, os, name) VALUES (";
  Appender::append(ss, h.getIP().to_string() );
  ss << ",";
  Appender::append(ss, h.getNetmask()->to_string() );
  ss << ",";
  Appender::append(ss, h.getOperatingSystem().get() );
  ss << ",";
  Appender::append(ss, h.getName().get() );
  ss << ");";
  t_.getAPI<Postgres::TransactionAPI>().exec(ss);

  return getID("hosts_id_seq");
}

DataBaseID EntrySaver::saveReportedHostData(DataBaseID               alertID,
                                            DataBaseID               hostID,
                                            const std::string        role,
                                            const Persistency::Host &h)
{
  stringstream ss;
  ss << "INSERT INTO reported_hosts(id_alert, id_host, role, id_ref) VALUES (";
  ss << alertID << ",";
  ss << hostID << ",";
  assert(role=="src" || role=="dst");
  Appender::append(ss, role);
  ss << ",";
  Appender::append(ss, h.getReferenceURL()?saveReferenceURL( *h.getReferenceURL() ):NULL);
  ss << ");";
  t_.getAPI<Postgres::TransactionAPI>().exec(ss);
  return getID("reported_hosts_id_seq");
}

DataBaseID EntrySaver::saveTargetHost(DataBaseID hostID, DataBaseID alertID, const Persistency::Host &h)
{
  return saveReportedHostData(alertID, hostID, "dst", h);
}

DataBaseID EntrySaver::saveSourceHost(DataBaseID hostID, DataBaseID alertID, const Persistency::Host &h)
{
  return saveReportedHostData(alertID, hostID, "src", h);
}

DataBaseID EntrySaver::saveAlert(const Persistency::Alert &a)
{
  stringstream ss;
  ss << "INSERT INTO alerts(name, detect_time, create_time, id_severity, certanity, description) VALUES (";
  Appender::append(ss, a.getName().get() );
  ss << ",";
  Appender::append(ss, a.getDetectionTime()?a.getDetectionTime():NULL);
  ss << ",";
  Appender::append(ss, a.getCreationTime() );
  ss << ",";
  const DataBaseID sevID = getSeverityID(a);
  Appender::append(ss, sevID);
  ss << ",";
  Appender::append(ss, a.getCertainty().get() );
  ss << ",";
  Appender::append(ss, a.getDescription() );
  ss << ");";
  t_.getAPI<Postgres::TransactionAPI>().exec(ss);
  return getID("alerts_id_seq");
}

DataBaseID EntrySaver::saveAnalyzer(const Analyzer &a)
{
  std::auto_ptr<DataBaseID> id = isAnalyzerInDataBase(a);
  if( id.get() == NULL)
  {
    stringstream ss;
    ss << "INSERT INTO analyzers(name, version, os, ip) VALUES (";
    Appender::append(ss, a.getName().get() );
    ss << ",";
    Appender::append(ss, a.getVersion()?a.getVersion()->get():NULL);
    ss << ",";
    Appender::append(ss, a.getOS()?a.getOS()->get():NULL );
    ss << ",";
    Appender::append(ss, a.getIP()?( a.getIP()->to_string().c_str() ):NULL);
    ss << ");";
    t_.getAPI<Postgres::TransactionAPI>().exec(ss);
    return getID("analyzers_id_seq");
  }
  return *id.get();
}

DataBaseID EntrySaver::saveServiceData(const Service &s)
{
  stringstream ss;
  ss << "INSERT INTO services(name, port, protocol) VALUES (";
  Appender::append(ss, s.getName().get() );
  ss << ",";
  Appender::append(ss, s.getPort() );
  ss <<",";
  Appender::append(ss, s.getProtocol().get() );
  ss << ");";
  t_.getAPI<Postgres::TransactionAPI>().exec(ss);

  return getID("services_id_seq");

}

void EntrySaver::saveReportedServiceData(DataBaseID     reportedHostID,
                                         DataBaseID     serID,
                                         const Service &s)
{
  stringstream ss;
  ss << "INSERT INTO reported_services(id_reported_host, id_service, id_ref) VALUES (";
  ss << reportedHostID << ",";
  ss << serID << ",";
  addReferenceURL(ss, s.getReferenceURL() );
  ss << ");";
  t_.getAPI<Postgres::TransactionAPI>().exec(ss);
}

DataBaseID EntrySaver::saveService(DataBaseID reportedHostID, const Service &s)
{
  DataBaseID serID = saveServiceData(s);
  saveReportedServiceData(reportedHostID, serID, s);
  return serID;
}

DataBaseID EntrySaver::saveMetaAlert(const Persistency::MetaAlert &ma)
{
  stringstream ss;
  ss << "INSERT INTO meta_alerts(name, severity_delta, certanity_delta, id_ref, create_time, last_update_time) VALUES (";
  Appender::append(ss, ma.getName().get() );
  ss << ",";
  Appender::append(ss, ma.getSeverityDelta() );
  ss << ",";
  Appender::append(ss, ma.getCertaintyDelta() );
  ss << ",";
  addReferenceURL(ss, ma.getReferenceURL() );
  ss << ",";
  Appender::append(ss, ma.getCreateTime() );
  ss << ",";
  Appender::append(ss, "now()");
  ss << ");";
  t_.getAPI<Postgres::TransactionAPI>().exec(ss);

  return getID("meta_alerts_id_seq");
}

void EntrySaver::saveAlertToMetaAlertMap(DataBaseID alertID, DataBaseID malertID)
{
  stringstream ss;
  ss << "INSERT INTO alert_to_meta_alert_map(id_alert, id_meta_alert) VALUES(";
  Appender::append(ss, alertID);
  ss << ",";
  Appender::append(ss, malertID);
  ss << ");";
  t_.getAPI<Postgres::TransactionAPI>().exec(ss);
}

void EntrySaver::saveAlertToAnalyzers(DataBaseID alertID, DataBaseID anlzID)
{
  stringstream ss;
  ss << "INSERT INTO alert_analyzers(id_alert, id_analyzer) VALUES (";
  Appender::append(ss, alertID);
  ss << ",";
  Appender::append(ss, anlzID);
  ss << ");";
  t_.getAPI<Postgres::TransactionAPI>().exec(ss);
}


void EntrySaver::saveMetaAlertsTree(DataBaseID nodeID, DataBaseID childID)
{
  stringstream ss;
  ss << "INSERT INTO meta_alerts_tree(id_node, id_child) VALUES (";
  Appender::append(ss, nodeID);
  ss << ",";
  Appender::append(ss, childID);
  ss << ");";
  t_.getAPI<Postgres::TransactionAPI>().exec(ss);
}

void EntrySaver::saveMetaAlertAsUsed(DataBaseID malertID)
{
  stringstream ss;
  ss << "INSERT INTO meta_alerts_in_use(id_meta_alert) VALUES (" << malertID << ");";
  t_.getAPI<Postgres::TransactionAPI>().exec(ss);
}

void EntrySaver::saveMetaAlertAsUnused(DataBaseID malertID)
{
  stringstream ss;
  ss << "DELETE FROM meta_alerts_in_use WHERE id_meta_alert = " << malertID << ";";
  t_.getAPI<Postgres::TransactionAPI>().exec(ss);
}

void EntrySaver::saveMetaAlertAsTriggered(DataBaseID malertID, const std::string &name)
{
  stringstream ss;
  ss << "INSERT INTO meta_alerts_already_triggered(id_meta_alert_in_use, trigger_name) VALUES(";
  Appender::append(ss, malertID);
  ss << ",";
  Appender::append(ss, name);
  ss << ");";
  t_.getAPI<Postgres::TransactionAPI>().exec(ss);
}

void EntrySaver::updateSeverityDelta(DataBaseID malertID, double severityDelta)
{
  stringstream ss;
  ss << "UPDATE meta_alerts SET severity_delta = severity_delta + ";
  Appender::append(ss, severityDelta);
  ss << " WHERE id = " << malertID << ";";
  t_.getAPI<Postgres::TransactionAPI>().exec(ss);
}

void EntrySaver::updateCertaintyDelta(DataBaseID malertID, double certanityDelta)
{
  stringstream ss;
  ss << "UPDATE meta_alerts SET certanity_delta = certanity_delta + ";
  Appender::append(ss, certanityDelta);
  ss << " WHERE id = " << malertID << ";";
  t_.getAPI<Postgres::TransactionAPI>().exec(ss);
}

} // namespace detail
} // namespace Postgres
} // namespace IO
} // namespace Persistency
