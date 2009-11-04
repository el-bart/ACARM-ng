/*
 * ParsePersistency.hpp
 *
 */
#ifndef INCLUDE_CONFIGIO_PARSEPERSISTENCY_HPP_FILE
#define INCLUDE_CONFIGIO_PARSEPERSISTENCY_HPP_FILE

/* public header */

#include <string>

#include "XML/Node.hpp"
#include "ConfigIO/PersistencyConfig.hpp"

// TODO: test
// TODO: comment

namespace ConfigIO
{

/** \brief exception informing about invalid port number.
 */
struct ExceptionPortNumberConversionFailed: public Exception
{
  /** \brief create exception of no appenders defined
   *  \param where place where problem has been detected.
   *  \param port  port string that was not valid.
   */
  ExceptionPortNumberConversionFailed(const char        *where,
                                      const std::string  port):
    Exception( std::string( ensureValidString(where) ) +
               ": invalid port in config: " + port )
  {
  }
}; // struct ExceptionPortNumberConversionFailed



class ParsePersistency
{
public:
  explicit ParsePersistency(const XML::Node &node);

  const PersistencyConfig &getConfig(void) const
  {
    return cfg_;
  }

private:
  PersistencyConfig parse(const XML::Node &node) const;

  PersistencyConfig::PortNumber parsePort(const std::string &port) const;

  PersistencyConfig cfg_;
}; // class ParasePersistency

} // namespace ConfigIO

#endif
