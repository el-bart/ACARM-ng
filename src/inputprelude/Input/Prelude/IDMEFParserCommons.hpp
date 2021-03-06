/*
 * IDMEFParserSource.hpp
 *
 */
#ifndef INCLUDE_INPUT_PRELUDE_IDMEFPARSERCOMMONS_HPP_FILE
#define INCLUDE_INPUT_PRELUDE_IDMEFPARSERCOMMONS_HPP_FILE

#include <prelude.h>
#include "Base/NullValue.hpp"
#include "Input/Prelude/ExceptionParse.hpp"
#include "Persistency/IPTypes.hpp"
#include "Persistency/Alert.hpp"
#include "Logger/Logger.hpp"

namespace Input
{
namespace Prelude
{
/**
 * \brief Parsing for some common fields
 */
class IDMEFParserCommons : public Persistency::IPTypes<IDMEFParserCommons>
{
public:
  /**
   * \brief get IP address from idmef_node_t object
   * \param idmef_node object to parse
   * \return return IP from idmef node.
   */
  static Base::NullValue<IP> getIPfromIdmefNode(idmef_node_t * idmef_node);

  /**
   * \brief get Service from idmef_service_t object
   * \param idmef_service object to parse
   * \return return service
   */
  static Persistency::ServicePtr getServicefromIdmefService(idmef_service_t * idmef_service);

  /**
   * \brief get Process from idmef_process_t object
   * \param idmef_process object to parse
   * \param idmef_user additional data if available
   * \return return process
   */
  static Persistency::ProcessPtr getProcessfromIdmefProcess(idmef_process_t * idmef_process, idmef_user_t * idmef_user);
}; // class IDMEFParserCommons

} // namespace Prelude
} // namespace Input

#endif
