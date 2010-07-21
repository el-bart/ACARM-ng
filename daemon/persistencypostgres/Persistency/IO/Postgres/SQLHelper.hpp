/*
 * SQLHelper.hpp
 *
 */

#ifndef INCLUDE_PERSISTENCY_IO_POSTGRES_SQLHELPER_HPP_FILE
#define INCLUDE_PERSISTENCY_IO_POSTGRES_SQLHELPER_HPP_FILE

#include <pqxx/pqxx>

#include "Logger/Logger.hpp"
#include "Persistency/IO/Transaction.hpp"
#include "Persistency/IO/Postgres/TransactionAPI.hpp"

namespace Persistency
{
namespace IO
{
namespace Postgres
{
/** \brief helper class to exec SQL query and write logs.
 */
class SQLHelper: private boost::noncopyable
{
public:
  /** \brief create helper.
   *  \param file file name.
   *  \param line line number.
   *  \param sql  SQL query.
   *  \param log  logger object.
   */
  SQLHelper(const char *file, int line, const std::string &sql, const Logger::Node &log):
    file_(file),
    line_(line),
    sql_(sql),
    log_(log)
  {

  }
  /** \brief execute SQL query
   *  \param t associated transaction.
   */
  inline pqxx::result exec(Transaction &t)
  {
    LOGMSG_DEBUG_S(log_)<<"file "<<file_<<" line "<<line_<<" [SQL-exec]: "<<sql_;
    assert(sql_.find("=NULL") || sql_.find("= NULL"));
    return t.getAPI<Postgres::TransactionAPI>().exec(sql_);
  }
private:
  const char        *file_;
  int                line_;
  const std::string &sql_;
  Logger::Node       log_;
}; // class SQLHelper

} // namespace Postgres
} // namespace IO
} // namespace Persistency
#endif
