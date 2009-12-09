/*
 * ExceptionUnexpectedNULL.hpp
 *
 */
#ifndef INCLUDE_COMMONS_EXCEPTIONUNEXPECTEDNULL_HPP_FILE
#define INCLUDE_COMMONS_EXCEPTIONUNEXPECTEDNULL_HPP_FILE

/* public header */

#include <string>

#include "Commons/Exception.hpp"

namespace Commons
{

/** \brief exception thrown by non-NULL smart pointer's implementaions.
 */
class ExceptionUnexpectedNULL: public Exception
{
public:
  /** \brief create execption with given message.
   *  \param where place where exception has been rised.
   */
  ExceptionUnexpectedNULL(const char *where):
    Exception(where, std::string("unexpected NULL pointer received") )
  {
  }
}; // class ExceptionUnexpectedNULL

}; // namespace Commons

#endif
