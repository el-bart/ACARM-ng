/*
 * IDMEFParserTarget.cpp
 *
 */
#include "Input/Exception.hpp"
#include "IDMEFParserTarget.hpp"
#include "Persistency/Process.hpp"

namespace Input
{
namespace Prelude
{

IDMEFParserTarget::IDMEFParserTarget(idmef_target_t *ptr):ptr_(ptr)
{
  if (!ptr)
    throw Exception(SYSTEM_SAVE_LOCATION, "Idmef Target is empty.");
  
  Persistency::Process::Username username;
  
  idmef_user_t *idmef_user = idmef_target_get_user(ptr);
  if (idmef_user)
    {
      idmef_user_id_t *idmef_user_id = idmef_user_get_next_user_id(idmef_user, NULL);
      if (idmef_user_id)
	{
	  prelude_string_t *user_id_str = idmef_user_id_get_name(idmef_user_id);
	  if (user_id_str)
	    username=prelude_string_get_string(user_id_str);
	}
    }
  
  idmef_process_t *idmef_process = idmef_target_get_process(ptr);
  if (idmef_process)
    {
      Persistency::Process::Path path;
      prelude_string_t *idmef_path_str = idmef_process_get_path(idmef_process);
      
      if (idmef_path_str)
	path=prelude_string_get_string(idmef_path_str);      
      
      prelude_string_t *idmef_process_str = idmef_process_get_name(idmef_process);
      if (idmef_process_str)
	{
	  const Persistency::Process::Name name=prelude_string_get_string(idmef_process_str);
	  uint32_t *pid=idmef_process_get_pid(idmef_process);
	  pid_t pidt=*pid;
	  process_.reset(new Persistency::Process(path,name,NULL,&pidt,NULL,username,NULL,Persistency::ReferenceURLPtr()));
	}
    }
  
  idmef_node_t *idmef_node = idmef_target_get_node(ptr_);
  
  if (idmef_node)
    {
      idmef_address_t *idmef_node_addr = idmef_node_get_next_address(idmef_node, NULL);
      if (idmef_node_addr)
	{
	  const prelude_string_t *idmef_node_address = idmef_address_get_address(idmef_node_addr);
	  if (idmef_node_address)
	    ip_.reset(new Persistency::Analyzer::IP(boost::asio::ip::address_v6::from_string(prelude_string_get_string(idmef_node_address))));
	}
    }  
  
  idmef_service_t *idmef_service = idmef_target_get_service(ptr);
  if (idmef_service)
    {      
      prelude_string_t *idmef_service_name = idmef_service_get_name(idmef_service);
      if (idmef_service_name)
	{
	  Persistency::Service::Name name=prelude_string_get_string(idmef_service_name);
	  Persistency::Service::Protocol protocol;
	  
	  prelude_string_t *idmef_protocol_str = idmef_service_get_iana_protocol_name(idmef_service);
	  if (idmef_protocol_str)
	    protocol=prelude_string_get_string(idmef_protocol_str);
	  
	  uint16_t *idmef_port = idmef_service_get_port(idmef_service);
	  
	  service_.reset(new Persistency::Service(name,*idmef_port,protocol,Persistency::ReferenceURLPtr()));
	}
    }
}

Persistency::Analyzer::IP IDMEFParserTarget::getAddress() const
{
  return *ip_;
}

const Persistency::ProcessPtr IDMEFParserTarget::getProcess() const
{
  return process_;
}

const Persistency::ServicePtr IDMEFParserTarget::getService() const
{
  return service_;
}

} // namespace Prelude
} // namespace Input


