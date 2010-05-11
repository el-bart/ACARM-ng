/*
 * EntrySaver.hpp
 *
 */
#ifndef INCLUDE_PERSISTENCY_IO_POSTGRES_DETAIL_ENTRYSAVER_HPP_FILE
#define INCLUDE_PERSISTENCY_IO_POSTGRES_DETAIL_ENTRYSAVER_HPP_FILE

#include <boost/noncopyable.hpp>

#include "Base/NullValue.hpp"
#include "Logger/Node.hpp"
#include "Persistency/Process.hpp"
#include "Persistency/Service.hpp"
#include "Persistency/ReferenceURL.hpp"
#include "Persistency/Host.hpp"
#include "Persistency/Analyzer.hpp"
#include "Persistency/Alert.hpp"
#include "Persistency/MetaAlert.hpp"
#include "Persistency/IO/Transaction.hpp"
#include "Persistency/IO/Postgres/DataBaseID.hpp"
#include "Persistency/IO/Postgres/DBHandler.hpp"
#include "Persistency/IO/Postgres/ExceptionNoEntries.hpp"

// TODO: add loging here:
//         - debug logs while saving each element
//         - info logs while in main algorithm parts
//         - detailed errors while error conditions are detected.

namespace Persistency
{
namespace IO
{
namespace Postgres
{
namespace detail
{

/** \brief helper class to save particular elements in data base.
 */
class EntrySaver: private boost::noncopyable
{
public:
  /** \brief create saver.
   *  \param t   transaction to be used for saving.
   *  \param dbh data base handler object.
   */
  EntrySaver(Transaction &t, DBHandler &dbh);

  /** \brief saves process data to data base.
   *  \param reportedHostID ID of reported host, that this Process should be assigned to.
   *  \param p              Process to be saved.
   */
  DataBaseID saveProcess(DataBaseID reportedHostID, const Process &p);
  /** \brief saves alert data to data base
   *  \param a          Alert to be saved.
   */
  DataBaseID saveAlert(const Persistency::Alert &a);
  /** \brief saves  analyzer data to data base
   *  \param a      Analyzer to be saved.
   */
  DataBaseID saveAnalyzer(const Analyzer &a);
  /** \brief save destination host data to data base
   *  \param hostID  ID of Host,
   *  \param alertID ID of Alert, that destination Host shoul be assigned to.
   *  \param h       Destination Host to be saved.
   */
  DataBaseID saveSourceHost(DataBaseID hostID, DataBaseID alertID, const Persistency::Host &h);
  /** \brief save target host data to data base
   *  \param hostID  ID of Host,
   *  \param alertID ID of Alert, that source Host shoul be assigned to.
   *  \param h       Target Host to be saved.
   */
  DataBaseID saveTargetHost(DataBaseID hostID, DataBaseID alertID, const Persistency::Host &h);
  /** \brief save service data to data base
   *  \param reportedHostID ID of reported host, that this Process should be assigned to.
   *  \param s              Service to be saved.
   */
  DataBaseID saveService(DataBaseID reportedHostID, const Service &s);
  /** \brief save host data to data base
   *  \param h host to be saved
   */
  DataBaseID saveHostData(const Persistency::Host &h);
  /**\brief save Meta Alert data to data base
   * \param ma Meta Alert to be saved
   */
  DataBaseID saveMetaAlert(const Persistency::MetaAlert &ma);

  /** \brief save meta alerts tree
   *  \param nodeID node ID
   *  \param childID child associated with node
   */
  void saveMetaAlertsTree(DataBaseID nodeID, DataBaseID childID);

  /** \brief save Alert to Meta Alert map
   *  \param alertID  ID of Alert
   *  \param malertID ID of Meta Alert
   */
  void saveAlertToMetaAlertMap(DataBaseID alertID, DataBaseID malertID);

  /** \brief save Alert to Analyzers map
   *  \param alertID ID of Alert
   *  \param anlzID  ID of Analyzer
   */
  void saveAlertToAnalyzers(DataBaseID alertID, DataBaseID anlzID);

  /** \brief mark Meta Alert as used
   *  \param malertID ID of Meta Alert
   */
  void markMetaAlertAsUsed(DataBaseID malertID);

  /** \brief mark Meta Alert as unused
   *  \param malertID ID of Meta Alert
   */
  void markMetaAlertAsUnused(DataBaseID malertID);

  /** \brief mark Meta Alert as triggered
   *  \param malertID ID of Meta Alert
   *  \param name
   */
  void markMetaAlertAsTriggered(DataBaseID malertID, const std::string &name);

  /** \brief update Meta Alert severity delta
   *  \param malertID ID   of Meta Alert to update
   *  \param severityDelta value of severity delta to write in data base
   */
  void updateSeverityDelta(DataBaseID malertID, double severityDelta);

  /** \brief update Meta Alert certanity delta
   *  \param malertID ID    of Meta Alert to update
   *  \param certanityDelta value of certanity delta to write in data base
   */
  void updateCertaintyDelta(DataBaseID malertID, double certanityDelta);

  /** \brief set Host name in the data base
   *  \param hostID ID of Host to set name
   *  \param name   name of Host
   */
  void setHostName(DataBaseID hostID, const Persistency::Host::Name &name);

private:
  DataBaseID getID(const std::string &seqName);
  DataBaseID getSeverityID(const Alert &a);
  Base::NullValue<DataBaseID> isAnalyzerInDataBase(const Analyzer &a);
  std::string addIPToSelect(const Analyzer::IP *ptr);

  DataBaseID saveProcessData(const Process &p);
  DataBaseID saveReportedProcessData(DataBaseID     reportedHostID,
                                     DataBaseID     procID,
                                     const Process &p);
  DataBaseID saveReferenceURL(const ReferenceURL &url);

  DataBaseID saveAlertData(const Alert &a);
  DataBaseID saveMetaAlertData(const MetaAlert &ma);

  DataBaseID saveReportedHostData(DataBaseID               alertID,
                                  DataBaseID               hostID,
                                  const char               *role,
                                  const Persistency::Host &h);

  DataBaseID saveServiceData(const Service &s);
  void saveReportedServiceData(DataBaseID     reportedHostID,
                               DataBaseID     serID,
                               const Service &s);
  void addReferenceURL(std::stringstream &ss, const ReferenceURL *url);
  bool isHostNameNull(DataBaseID hostID);

  pqxx::result execSQL(const std::string &sql, int line);

  Logger::Node  log_;
  DBHandler    &dbh_;
  Transaction  &t_;
}; // class EntrySaver

} // namespace detail
} // namespace Postgres
} // namespace IO
} // namespace Persistency

#endif
