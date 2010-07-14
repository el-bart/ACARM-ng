/*
 * FactoryBuilder.cpp
 *
 */
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <cassert>

#include "BuildProcess/ForceLink.hpp"
#include "ConfigIO/TriggerConfig.hpp"
#include "Commons/Factory/RegistratorHelper.hpp"
#include "Core/Types/Proc/InterfaceImpl.hpp"
#include "Trigger/Mail/Strategy.hpp"
#include "Trigger/Mail/Config.hpp"
#include "Trigger/Mail/FactoryBuilder.hpp"

using namespace std;
using namespace ConfigIO;
using namespace Core::Types::Proc;
using Trigger::Simple::ThresholdConfig;


namespace Trigger
{
namespace Mail
{

namespace
{
const Commons::Factory::RegistratorHelper<Factory, FactoryBuilder> g_rh;
} // unnamed namespace

FORCE_LINK_THIS_OBJECT(Trigger_Mail_FactoryBuilder)


FactoryBuilder::FactoryBuilder(void):
  name_("mail"),
  log_("trigger.mail.factorybuilder")
{
}

namespace
{
uint16_t getPort(const std::string &v)
{
  try
  {
    const long     tmp=boost::lexical_cast<long>(v);
    const uint16_t out=boost::numeric_cast<uint16_t>(tmp);
    return out;
  }
  catch(const std::exception &ex)
  {
    // re-throw common type
    throw ExceptionInvalidValue(SYSTEM_SAVE_LOCATION, "port", v.c_str(), ex.what() );
  }
} // getPort()

Config::Server::Security getSecurity(const std::string &v)
{
  if(v=="ssl")
    return Config::Server::Security::SSL;
  if(v=="starttls")
    return Config::Server::Security::STARTTLS;

  // ok - we have invalid value here...
  throw ExceptionInvalidValue(SYSTEM_SAVE_LOCATION, "security", v.c_str() );
} // getSecurity()
} // unnamed namespace


FactoryBuilder::FactoryPtr FactoryBuilder::buildImpl(const Options &options) const
{
  LOGMSG_INFO(log_, "building trigger's instance");
  assert(g_rh.isRegistered() && "oops - registration failed");

  const TriggerConfig fc(name_, options);

  // gather required config
  const std::string              &server=fc["server"];
  LOGMSG_INFO_S(log_)<<"setting server to "<<server;
  const uint16_t                  port  =getPort(fc["port"]);
  LOGMSG_INFO_S(log_)<<"setting port to "<<port;
  const Config::Server::Security  sec   =getSecurity(fc["security"]);
  LOGMSG_INFO_S(log_)<<"setting security to "<<sec.toInt();
  const std::string              &from  =fc["from"];
  LOGMSG_INFO_S(log_)<<"setting from-address to "<<from;
  const Config::Server            serverCfg(from, server, port, sec);

  // thresholds' config
  const char *sevTh=fc.get("severity_threshold");
  if(sevTh!=NULL)
    LOGMSG_INFO_S(log_)<<"setting severity threshold to "<<sevTh;
  const char *cntTh=fc.get("alerts_count_threshold");
  if(cntTh!=NULL)
    LOGMSG_INFO_S(log_)<<"setting alerts count threshold to "<<cntTh;
  const ThresholdConfig thCfg(sevTh, cntTh);

  // recipient address
  const std::string &to    =fc["to"];
  LOGMSG_INFO_S(log_)<<"setting to-address to "<<to;

  // defile output type
  typedef InterfaceImpl<Mail::Strategy, Mail::Config> Impl;
  typedef FactoryBuilder::FactoryPtr                  OutPtr;

  // check for authorization requirement
  const char *user=fc.get("user");
  if(user!=NULL)
  {
    LOGMSG_INFO_S(log_)<<"setting user to "<<user;
    const std::string           &pass=fc["password"];
    const Config::Authorization  auth(user, pass);
    // create and return new handle, with configured authorization
    LOGMSG_INFO(log_, "account configured with authorization required");
    return OutPtr( new Impl( name_, Mail::Config(thCfg, to, serverCfg, auth) ) );
  } // if(use_auth)
  else
    if( fc.get("password")!=NULL )
      LOGMSG_WARN(log_, "user not configured, but password provided - skipping any way");

  // create and return new handle, with config without authorization
  LOGMSG_INFO(log_, "account configured without authorization required");
  return OutPtr( new Impl( name_, Mail::Config(thCfg, to, serverCfg) ) );
}

const FactoryBuilder::FactoryTypeName &FactoryBuilder::getTypeNameImpl(void) const
{
  return name_;
}

} // namespace Mail
} // namespace Trigger
