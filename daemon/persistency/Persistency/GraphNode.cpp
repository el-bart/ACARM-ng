/*
 * GraphNode.cpp
 *
 */
#include <pthread.h>
#include <cassert>

#include "Persistency/GraphNode.hpp"
#include "Persistency/IO/GlobalConnection.hpp"

namespace Persistency
{

GraphNode::GraphNode(AlertPtrNN           alert,
                     IO::ConnectionPtrNN  connection,
                     IO::Transaction     &t):
  self_( new MetaAlert(alert) ),
  leaf_(alert)
{
  assert( leaf_.get()!=NULL );
  assert( self_.get()!=NULL );
  assert( connection.get()!=NULL );

  // save data to DB
  connection->alert(leaf_, t)->save();
  IO::MetaAlertAutoPtr maIO=connection->metaAlert( getMetaAlert(), t);
  maIO->save();
  maIO->associateWithAlert(leaf_);
  maIO->markAsUsed();

  assert( isLeaf() && "invalid initialization");
}

GraphNode::GraphNode(MetaAlertPtrNN        ma,
                     IO::ConnectionPtrNN   connection,
                     IO::Transaction      &t,
                     GraphNodePtrNN        child1,
                     GraphNodePtrNN        child2,
                     const ChildrenVector &otherChildren):
  self_(ma),
  leaf_()
{
  assert( leaf_.get()==NULL );
  assert( self_.get()!=NULL );
  assert( connection.get()!=NULL );

  // save data to DB along with adding elements to graph
  IO::MetaAlertAutoPtr maIO=connection->metaAlert( getMetaAlert(), t);
  assert( maIO.get()!=NULL );
  maIO->save();
  addChild(child1, *maIO);
  addChild(child2, *maIO);
  for(ChildrenVector::const_iterator it=otherChildren.begin();
      it!=otherChildren.end(); ++it)
    addChild(*it, *maIO);
  maIO->markAsUsed();

  assert( !isLeaf() && "invalid initialization");
}

GraphNode::~GraphNode(void)
{
  IO::GlobalConnection::get()->markAsUnused( getMetaAlert() );
}

GraphNode::iterator GraphNode::begin(void)
{
  ensureIsNode();
  return children_.begin();
}

GraphNode::iterator GraphNode::end(void)
{
  ensureIsNode();
  return children_.end();
}

GraphNode::const_iterator GraphNode::begin(void) const
{
  ensureIsNode();
  return children_.begin();
}

GraphNode::const_iterator GraphNode::end(void) const
{
  ensureIsNode();
  return children_.end();
}

void GraphNode::markAsTriggered(const std::string &name)
{
  IO::GlobalConnection::get()->markAsTriggered( getMetaAlert(), name);
}

void GraphNode::addChild(GraphNodePtrNN child, IO::MetaAlert &maIO)
{
  ensureIsNode();
  // check if addition will not cause cycle
  nonCyclicAddition(child);
  // persistency save
  maIO.addChild( child->getMetaAlert() );
}

bool GraphNode::isLeaf(void) const
{
  if( leaf_.get()!=NULL )
  {
    assert( children_.size()==0 );
    return true;
  }
  return false;
}

MetaAlertPtrNN GraphNode::getMetaAlert(void)
{
  assert(self_.get()!=NULL);
  return self_;
}

AlertPtrNN GraphNode::getAlert(void)
{
  assert( getMetaAlert().get()!=NULL );
  if( !isLeaf() )
    throw ExceptionNotLeaf(__FILE__, getMetaAlert()->getName().get() );

  assert( leaf_.get()!=NULL );
  return leaf_;
}

void GraphNode::ensureIsNode(void) const
{
  assert( self_.get()!=NULL );
  if( isLeaf() )
    throw ExceptionNotNode(__FILE__, self_->getName().get() );
}


//
// TODO: this is a TEMPORARY solution - it must be reworked to use some
//       sort of distributed algorithm, not to block whole structure
//       when non-dependent data parts are being operated on.
//
// TODO: this can be repaced with System::SecureInitLock.
// namespace with helpers for locking on global level of whole structure
namespace
{
pthread_mutex_t additionMutex=PTHREAD_MUTEX_INITIALIZER;

struct PtrLock
{
  PtrLock(void)
  {
    if( pthread_mutex_lock(&additionMutex)!=0 )
      throw Exception(__FILE__, "unable to lock mutex for "
                                "GraphNode/addition implementation");
  }
  ~PtrLock(void)
  {
    if( pthread_mutex_unlock(&additionMutex)!=0 )
      assert(!"unable to lock mutex for GraphNode/addition implementation");
  }
}; // struct PtrLock

} // unnamed namespace

void GraphNode::nonCyclicAddition(GraphNodePtrNN child)
{
  const GraphNode *childPtr=child.get();
  assert(childPtr!=NULL);

  PtrLock lock;     // only one addition at a time! (TODO: it's too restrictive)

  if( this==childPtr           ||   // instant-cycle
      childPtr->hasCycle(this)    ) // is it possible to access self through child
    throw ExceptionCycleDetected(__FILE__,
                                 child->getMetaAlert()->getName().get(),
                                 getMetaAlert()->getName().get() );

  // if there is no cycle, add new child
  children_.push(child);
}

bool GraphNode::hasCycle(const GraphNode *child) const
{
  assert(child!=NULL);

  // for leaf check if current node is not the one to be added
  if( isLeaf() )
    return this==child;

  //
  // TODO: where is better algorithm with do { for(...) } while() loops, to
  //       be used when no global locking is present.
  //       depending on strategy implemented it might be not needed though.
  //
  for(const_iterator it=begin(); it!=end(); ++it)
  {
    const GraphNode *tmp=it->get();
    assert(tmp!=NULL);
    if( tmp==child || tmp->hasCycle(child) )    // ooops - cycle detected...
      return true;
  } // for(children)

  // no cycle detected - seems ok.
  return false;
}

} // namespace Persistency
