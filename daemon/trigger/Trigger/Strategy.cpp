/*
 * Strategy.cpp
 *
 */
#include <boost/thread.hpp>
#include <cassert>

#include "Logger/Logger.hpp"
#include "Persistency/IO/BackendFactory.hpp"
#include "Trigger/Strategy.hpp"

using namespace std;

namespace Trigger
{

Strategy::~Strategy(void)
{
  LOGMSG_INFO(log_, "deallocating trigger");
}

void Strategy::process(Persistency::GraphNodePtrNN n)
{
  LOGMSG_DEBUG_S(log_)<<"processing node at address 0x"
                      <<static_cast<void*>( n.get() );
  // clean-up old entries
  nos_.prune();

  // check if node should be processed at all
  if( !matchesCriteria(*n) )
  {
    LOGMSG_DEBUG_S(log_)<<"node at address 0x"
                        <<static_cast<void*>( n.get() )
                        <<" does not match criteria";
    return;
  }

  // process node
  nos_.add(n);
  LOGMSG_INFO_S(log_)<<"calling trigger for node at address 0x"
                     <<static_cast<void*>( n.get() );
  trigger(*n);
  // if it succeeded, mark it as triggered
  bf_.markAsTriggered( n->getMetaAlert() );
}


namespace
{
inline Logger::NodeName makeNodeName(const string &name)
{
  const string &out="trigger." + Logger::NodeName::removeInvalidChars(name);
  return Logger::NodeName( out.c_str() );
} // makeNodeName()
} // unnamed namespace

Strategy::Strategy(const std::string &name):
  log_( makeNodeName(name) ),
  name_(name),
  conn_( Persistency::IO::create() ),
  bf_(conn_, name_)
{
  LOGMSG_INFO(log_, "trigger created");
}

void Strategy::interruptionPoint(void)
{
  boost::this_thread::interruption_point();
}

} // namespace Filter
