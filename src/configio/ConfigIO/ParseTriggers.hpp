/*
 * ParseTriggers.hpp
 *
 */
#ifndef INCLUDE_CONFIGIO_PARSETRIGGERS_HPP_FILE
#define INCLUDE_CONFIGIO_PARSETRIGGERS_HPP_FILE

#include "XML/Node.hpp"
#include "ConfigIO/TriggerConfig.hpp"
#include "ConfigIO/TriggersConfigCollection.hpp"
#include "ConfigIO/Generic/Parse.hpp"


namespace ConfigIO
{

/** \brief parser for triggers' configuration.
 */
class ParseTriggers: public Generic::Parse<TriggerConfig, TriggersConfigCollection, true>
{
public:
  /** \brief parses configuration and saves it internally.
   *  \param node node to start parsing from.
   */
  explicit ParseTriggers(const XML::Node &node);
}; // class ParaseTriggers

} // namespace ConfigIO

#endif
