/*
 * computeSeverity.cpp
 *
 */
#include <cmath>
#include <cassert>

#include "Algo/computeSeverity.hpp"
#include "Algo/forEachUniqueInTree.hpp"

namespace Algo
{

namespace
{
/** \brief helper object computing severity.
 */
class SeveritCompute
{
public:
  /** \brief initialize object.
   */
  SeveritCompute(void):
    delta_(0),
    leafsCount_(0),
    leafsSeveritySum_(0)
  {
  }
  /** \brief work procedure itself.
   *  \param node root node to start computation from.
   */
  void operator()(const Persistency::GraphNode *node)
  {
    assert(node!=NULL);
    if( node->isLeaf() )
    {
      ++leafsCount_;
      leafsSeveritySum_+=node->getAlert()->getSeverity().getLevel().toInt();
    }
    // add severity delta for both node and leafs
    delta_+=node->getMetaAlert()->getSeverityDelta();
  }
  /** \brief gets the computed value.
   *  \return total (sub)tree severity.
   */
  double get(void) const
  {
    assert( leafsCount_>0 );
    // arithmetic average of all severities modified by delta.
    const double aver      =(0.0+leafsSeveritySum_)/leafsCount_;
    const double countBonus=log(10+leafsCount_)/log(10) - 1;
    assert( countBonus>=0 && "logarithm went out of scope" );
    return countBonus + aver + delta_;
  }

private:
  double delta_;
  int    leafsCount_;
  int    leafsSeveritySum_;
}; // struct SeveritCompute
} // unnamed namespace


double computeSeverity(Persistency::ConstGraphNodePtrNN root)
{
  return computeSeverity(root.get());
} // computeSeverity()

double computeSeverity(const Persistency::GraphNode *root)
{
  return forEachUniqueInTree(root, SeveritCompute() ).get();
} // computeSeverity()

} // namespace Algo
