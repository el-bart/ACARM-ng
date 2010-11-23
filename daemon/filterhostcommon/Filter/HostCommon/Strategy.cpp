/*
 * Strategy.cpp
 *
 */
#include "Filter/HostCommon/Strategy.hpp"
#include <cassert>
#include "Algo/forEachUniqueLeaf.hpp"
#include "Filter/HostCommon/CheckHosts.hpp"

using namespace Persistency;

namespace Filter
{
namespace HostCommon
{

Strategy::Strategy(const std::string &type, const std::string &name, unsigned int timeout):
  Filter::Simple::Strategy<Data>(type, name, timeout)
{
}

Core::Types::Proc::EntryControlList Strategy::createEntryControlList(void)
{
  Core::Types::Proc::EntryControlList ecl=Core::Types::Proc::EntryControlList::createDefaultReject();
  ecl.add("*input*");   // TODO: magic value
  return ecl;
}

Persistency::HostPtr Strategy::getHost(const Node node) const
{
  return Algo::forEachUniqueLeaf( node, CheckHosts(this) ).out_;
}

Data Strategy::makeThisEntryUserData(const Node n) const
{
  // get host from this node.
  HostPtr h=getHost(n);
  return Data(h);
}

bool Strategy::isEntryInteresting(const NodeEntry thisEntry) const
{
  // must be common host
  return thisEntry.t_.host_.get()!=NULL;
}

Persistency::MetaAlert::Name Strategy::getMetaAlertName(
                                              const NodeEntry thisEntry,
                                              const NodeEntry /*otherEntry*/) const
{
  assert( thisEntry.t_.host_.get() !=NULL );
  // forward to user's implementation
  return getMetaAlertName(thisEntry.t_.host_);
}

bool Strategy::canCorrelate(const NodeEntry thisEntry,
                            const NodeEntry otherEntry) const
{
  assert( thisEntry.t_.host_.get() !=NULL );
  assert( otherEntry.t_.host_.get()!=NULL );
  // common host entries must equal
  return thisEntry.t_.host_->getIP() == otherEntry.t_.host_->getIP();
}

Data Strategy::makeUserDataForNewNode(const NodeEntry &thisEntry,
                                      const NodeEntry &/*otherEntry*/,
                                      const Node       /*newNode*/) const
{
  return thisEntry.t_;
}

void Strategy::postProcessNode(Node &/*n*/, Filter::BackendFacade &/*bf*/) const
{
  // nothing to be done here
}

} // namespace HostCommon
} // namespace Filter
