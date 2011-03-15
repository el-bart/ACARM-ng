/*
 * TimeoutedSet.cpp
 *
 */
#include "Filter/NewEvent/TimeoutedSet.hpp"

namespace Filter
{
namespace NewEvent
{

void TimeoutedSet::add(const HashSharedPtr &key)
{
  timeouted_.push_back( key );
}

void TimeoutedSet::markRemoved(BackendFacade &bf, const Persistency::IO::DynamicConfig::Owner &owner)
{
  // TODO: notice that only elements that are NOT is processedset at the moment should be removed here.
  //       this is required since Entry() elements withe a given name can repeat.
  Persistency::IO::DynamicConfigAutoPtr dc = bf.createDynamicConfig(owner);
  for(Timeouted::iterator it = timeouted_.begin(); it != timeouted_.end(); ++it)
  {
    // TODO: add try{}catch(...) around loop's body to ensure single exception for one entry
    //       will not block removing others from DC.
    dc->remove( it->get()->getHash().get() );
  }
  timeouted_.clear();
}

bool TimeoutedSet::isTimeouted(const HashSharedPtr &key) const
{
  for(Timeouted::const_iterator it = timeouted_.begin(); it != timeouted_.end(); ++it)
  {
    if(*it==key)
      return true;
  }
  // object not found
  return false;
}

} // namespace Filter
} // namespace NewEvent
