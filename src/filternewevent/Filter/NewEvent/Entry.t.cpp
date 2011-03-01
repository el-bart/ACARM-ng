/*
 * Entry.t.cpp
 *
 */

#include <tut.h>

#include "Filter/NewEvent/Entry.hpp"
#include "Filter/NewEvent/TimeoutedSet.hpp"
#include "Persistency/IO/BackendFactory.hpp"
#include "Persistency/IO/DynamicConfig.hpp"
#include "Filter/NewEvent/TestConnection.t.hpp"

using namespace std;
using namespace Filter;
using namespace Filter::NewEvent;

namespace
{

struct TestClass
{

  TestClass(void):
    conn_( createUserStub() ),
    bf_(conn_, changed_, "testnewevent"),
    owner_("Filter::NewEvent")
  {
  }

  Persistency::IO::ConnectionPtrNN      conn_;
  BackendFacade::ChangedNodes           changed_;
  BackendFacade                         bf_;
  TimeoutedSet                          ts_;
  Persistency::IO::DynamicConfig::Owner owner_;
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Filter/NewEvent/Entry");
} // unnamed namespace

namespace tut
{

// TODO
template<>
template<>
void testObj::test<1>(void)
{
  Entry e("key", &bf_, &ts_);
  Persistency::IO::DynamicConfigAutoPtr dc(e.getDynamicConfig());
  Persistency::IO::DynamicConfig::ValueNULL v=dc->read( e.getHash() );
  ensure("NULL value read", v.get()!=NULL );
  ensure_equals("invalid value", v.get()->get(), string("true") );
}

} // namespace tut
