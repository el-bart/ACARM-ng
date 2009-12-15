/*
 * MetaAlert.t.cpp
 *
 */
#include <tut.h>
#include <string>

#include "Persistency/MetaAlert.hpp"
#include "Persistency/TestHelpers.t.hpp"

using namespace std;
using namespace Persistency;

namespace
{

struct TestClass
{
  TestClass(void):
    url_( makeNewReferenceURL() ),
    ma_( "name 1", 42, 4.2, url_, ts_)
  {
  }

  ReferenceURLPtrNN url_;
  const Timestamp   ts_;
  MetaAlert         ma_;
};

typedef TestClass TestClass;
typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Persistency/MetaAlert");
} // unnamed namespace


namespace tut
{

// test creting meta-alert from alert
template<>
template<>
void testObj::test<1>(void)
{
  AlertPtr  alert=makeNewAlert();
  MetaAlert ma(alert);
  ensure_equals("invalid name", string( ma.getName().get() ),
                                alert->getName().get() );
  ensure_equals("invalid severity delta", ma.getSeverityDelta(), 0);
  ensure_equals("invalid certanity delta", ma.getCertanityDelta(), 0);
  ensure("invalid reference URL", ma.getReferenceURL()==NULL );
  ensure("invalid creation time",
         ma.getCreateTime()==alert->getCreationTime() );
}

// test getting name
template<>
template<>
void testObj::test<2>(void)
{
  ensure_equals("invalid name", ma_.getName().get(), string("name 1") );
}

// test getting severity delta
template<>
template<>
void testObj::test<3>(void)
{
  ensure_equals("invalid severity delta", ma_.getSeverityDelta(), 42);
}

// test getting ceratnity delta
template<>
template<>
void testObj::test<4>(void)
{
  ensure_equals("invalid certanity delta", ma_.getCertanityDelta(), 4.2);
}

// test reference url getting
template<>
template<>
void testObj::test<5>(void)
{
  ensure("invalid reference URL", ma_.getReferenceURL()==url_.get() );
}

// test creation time
template<>
template<>
void testObj::test<6>(void)
{
  ensure("invalid creation time", ma_.getCreateTime()==ts_ );
}

} // namespace tut
