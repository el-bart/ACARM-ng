/*
 * Interface.hpp
 *
 */
#ifndef INCLUDE_FILTER_INTERFACE_HPP_FILE
#define INCLUDE_FILTER_INTERFACE_HPP_FILE

/* public header */

#include <string>
#include <vector>
#include <boost/noncopyable.hpp>

#include "Base/TimeoutQueue.hpp"
#include "Logger/Logger.hpp"
#include "Persistency/GraphNode.hpp"
#include "Filter/BackendProxy.hpp"


namespace Filter
{

/** \brief interface (base) for all filters.
 */
class Interface: private boost::noncopyable
{
public:
  /** \brief helper typedef for GraphNode pointer. */
  typedef Persistency::GraphNodePtrNN Node;
  /** \brief helper typedef for list of chenged nodes. */
  typedef std::vector<Node>           ChangedNodes;

  /** \brief processes given meta-alert.
   */
  void process(Node n, ChangedNodes &changed);
  /** \brief gets filter name.
   *  \return name of implemented filter.
   */
  const std::string &getFilterName(void) const
  {
    return name_;
  }

protected:
  /** \brief timeouting queue colleciton type. */
  typedef Base::TimeoutQueue<Node> NodesTimeoutQueue;

  /** \brief create instance.
   */
  explicit Interface(const std::string &name);

private:
  /** \brief user-provided implementation of node processing.
   *  \param n       added/changed node to be processed by filter.
   *  \param changed collection of nodes that changed (user should use this
   *                 to inform other filters about changes).
   *  \param ntq     internal filter's collection of elements with assigned
   *                 timeout. it can be browsed any time. if element is to be
   *                 saved for processing in later calls (ex.: it looks like it
   *                 can be correlated later on) it is to be saved here, with
   *                 proper timeout.
   *  \param bp      persistency proxy - object that allows saving changes on
   *                 persistent storage.
   */
  virtual void processImpl(Node               n,
                           ChangedNodes      &changed,
                           NodesTimeoutQueue &ntq,
                           BackendProxy      &bp) = 0;

  const std::string                name_;
  Persistency::IO::ConnectionPtrNN conn_;
  NodesTimeoutQueue                ntq_;
}; // class Interface

} // namespace Filter

#endif
