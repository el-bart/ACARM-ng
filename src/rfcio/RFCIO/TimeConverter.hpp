/*
 * TimeConverter.hpp
 *
 */
#ifndef INCLUDE_RFCIO_XMLCREATOR_HPP_FILE
#define INCLUDE_RFCIO_XMLCREATOR_HPP_FILE

#include <string>
#include <utility>
#include <inttypes.h>

#include "Persistency/Timestamp.hpp"
#include "RFCIO/Exception.hpp"

namespace RFCIO
{

/** \brief time converting class.
 *
 *  converter between timestamps in GNU epoch format (used internally by system)
 *  and string time values used by IDMEF (RFC-4765), as defined in RFC-1305.
 *
 *  notice that in some cases convertion may fail silently, i.e. when timestamp
 *  cannot be represented in ntpstamp, it is zeroed since RFC-1305 does provide
 *  format that represents invalid/unset value.
 */
class TimeConverter
{
public:
  /** \brief exception thrown on invalid time string or timestamp.
   */
  class ExceptionInvalidTime: public Exception
  {
  public:
    /** \brief create exception on invalid convertion of timestamp.
     *  \param where  place where exception has been thrown.
     *  \param ts     timestamp that has been found invalid.
     *  \param reason why string is invalid.
     */
    ExceptionInvalidTime(const Location &where, const Persistency::Timestamp &ts, const std::string &reason):
      Exception(where, cc("timestamp is invalid for given convertion: '", ts.str(), "' - ", reason) )
    {
    }
    /** \brief create execption on invalid string.
     *  \param where  place where exception has been thrown.
     *  \param str    string with supposed timestamp to convert.
     *  \param reason why string is invalid.
     */
    ExceptionInvalidTime(const Location &where, const std::string &str, const std::string &reason):
      Exception(where, cc("timestamp is invalid for given convertion: '", str, "' - ", reason) )
    {
    }
  }; // class ExceptionInvalidTime


  /** \brief return value from convertion string to timestamp.
   *
   *  first element is regular timestamp, used internally by system. second
   *  element is fraction of second, that did not fit into timestamp. fraction
   *  is ensured to be in [0; 1) range.
   *  \return ?return required because of doyxgen bug?
   */
  typedef std::pair<Persistency::Timestamp, double> ExactTimestamp;

  /** \brief create object and initialize data.
   */
  TimeConverter(void);

  /** \brief create string representation in RFC-1305 ntpstamp format.
   *  \param t timestamp to create ntpstamp from.
   *  \return string representation of a given timestamp.
   */
  std::string toNtpStamp(const Persistency::Timestamp &t) const;
  /** \brief create human-readable string representation in format required by IDMEF.
   *  \param t timestamp to create ntpstamp from.
   *  \return string representation of a given timestamp.
   */
  std::string toString(const Persistency::Timestamp &t) const;

  /** \brief create ExactTimestamp representation from RFC-1305 ntpstamp format.
   *  \param str string containing ntpstamp.
   *  \return ExactTimestamp converted from ntpstamp.
   */
  ExactTimestamp fromNtpStamp(const std::string &str) const;
  /** \brief create ExactTimestamp representation from format required by IDMEF.
   *  \param str string to convert timestamp from.
   *  \return ExactTimestamp converted from string.
   */
  ExactTimestamp fromString(const std::string &str) const;

private:
  const int64_t the1900_;
}; // class TimeConverter

} // namespace RFCIO

#endif
