/*
 * InterfaceImpl.t.cpp
 *
 */
#include <tut.h>

#include "Core/Types/Proc/InterfaceImpl.hpp"
#include "Persistency/Stubs/TestHelpers.hpp"
#include "TestHelpers/Persistency/TestStubs.hpp"

using namespace Core::Types::Proc;
using namespace Persistency;
using namespace Persistency::Stubs;

namespace
{

struct TestStrategyParams
{
  mutable int calls_;
}; // struct TestStrategyParams

struct TestStrategy
{
  explicit TestStrategy(const TestStrategyParams &p):
    p_(p)
  {
    p_.calls_=0;
  }

  void process(GraphNodePtrNN, Interface::ChangedNodes&)
  {
    ++p_.calls_;
  }

  const TestStrategyParams &p_;
};


namespace
{
int testStrategyNoParmCalls=0;
} // unnamed namespace

struct TestStrategyNoParm
{
  TestStrategyNoParm(void)
  {
    testStrategyNoParmCalls=0;
  }

  void process(GraphNodePtrNN, Interface::ChangedNodes&)
  {
    ++testStrategyNoParmCalls;
  }
};


struct TestClass: private TestHelpers::Persistency::TestStubs
{
  TestClass(void):
    impl_("somename", params_)
  {
  }

  TestStrategyParams                              params_;
  InterfaceImpl<TestStrategy, TestStrategyParams> impl_;
};

typedef TestClass TestClass;
typedef tut::test_group<TestClass> factory;
typedef factory::object testObj;

factory tf("Core/Types/Proc/InterfaceImpl");
} // unnamed namespace


namespace tut
{

// test getting name
template<>
template<>
void testObj::test<1>(void)
{
  ensure_equals("invalid name set", impl_.getName(), "somename");
}

// test passing call to process
template<>
template<>
void testObj::test<2>(void)
{
  ensure_equals("pre-condition failed", params_.calls_, 0);
  Interface::ChangedNodes changed;
  impl_.process( makeNewLeaf(), changed );
  ensure_equals("process() is not virtual", params_.calls_, 1);
}

// test test 1-arg c-tor (should compile)
template<>
template<>
void testObj::test<3>(void)
{
  InterfaceImpl<TestStrategyNoParm> tmp("sometest");
}

// test process() on non-param strategy object
template<>
template<>
void testObj::test<4>(void)
{
  InterfaceImpl<TestStrategyNoParm> tmp("sometest");
  ensure_equals("pre-condition failed", testStrategyNoParmCalls, 0);
  Interface::ChangedNodes changed;
  tmp.process( makeNewLeaf(), changed );
  ensure_equals("process() is not virtual", testStrategyNoParmCalls, 1);
}

} // namespace tut
