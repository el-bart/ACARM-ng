/*
 * Exception.hpp
 *
 */
#ifndef INCLUDE_TRIGGER_EXCEPTION_HPP_FILE
#define INCLUDE_TRIGGER_EXCEPTION_HPP_FILE

/* public header */

#include "Commons/Exception.hpp"

namespace Trigger
{
/** \brief base for all trigger-related exceptions.
 */
class Exception: public Commons::Exception
{
public:
  /** \brief create execption with given message.
   *  \param where place where exception has been thrown.
   *  \param msg   message to represent.
   */
  template<typename T>
  Exception(const Location &where, const T &msg):
    Commons::Exception(where, msg)
  {
  }
}; // class Exception

} // namespace Trigger

#endif
