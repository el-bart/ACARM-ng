/*
 * Summary.t.cpp
 *
 */
#include <tut.h>
#include <cassert>

#include "Trigger/Compose/Summary.hpp"
#include "TestHelpers/Persistency/TestHelpers.hpp"
#include "TestHelpers/Persistency/TestStubs.hpp"

using namespace Trigger::Compose;
using namespace Persistency;
using namespace TestHelpers::Persistency;

namespace
{

struct TestClass: private TestHelpers::Persistency::TestStubs
{
};

typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Trigger/Compose/Summary");
} // unnamed namespace


namespace tut
{

// test creating report
template<>
template<>
void testObj::test<1>(void)
{
  std::stringstream ss;
  Summary::append( ss, makeNewNode() );
  ensure("invalid report", strstr( ss.str().c_str(), "reporting triggered for meta-alert \"")!=0 );
}

} // namespace tut
