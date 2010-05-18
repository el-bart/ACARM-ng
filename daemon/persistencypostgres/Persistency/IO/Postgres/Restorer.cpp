/*
 * Restorer.cpp
 *
 */
#include <set>
#include <cassert>

#include "Persistency/IO/Postgres/Restorer.hpp"
#include "Persistency/IO/Postgres/ExceptionBadNumberOfNodeChildren.hpp"

using namespace Persistency::IO::Postgres::detail;
using namespace std;

namespace
{
template<typename T>
void removeDuplicates(std::vector<T> &out)
{
  std::set<T> s( out.begin(), out.end() );
  out.erase( out.begin(), out.end() );
  out.assign( s.begin(), s.end() );
} // removeDuplicates()
} // unnamed namespace


namespace Persistency
{
namespace IO
{
namespace Postgres
{

Restorer::Restorer(Transaction    &t,
                   DBHandlerPtrNN  dbHandler):
  IO::Restorer(t),
  log_("persistency.io.postgres.restorer"),
  dbHandler_(dbHandler)
{
}

void Restorer::restoreAllInUseImpl(Transaction &t, NodesVector &out)
{
  EntryReader er(t, *dbHandler_);
  const Tree::IDsVector &maInUse=er.readIDsMalertsInUse();
  const Tree::IDsVector &roots  =er.readRoots();
  restore(er, out, maInUse, roots);
}

void Restorer::restoreBetweenImpl(Transaction     &t,
                                  NodesVector     &out,
                                  const Timestamp &from,
                                  const Timestamp &to)
{
  EntryReader er(t, *dbHandler_);
  const Tree::IDsVector  maBetween=er.readIDsMalertsBetween(from, to);
  const Tree::IDsVector &roots    =er.readRoots(from, to);
  restore(er, out, maBetween, roots);
}

BackendFactory::FactoryPtr Restorer::createStubIO(void)
{
  const BackendFactory::FactoryTypeName name("stubs");
  const BackendFactory::Options         options;
  return BackendFactory::create(name, options);
}


GraphNodePtrNN Restorer::makeLeaf(DataBaseID           id,
                                 AlertPtrNN           aPtr,
                                 IO::ConnectionPtrNN  connStubIO,
                                 IO::Transaction     &tStubIO)
{
  if( leafCache_.has(id) )
    return leafCache_.get(id);
  GraphNodePtrNN leaf( new GraphNode( aPtr, connStubIO, tStubIO ) );
  leafCache_.add(id, leaf);
  return leaf;
}

GraphNodePtrNN Restorer::makeNode(DataBaseID                id,
                                  MetaAlertPtrNN            maPtr,
                                  const NodeChildrenVector &vec,
                                  IO::ConnectionPtrNN       connStubIO,
                                  IO::Transaction          &tStubIO)
{
  if( nodeCache_.has(id) )
    return nodeCache_.get(id);
  GraphNodePtrNN node( new GraphNode( maPtr, connStubIO, tStubIO, vec ) );
  nodeCache_.add(id, node);
  return node;
}

GraphNodePtrNN Restorer::deepFirstSearch(DataBaseID                                      id,
                                         NodesVector                                    &out,
                                         Persistency::IO::Postgres::detail::EntryReader &er,
                                         IO::ConnectionPtrNN                             connStubIO,
                                         IO::Transaction                                &tStubIO)
{
  TreePtrNN node = treeNodes_.get(id);
  // check if there are no children (i.e. is leaf)
  if( node->getChildrenNumber() == 0 )
    return restoreLeaf(id, out, er, connStubIO, tStubIO);
  return restoreNode(node, id, out, er, connStubIO, tStubIO);
}

// TODO: add class with lists of errors
void Restorer::restore(Persistency::IO::Postgres::detail::EntryReader &er,
                       NodesVector                                    &out,
                       const Tree::IDsVector                          &malerts,
                       const Tree::IDsVector                          &roots)
{
  addTreeNodesToCache(er, malerts);

  IO::ConnectionPtrNN connStubIO( createStubIO() );
  IO::Transaction     tStubIO( connStubIO->createNewTransaction("stub transaction") );

  for(Tree::IDsVector::const_iterator it = roots.begin(); it != roots.end(); ++it)
  {
    try
    {
      deepFirstSearch(*it, out, er, connStubIO, tStubIO);
    }
    catch(const ExceptionBadNumberOfNodeChildren &)
    {
      // TODO: add note that this subtree is being skipped.
      LOGMSG_WARN_S(log_)<<"root with id "<< *it << " has bad number of children";
    }
  }
  // remove doplicates from out vector
  removeDuplicates(out);
}

template<typename T>
void Restorer::addIfNew(const T &e, DataBaseID id)
{
  if(!dbHandler_->getIDCache()->has(e))
    dbHandler_->getIDCache()->add(e, id);
  else
    assert(id == dbHandler_->getIDCache()->get(e));
}

GraphNodePtrNN Restorer::restoreLeaf(DataBaseID                                      id,
                                     NodesVector                                    &out,
                                     Persistency::IO::Postgres::detail::EntryReader &er,
                                     IO::ConnectionPtrNN                             connStubIO,
                                     IO::Transaction                                &tStubIO)
{
  // read Alert from data base
  AlertPtrNN alertPtr( er.getLeaf(id) );
  const DataBaseID alertID = er.getAlertIDAssociatedWithMetaAlert(id);
  // add Alert to cache
  addIfNew(alertPtr, alertID);
  const GraphNodePtrNN graphNodeLeaf( makeLeaf( alertID, alertPtr, connStubIO, tStubIO ) );
  out.push_back(graphNodeLeaf);
  return graphNodeLeaf;
}

GraphNodePtrNN Restorer::restoreNode(TreePtrNN                                       node,
                                     DataBaseID                                      id,
                                     NodesVector                                    &out,
                                     Persistency::IO::Postgres::detail::EntryReader &er,
                                     IO::ConnectionPtrNN                             connStubIO,
                                     IO::Transaction                                &tStubIO)
{
  // read Meta Alert from data base
  MetaAlertPtrNN malertPtr( er.readMetaAlert(id) );
  // add Meta Alert to cache
  addIfNew(malertPtr, id);
  GraphNodePtrNN graphNode( makeNode( id, malertPtr,
                                         restoreNodeChildren(node, id, out, er, connStubIO, tStubIO),
                                         connStubIO,
                                         tStubIO ));
  out.push_back(graphNode);
  return graphNode;
}
NodeChildrenVector Restorer::restoreNodeChildren(TreePtrNN                                       node,
                                                 DataBaseID                                      id,
                                                 NodesVector                                    &out,
                                                 Persistency::IO::Postgres::detail::EntryReader &er,
                                                 IO::ConnectionPtrNN                             connStubIO,
                                                 IO::Transaction                                &tStubIO)
{
  vector<GraphNodePtrNN>  tmpNodes;
  // get children IDs vector for given node
  const Tree::IDsVector  &nodeChildren = node->getChildren();
  tmpNodes.reserve( nodeChildren.size() );
  // restore children
  for(Tree::IDsVector::const_iterator it = nodeChildren.begin();
      it != nodeChildren.end(); ++it)
  {
    try
    {
      tmpNodes.push_back( deepFirstSearch( *it, out, er, connStubIO, tStubIO ) );
    }
    catch(const ExceptionNoSuchEntry &)
    {
      LOGMSG_WARN_S(log_)<<"child with id "<< *it << " does'n exist";
    }
    catch(const ExceptionBadNumberOfNodeChildren &)
    {
      LOGMSG_WARN_S(log_)<<"child with id "<< *it << " has bad number of children";
    }
  }
  if(tmpNodes.size() < 2)
    throw ExceptionBadNumberOfNodeChildren(SYSTEM_SAVE_LOCATION, id );
  // add first two children to the node children vector
  assert(tmpNodes.size() >= 2);
  NodeChildrenVector vec(tmpNodes[0], tmpNodes[1]);
  // add rest of children (indexing is started with 2 becouse node should have at least two children)
  for(size_t i = 2; i<tmpNodes.size(); ++i)
    vec.push_back(tmpNodes[i]);
  return vec;
}

void Restorer::addTreeNodesToCache(Persistency::IO::Postgres::detail::EntryReader &er,
                                   const Tree::IDsVector                          &malerts)
{
  for(Tree::IDsVector::const_iterator it = malerts.begin(); it != malerts.end(); ++it)
  {
    const Tree::IDsVector &malertChildren = er.readMetaAlertChildren( (*it) );
    // put this data to the tree which represents meta alerts tree structure
    treeNodes_.add(*it, TreePtr(new Tree(*it, malertChildren) ));
  }
}

} // namespace Postgres
} // namespace IO
} // namespace Persistency
