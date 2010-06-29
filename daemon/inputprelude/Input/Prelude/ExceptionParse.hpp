/*
 * ExceptionParse.hpp
 *
 */
#ifndef INCLUDE_INPUT_PRELUDE_PARSEEXCEPTION_HPP_FILE
#define INCLUDE_INPUT_PRELUDE_PARSEEXCEPTION_HPP_FILE

#include <string>

#include "Input/Exception.hpp"

namespace Input
{
namespace Prelude
{

/** \brief parse exception
 */
class ExceptionParse: public Input::Exception
{
public:
  /** \brief create execption with given message.
   *  \param where place where exception has been thrown.
   *  \param msg   message to represent.
   */
  template<typename T>
  ExceptionParse(const Location &where, const T &msg):
    Input::Exception(where, cc("parse exception: ",msg))
  {
  }
}; // class Exception

} // namespace Prelude
} // namespace Input

#endif