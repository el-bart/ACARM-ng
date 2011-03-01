/*
 * TestEntry.t.hpp
 *
 */
#ifndef INCLUDE_FILTER_NEWEVENT_TESTENTRY_T_HPP_FILE
#define INCLUDE_FILTER_NEWEVENT_TESTENTRY_T_HPP_FILE
#include "Filter/NewEvent/Entry.hpp"


namespace Filter
{
namespace NewEvent
{

class TestEntry: private Entry
{
public:
  TestEntry(Name name, Filter::BackendFacade *bf, TimeoutedSet *ts):
    Entry(name, bf, ts)
  {
  }
  Persistency::IO::DynamicConfigAutoPtr getDynamicConfig()
  {
    assert(dc_.get() != NULL);
    return dc_;
  }
};

} // namespace Filter
} // namespace NewEvent
#endif
