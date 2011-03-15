/*
 * TimeoutedSet.hpp
 *
 */
#ifndef INCLUDE_FILTER_NEWEVENT_TIMEOUTEDSET_HPP_FILE
#define INCLUDE_FILTER_NEWEVENT_TIMEOUTEDSET_HPP_FILE

#include "Filter/BackendFacade.hpp"
#include "Filter/Exception.hpp"
#include "Filter/NewEvent/Hash.hpp"

namespace Filter
{
namespace NewEvent
{

/** \brief local cache to store timeouted elements.
 *
 */
class TimeoutedSet
{

  typedef std::vector<HashSharedPtr> Timeouted;

public:
  /** \brief add timeouted key to the collection.
   *  \param key key to be added.
   */
  void add(const HashSharedPtr &key);
  /** \brief remove timeouted element from collection and from persistency.
   *  \param bf    facade for removing timeouting elements from persistency.
   *  \param owner owner's name.
   */
  void markRemoved(BackendFacade &bf, const Persistency::IO::DynamicConfig::Owner &owner);
  // TODO: 'key' must be NULL.
  // TODO: this method is never used and should be removed
  /** \brief checks if given key has been alrady timeouted or not.
   *  \param key key from which ...
   *  \return true if key has been already timeouted, false otherwise.
   */
  bool isTimeouted(const HashSharedPtr &key) const;

private:
  Timeouted timeouted_;
}; // class TimeoutedSet

} // namespace Filter
} // namespace NewEvent

#endif
