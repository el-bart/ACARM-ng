/*
 * Connection.cpp
 *
 */
#include <sstream>

#include "Logger/Logger.hpp"
#include "Persistency/IO/Postgres/Connection.hpp"
#include "Persistency/IO/Postgres/TryCatchInAPI.hpp"

using namespace std;


namespace Persistency
{
namespace IO
{
namespace Postgres
{

// helper calls
namespace
{

// passing anything to this call ignores argument
template<typename T>
inline void ignore(const T &)
{
} // ignore()

inline pqxx::result execSQL(const Logger::Node &log, Transaction &t, const char *sql)
{
  LOGMSG_DEBUG_S(log)<<"calling SQL statement: "<<sql;
  ignore(log);      // this suppresses warning in release mode about unsued parameter
  const pqxx::result r=t.getAPI<TransactionAPI>().exec(sql);
  LOGMSG_DEBUG_S(log)<<"affected rows: "<<r.affected_rows();
  return r;
} // execSQL()


void createTempTable(Transaction &t, const Logger::Node &log, const std::string &name, const char *sql=NULL)
{
  // create temporary table
  {
    stringstream ss;
    ss << "CREATE TEMP TABLE " << name;
    if(sql==NULL)
      ss << "(id int NOT NULL)";
    ss << " ON COMMIT DROP";
    if(sql!=NULL)
      ss << " AS (" << sql << ")";
    execSQL(log, t, ss.str().c_str() );
  }

  // create index for this table
  {
    stringstream ss;
    ss << "CREATE INDEX " << name << "_id_index ON " << name << "(id)";
    execSQL(log, t, ss.str().c_str() );
  }
}


void createTemporaryTables(size_t days, Transaction &t, const Logger::Node &log)
{
  // build and execute first statement, that creates sql query for gathering
  // IDs to be removed.
  {
    stringstream ss;
    ss << "SELECT id FROM alerts WHERE create_time < now() - interval '"
       << days << " day' AND id NOT IN"
          "      ( SELECT id_alert FROM alert_to_meta_alert_map WHERE id_meta_alert IN"
          "        ( SELECT id_meta_alert FROM meta_alerts_in_use ) ORDER BY id_alert"
          "      )";
    createTempTable(t, log, "cleanup_alerts_ids", ss.str().c_str() );
  }

  // reported hosts' IDs
  createTempTable(t, log, "cleanup_hosts_ids",
                  "SELECT id FROM hosts WHERE id_alert IN (SELECT id FROM cleanup_alerts_ids)");

  // meta alerts' IDs
  createTempTable(t, log, "cleanup_meta_alerts_ids",
                  "SELECT id_meta_alert AS id FROM alert_to_meta_alert_map WHERE id_alert IN (SELECT id FROM cleanup_alerts_ids)");

  // TODO: add debug check if cleanup_alerts_ids and cleanup_meta_alerts_ids have equal count
}


void removeReportedServices(Transaction &t, const Logger::Node &log)
{
  execSQL(log, t, "DELETE FROM services WHERE id_host IN (SELECT id FROM cleanup_hosts_ids)");
}


void removeReportedProcs(Transaction &t, const Logger::Node &log)
{
  execSQL(log, t, "DELETE FROM procs WHERE id_host IN (SELECT id FROM cleanup_hosts_ids)");
}


void removeReportedHosts(Transaction &t, const Logger::Node &log)
{
  execSQL(log, t, "DELETE FROM hosts WHERE id IN (SELECT id FROM cleanup_hosts_ids)");
}


void removeAnalyzers(Transaction &t, const Logger::Node &log)
{
  execSQL(log, t, "DELETE FROM alert_analyzers WHERE id_alert IN (SELECT id FROM cleanup_alerts_ids)");

  // TODO: following query takes a lot of time (remove it?)
  execSQL(log, t, "DELETE FROM analyzers WHERE id "
                  " NOT IN (SELECT id_analyzer FROM alert_analyzers ORDER BY id_analyzer)");
}


size_t removeAlerts(Transaction &t, const Logger::Node &log)
{
  execSQL(log, t, "DELETE FROM alert_to_meta_alert_map WHERE id_alert "
                  " IN (SELECT id FROM cleanup_alerts_ids)");
  // finaly remove all alerts, that are not used
  const size_t removed=execSQL(log, t, "DELETE FROM alerts WHERE id "
                                       " IN (SELECT id FROM cleanup_alerts_ids)").affected_rows();
  return removed;
}


void removeExtraMetaAlertsEntries(Transaction &t, const Logger::Node &log)
{
  // table holding IDs that are to be checked, ifthey can be removed or not
  createTempTable(t, log, "cleanup_candidates_to_remove_ma_ids");
  // table containing IDs that are known to be removed from previous iterations. initially
  // filled up with leafs to be deleted.
  createTempTable(t, log, "cleanup_new_to_remove_ma_ids", "SELECT * FROM cleanup_meta_alerts_ids");

#warning THIS CODE DOES NOT WORK AS IT SHOULD YET...
  size_t affected;
  bool   firstRun=true;
  do
  {
    // save all parents of childrent to be removed - if they stay empty, they are to be removed as well!
    execSQL(log, t, "INSERT INTO cleanup_candidates_to_remove_ma_ids"
                    "  SELECT id_node AS id FROM meta_alerts_tree WHERE id_child IN"
                    "    (SELECT id FROM cleanup_new_to_remove_ma_ids)");
    // remove all tree entries, related to meta-alerts to be removed
    execSQL(log, t, "DELETE FROM meta_alerts_tree WHERE id_child IN (SELECT id FROM cleanup_new_to_remove_ma_ids)");
    // preapre table to accept new data
    execSQL(log, t, "DELETE FROM cleanup_new_to_remove_ma_ids");
    // check which parents are no longer in use, thus should be marked for removal
    affected=execSQL(log, t, "INSERT INTO cleanup_new_to_remove_ma_ids"
                             "  SELECT id FROM cleanup_candidates_to_remove_ma_ids WHERE"
                             "    id NOT IN (SELECT id_node  FROM meta_alerts_tree ORDER BY id_node)"
                             "    AND"
                             "    id NOT IN (SELECT id_child FROM meta_alerts_tree ORDER BY id_child)").affected_rows();
    // after the first run is done copy new IDs to be removed to main table. we skip this step
    // the first time, since cleanup_new_to_remove_ma_ids has initial value of cleanup_meta_alerts_ids
    // thus it makes no sense to copy data again.
    if(!firstRun)
      execSQL(log, t, "INSERT INTO cleanup_meta_alerts_ids SELECT id FROM cleanup_new_to_remove_ma_ids");
    else
      firstRun=false;
  }
  while(affected>0);

  // when no more IDs are to be removed from tree, remove meta-alerts them selfes
  execSQL(log, t, "DELETE FROM meta_alerts WHERE id IN (SELECT id FROM cleanup_meta_alerts_ids)");
}

} // unnamed namespace



Connection::Connection(DBHandlePtrNN handle):
  detail::ConnectionBase(handle),
  log_("persistency.io.postgres.connection")
{
}

size_t Connection::removeEntriesOlderThanImpl(size_t days, Transaction &t)
{
  TRYCATCH_BEGIN
    createTemporaryTables(days, t, log_);
    removeReportedServices(t, log_);
    removeReportedProcs(t, log_);
    removeReportedHosts(t, log_);
    removeAnalyzers(t, log_);
    const size_t removed=removeAlerts(t, log_);
    removeExtraMetaAlertsEntries(t, log_);
    return removed;
  TRYCATCH_END
}

} // namespace Postgres
} // namespace IO
} // namespace Persistency
