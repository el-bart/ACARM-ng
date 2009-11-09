/*
 * ExceptionNullBuilder.hpp
 *
 */
#ifndef INCLUDE_COMMONS_FACTORY_EXCEPTIONNULLBUILDER_HPP_FILE
#define INCLUDE_COMMONS_FACTORY_EXCEPTIONNULLBUILDER_HPP_FILE

/* public header */

#include "Commons/Factory/Exception.hpp"

namespace Commons
{
namespace Factory
{

/** \brief exception on NULL builder given
 */
class ExceptionNullBuilder: public Commons::Factory::Exception
{
public:
  /** \brief create execption with given message.
   *  \param msg message to represent.
   */
  explicit ExceptionNullBuilder(const char *where):
    Commons::Factory::Exception(where, "builder is NULL")
  {
  }
}; // class ExceptionNullBuilder

} // namespace Factory
} // namespace Commons

#endif
