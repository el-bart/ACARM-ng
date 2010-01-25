/*
 * Exception.hpp
 *
 */
#ifndef INCLUDE_LOGGER_EXCEPTION_HPP_FILE
#define INCLUDE_LOGGER_EXCEPTION_HPP_FILE

/* public header */

#include "System/Exceptions/RuntimeError.hpp"

namespace Logger
{

/** \brief internal exception class.
 */
class Exception: public System::Exceptions::RuntimeError<Exception>
{
public:
  /** \brief create execption with given message.
   *  \param where place where exception has been rised.
   *  \param msg   message to represent.
   */
  template<typename T>
  explicit Exception(const Location &where, const T &msg):
    System::Exceptions::RuntimeError<Exception>(where, msg)
  {
  }

protected:
  /** \brief method that always returns valid string.
   *  \param str string to be checked.
   *  \return original string, if it is not NULL and "<?>" otherwise.
   */
  const char *ensureValidString(const char *str)
  {
    if(str==NULL)
      return "<?>";
    return str;
  }
}; // class Exception

}; // namespace Logger

#endif
