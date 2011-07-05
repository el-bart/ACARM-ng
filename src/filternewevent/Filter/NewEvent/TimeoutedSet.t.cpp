/*
 * TimeoutedSet.t.cpp
 *
 */
#include <tut.h>

#include "Filter/NewEvent/Entry.hpp"
#include "Filter/NewEvent/TimeoutedSet.hpp"
#include "Persistency/IO/BackendFactory.hpp"
#include "Persistency/IO/DynamicConfig.hpp"
#include "Filter/NewEvent/TestConnection.t.hpp"

using namespace std;
using namespace Core::Types::Proc;
using namespace Filter;
using namespace Filter::NewEvent;
using namespace TestHelpers::Persistency;

namespace
{
struct TestClass
{
  TestClass(void):
    tconn_(new TestConnection),
    conn_( tconn_ ),
    bf_(conn_, changed_, TypeName("testnewevent"), InstanceName("myname")),
    owner_("Filter::NewEvent")
  {
  }

  // TODO: this function is trivial - 1 line in code: e_q("...", tconn_->data_[key], value);
  void testData(const std::string &key, const std::string &value)
  {
    IODynamicConfigMemory::Memory data=tconn_->data_;
    tut::ensure_equals("invalid value", data[key], value );
  }

  TestConnection                        *tconn_;    // TODO: memory leak - use scoped_ptr<> for this
  Persistency::IO::ConnectionPtrNN       conn_;
  BackendFacade::ChangedNodes            changed_;
  BackendFacade                          bf_;
  TimeoutedSet                           ts_;
  Persistency::IO::DynamicConfig::Owner  owner_;
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Filter/NewEvent/TimeoutedSet");
} // unnamed namespace

namespace tut
{

// check adding simple element
template<>
template<>
void testObj::test<1>(void)
{
  Hash hash("key"); // TODO: add const
  ts_.add(hash);
  ensure("Element not present in collection", ts_.isTimeouted(hash));
}

// check addition from Entry's destructor
template<>
template<>
void testObj::test<2>(void)
{
  Hash  hash("key");
  {
    EntrySharedPtr entry(new Entry(hash, bf_, ts_));
  }
  ensure("Element not present in collection", ts_.isTimeouted(hash));
}

// check pruning element not saved in Dynamic Config
template<>
template<>
void testObj::test<3>(void)
{
  Hash hash("hash");
  ts_.add(hash);
  ensure("Element not present in collection", ts_.isTimeouted(hash));
  ts_.markRemoved(bf_, owner_);
  ensure("Element present in collection after prune", ts_.isTimeouted(hash) == false);
}

// check pruning element saved in Dynamic Config
template<>
template<>
void testObj::test<4>(void)
{
  std::string hashStr;              // TODO: add const
  Hash        hash("some key");     // TODO: add const
  {
    EntrySharedPtr  entryPtr(new Entry(hash, bf_, ts_));
    hashStr = string(entryPtr.get()->getHash().get());
  }
  // test if hash is in the TimeoutedSet
  testData( hashStr, string("true") );
  // clear timeouted set
  ts_.markRemoved(bf_, owner_);
  testData( hashStr, string("") );
}

// TODO: test if no exceptions are thrown from inside the 'makrRemoved()' call.

} // namespace tut
