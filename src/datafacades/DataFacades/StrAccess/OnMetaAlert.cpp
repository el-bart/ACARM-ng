/*
 * OnMetaAlert.cpp
 *
 */
#include <iterator>
#include <cassert>

#include "Algo/forEach.hpp"
#include "Algo/forEachUniqueLeaf.hpp"
#include "DataFacades/StrAccess/OnMetaAlert.hpp"

namespace Persistency
{
namespace Facades
{
namespace StrAccess
{

OnMetaAlert::Nodes OnMetaAlert::getAllChildren(const GraphNode &root)
{
  Nodes out;
  std::copy( root.begin(), root.end(), std::back_insert_iterator<Nodes>(out) );
  return out;
}


namespace
{
template<typename TColl>
struct LeafsGatherer
{
  explicit LeafsGatherer(TColl &out):
    out_(&out)
  {
  }

  void operator()(const GraphNodePtrNN &leaf)
  {
    assert( leaf->isLeaf() );
    assert(out_!=NULL);
    out_->push_back(leaf);
  }

private:
  TColl *out_;
}; // struct LeafsGatherer


template<typename TColl>
struct FuncObj
{
  explicit FuncObj(TColl &out):
    lg_(out)
  {
  }

  void operator()(const GraphNodePtrNN &node)
  {
    Algo::forEachUniqueLeaf(node, lg_);
  }

private:
  LeafsGatherer<TColl> lg_;
}; // struct FuncObj
} // unnamed namespace


OnMetaAlert::Nodes OnMetaAlert::getAllLeafs(const GraphNode &root)
{
  Nodes out;
  // if there is no leafs of this node, just return empty set
  if(root.isLeaf())
    return out;
  // we have a node, so process it's children
  FuncObj<OnMetaAlert::Nodes> fo(out);
  Algo::forEach(root.begin(), root.end(), fo);
  // return the result
  return out;
}

} // namespace StrAccess
} // namespace Facades
} // namespace Persistency
