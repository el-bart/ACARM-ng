/*
 * Strategy.cpp
 *
 */
#include <algorithm>
#include <boost/thread.hpp>
#include <cassert>

#include "Logger/Logger.hpp"
#include "Persistency/IO/BackendFactory.hpp"
#include "Trigger/Strategy.hpp"

using namespace std;


namespace
{
struct FindGivenNode
{
  explicit FindGivenNode(Persistency::GraphNodePtrNN n):
    n_(n)
  {
  }

  bool operator()(const boost::weak_ptr<Persistency::GraphNode> &w) const
  {
    return n_.get()==w.lock().get();
  }

private:
  Persistency::GraphNodePtrNN n_;
}; // struct FindGivenNode
} // unnamed namespace


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

  // check nos_ cache - maybe entry has been already triggered before?
  if( find_if( nos_.begin(), nos_.end(), FindGivenNode(n) )!=nos_.end() )
  {
    LOGMSG_DEBUG_S(log_)<<"node at address 0x"
                        <<static_cast<void*>( n.get() )
                        <<" matches criteria but was already triggered before";
    return;
  }

  // check if node should be processed at all
  if( !matchesCriteria(n) )
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
  trigger(n);

  // if it succeeded, mark it as triggered
  BackendFacade bf(conn_, name_);
  bf.markAsTriggered( n->getMetaAlert() );
  bf.commitChanges();

  LOGMSG_DEBUG_S(log_)<<"triggering node at address 0x"
                      <<static_cast<void*>( n.get() )
                      <<" finished successfully";
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
  conn_( Persistency::IO::create() )
{
  LOGMSG_INFO(log_, "trigger created");
}

void Strategy::interruptionPoint(void)
{
  boost::this_thread::interruption_point();
}

} // namespace Filter
