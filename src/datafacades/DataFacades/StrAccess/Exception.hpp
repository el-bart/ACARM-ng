/*
 * Exception.hpp
 *
 */
#ifndef INCLUDE_DATAFACADES_STRACCESS_EXCEPTION_HPP_FILE
#define INCLUDE_DATAFACADES_STRACCESS_EXCEPTION_HPP_FILE

/* public header */

#include "Persistency/Exception.hpp"

namespace Persistency
{
namespace Facades
{
namespace StrAccess
{

/** \brief base exception for string-access facade.
 */
class Exception: public Persistency::Exception
{
public:
  /** \brief create execption with given message.
   *  \param where   place where exception has been rised.
   *  \param details details of conversion problem.
   */
  Exception(const Location &where, const std::string &details);
}; // class ExceptionConversionError

} // namespace StrAccess
} // namespace Facades
} // namespace Persistency

#endif
