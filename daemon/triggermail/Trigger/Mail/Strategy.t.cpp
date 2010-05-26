/*
 * Strategy.t.cpp
 *
 */
#include <tut.h>

#include "Trigger/Mail/Strategy.hpp"
#include "Trigger/Mail/TestAccount.t.hpp"
#include "TestHelpers/Persistency/TestHelpers.hpp"
#include "TestHelpers/Persistency/TestStubs.hpp"

using namespace Trigger::Mail;
using namespace Persistency;
using namespace TestHelpers::Persistency;

namespace
{

struct TestClass: private TestHelpers::Persistency::TestStubs
{
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Trigger/Mail/Strategy");
} // unnamed namespace


namespace tut
{

// check if name has been saved correctly
template<>
template<>
void testObj::test<1>(void)
{
  Strategy s( getTestConfig1() );
  ensure_equals("invalid name", s.getTriggerName(), "mail");
}

// test sending report
template<>
template<>
void testObj::test<2>(void)
{
  // wipe-out account's content
  removeMessagesFromAccount( getTestConfig2() );
  // send report
  Strategy               s( getTestConfig1() );
  Strategy::ChangedNodes nc;
  s.process( makeNewNode(), nc );
  // check results
  const int count=removeMessagesFromAccount( getTestConfig2(), 1 );
  ensure_equals("invalid number of messages removed", count, 1);
}

} // namespace tut
