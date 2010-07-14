/*
 * FactoryBuilder.cpp
 *
 */
#include <boost/lexical_cast.hpp>
#include <cassert>

#include "BuildProcess/ForceLink.hpp"
#include "ConfigIO/FilterConfig.hpp"
#include "Commons/Factory/RegistratorHelper.hpp"
#include "Core/Types/Proc/InterfaceImpl.hpp"
#include "Filter/DNSResolver/FactoryBuilder.hpp"
#include "Filter/DNSResolver/Strategy.hpp"

using namespace std;
using namespace ConfigIO;
using namespace Core::Types::Proc;


namespace Filter
{
namespace DNSResolver
{

namespace
{
const Commons::Factory::RegistratorHelper<Factory, FactoryBuilder> g_rh;
} // unnamed namespace

FORCE_LINK_THIS_OBJECT(Filter_DNSResolver_FactoryBuilder)


FactoryBuilder::FactoryBuilder(void):
  type_("dnsresolver"),
  log_("filter.dnsresolver.factorybuilder")
{
}

FactoryBuilder::FactoryPtr FactoryBuilder::buildImpl(const Options &options) const
{
  LOGMSG_INFO(log_, "building filter's instance");
  assert(g_rh.isRegistered() && "oops - registration failed");

  const FilterConfig         fc(type_, options);
  const int                  timeout=boost::lexical_cast<int>( fc["cachetimeout"] );
  LOGMSG_INFO_S(log_)<<"setting cache timeout to "<<timeout<<"[s]";
  const Strategy::Parameters params(timeout);

  // create and return new handle.
  typedef InterfaceImpl<Strategy, Strategy::Parameters> Impl;
  return FactoryBuilder::FactoryPtr( new Impl(type_, type_, params) );
}

const FactoryBuilder::FactoryTypeName &FactoryBuilder::getTypeNameImpl(void) const
{
  return type_;
}

} // namespace DNSResolver
} // namespace Filter
