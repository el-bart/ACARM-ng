/*
 * Strategy.cpp
 *
 */
#include <sstream>
#include <cassert>

#include "Filter/UsersMonitor/Strategy.hpp"
#include "Algo/forEachUniqueLeaf.hpp"
#include "Algo/GatherHosts.hpp"

using namespace std;
using namespace Persistency;

namespace Filter
{
namespace UsersMonitor
{

Strategy::Strategy(const std::string &name, const Parameters &params):
  Filter::Simple::Strategy<Data>("usersmonitor", name, params.timeout_),
  params_(params)
{
}

Core::Types::Proc::EntryControlList Strategy::createEntryControlList(void)
{
  Core::Types::Proc::EntryControlList ecl=Core::Types::Proc::EntryControlList::createDefaultReject();
  ecl.add("*input*");   // TODO: magic value
  return ecl;
}

Data Strategy::makeThisEntryUserData(const Node n) const
{
  return Data( n->getAlert(), params_.skip_ );
}

bool Strategy::isEntryInteresting(const NodeEntry thisEntry) const
{
  assert( thisEntry.node_->isLeaf() && "ECL accepted non-leaf" );
  Data d( thisEntry.node_->getAlert(), params_.skip_ );
  if(d.get().size()==0u)
    return false;
  assert( d.get().size()>0u );
  return true;
}

Persistency::MetaAlert::Name Strategy::getMetaAlertName(
                                              const NodeEntry thisEntry,
                                              const NodeEntry otherEntry) const
{
  Data::Names::const_iterator it=thisEntry.t_.commonWith(otherEntry.t_);
  assert( it!=thisEntry.t_.get().end() );
  stringstream ss;
  ss << "[usersmonitor] actions of user '"<<*it<<"'";
  return ss.str();
}


bool Strategy::canCorrelate(const NodeEntry thisEntry,
                            const NodeEntry otherEntry) const
{
  return thisEntry.t_.commonWith(otherEntry.t_)!=thisEntry.t_.get().end();
}

Data Strategy::makeUserDataForNewNode(const NodeEntry &thisEntry,
                                      const NodeEntry &otherEntry,
                                      const Node       /*newNode*/) const
{
  Data::Names::const_iterator it=thisEntry.t_.commonWith(otherEntry.t_);
  assert( it!=thisEntry.t_.get().end() );
  const Data d(*it, params_.skip_);
  assert( d.get().size()==1u ); // skip list should not apply here
  return d;
}

void Strategy::postProcessNode(Node &/*n*/, Filter::BackendFacade &/*bf*/) const
{
  // nothing to be done here
}

} // namespace UsersMonitor
} // namespace Filter